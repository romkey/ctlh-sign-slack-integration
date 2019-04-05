# coding: utf-8
require 'mqtt'
require 'json'
require 'date'
require 'timeout'
require 'tzinfo'
require 'pp'

WATER_HEIGHT = 62

def aqua_command(username, channel_name, arguments)
  mqtt = MQTT::Client.connect(host: ENV['MQTT_HOST'], port: ENV['MQTT_PORT'], ssl: ENV['MQTT_SSL'], username: ENV['MQTT_USERNAME'], password: ENV['MQTT_PASSWORD'])

  begin
    Timeout.timeout(0.5) do
      topic, msg = mqtt.get '/aqua'
      mqtt.disconnect

      json = JSON.parse msg
      
      colors_total = (json["red"] + json["green"] + json["blue"]).to_f

      dt = DateTime.strptime(json["timestamp"].to_s,'%s')
      tz = TZInfo::Timezone.get('US/Pacific')

      <<END_OF_RESPONSE
*Hydroponics status*
Air temperature is #{json["temperature"]}°C/#{json["temperature"]*9/5+32}°F, humidity #{json["humidity"]}% and pressure #{json["pressure"]/100} millibars

Water level has dropped #{WATER_HEIGHT - json["distance"]} cm. Water temperature is #{json["water_temperature"]}°C/#{json["water_temperature"]*9/5+32}°F. Water pH is #{json["ph"]}

Light is #{json["lux"]} lux - red: #{(json["red"]/colors_total*100).to_i}%, green: #{(json["green"]/colors_total*100).to_i}%, blue: #{(json["blue"]/colors_total*100).to_i}%

Last update at #{dt.getlocal(tz.current_period.offset.utc_total_offset).to_s}. Sensors have been up for #{json["uptime"]} seconds.
END_OF_RESPONSE
    end
  rescue
    "No response"
  end
end

# coding: utf-8
require 'mqtt'
require 'json'
require 'date'
require 'timeout'
require 'pp'

def projectors_command(username, channel_name, cmd)
  mqtt = MQTT::Client.connect(host: ENV['MQTT_HOST'], port: ENV['MQTT_PORT'], ssl: ENV['MQTT_SSL'], username: ENV['MQTT_USERNAME'], password: ENV['MQTT_PASSWORD'])

  if cmd != ''
      mqtt.publish('/projectors/cmd', cmd)
      mqtt.disconnect
      return
  end

  begin
    Timeout.timeout(0.5) do
      topic, msg = mqtt.get '/projectors'
      mqtt.disconnect

      json = JSON.parse msg
      
      dt = DateTime.strptime(json["timestamp"].to_s,'%s')
      proj0 = json["projectors"][0]
      proj1 = json["projectors"][1]
      <<END_OF_RESPONSE
*Projectors status*
#{proj0["name"]} #{proj0["status"]}  #{proj0["temperatures"].map { |t| "#{t}°C/#{t*9/5+32}°F" }.join(', ') }
#{proj1["name"]} #{proj1["status"]}  #{proj1["temperatures"].map { |t| "#{t}°C/#{t*9/5+32}°F" }.join(', ') }

Last update at #{dt.to_s} UTC.
END_OF_RESPONSE
    end
  rescue
        "No response"
  end
end

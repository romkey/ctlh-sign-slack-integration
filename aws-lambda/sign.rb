$LOAD_PATH.unshift('./vendor')

require 'mqtt'
require 'color'

def sign_command(username, channelname, cmd)

  case cmd
  when 'help'
    reply = help
  when /([0-9|a-f|A-F]){3}/
    message ="C#{cmd}"
    reply = "Sign set to #{cmd}"
  when 'on'
    message = 'Cffffff'
    reply = 'Sign turned on'
  when 'off'
    message = 'C000000'
    reply = 'Sign turned off'
  when 'morse'
    message = "M"
    reply = '-- --- .-. ... . / -.-. --- -.. . / ... . -. -'
  when 'restart'
    message = 'B'
    reply = 'Reboot request sent to controller'
  when 'reboot'
    message = 'B'
    reply = 'Reboot request sent to controller'
  else
    begin
      rgb = Color::RGB.by_name cmd
    rescue
    end
    if rgb
      red = "%02x" % rgb.red.to_i
      green = "%02x" % rgb.green.to_i
      blue = "%02x" % rgb.blue.to_i

      message = "C#{red}#{green}#{blue}"
      reply = "Setting sign to #{red}#{green}#{blue}"
    else
      reply = "Unknown command #{cmd}\n\n" + help
    end
  end
  
  if message
    MQTT::Client.connect(host: ENV['MQTT_HOST'], port: ENV['MQTT_PORT'], ssl: ENV['MQTT_SSL'], username: ENV['MQTT_USERNAME'], password: ENV['MQTT_PASSWORD']) do |c|
      c.publish('signrgb', message)
    end
  end

  reply
end

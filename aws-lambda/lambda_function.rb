$LOAD_PATH.unshift('./vendor')

require 'json'
require 'net/http'
require 'cgi'
require 'mqtt'
require 'color'

def help
  <<END_OF_HELP
usage: /sign on | off | R G B | hexRGB | possibly-some-other-stuff
examples:
  /sign b784a7
  /sign 183 132 167
  /sign 0xb7 0x84 0xa7
  
Code is at https://github.com/romkey/ctlh-sign-slack-integration
END_OF_HELP
end

def lambda_handler(event:, context:)
  if ENV['IFTTT_API_KEY']
    str = Net::HTTP.get('maker.ifttt.com', "/trigger/dustball/with/key/#{ENV['IFTTT_API_KEY']}")
  end

  command = URI.decode(event["body"]).split('&')
  params = CGI::parse(URI.decode(event["body"]))
  cmd = params["text"][0]

  case cmd
  when 'help'
    reply = help
  when /([0-9|a-f|A-F]){6}/
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
  else
    begin
      rgb = Color::RGB.by_name cmd
    rescue
    end
    if rgb
      red = rgb.red.to_i.to_s(16)
      green = rgb.green.to_i.to_s(16)
      blue = rgb.blue.to_i.to_s(16)

      message = "C#{red}#{green}#{blue}"
      reply = "Setting sign to #{red}#{green}#{blue}"
    else
      reply = "Unknown command\n\n" + help
    end
  end
  
  if message
    MQTT::Client.connect(host: ENV['MQTT_HOST'], port: ENV['MQTT_PORT'], ssl: ENV['MQTT_SSL'], username: ENV['MQTT_USERNAME'], password: ENV['MQTT_PASSWORD']) do |c|
      c.publish('signrgb', message)
    end
  end
    
  { statusCode: 200, body: reply }
end

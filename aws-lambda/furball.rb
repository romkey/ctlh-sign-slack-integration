$LOAD_PATH.unshift('./vendor')

require 'cgi'
require 'mqtt'

def furball_command(body)
  if message
    MQTT::Client.connect(host: ENV['MQTT_HOST'], port: ENV['MQTT_PORT'], ssl: ENV['MQTT_SSL'], username: ENV['MQTT_USERNAME'], password: ENV['MQTT_PASSWORD']) do |c|
      c.publish('signrgb', message)
    end
end

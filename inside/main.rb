require 'mqtt'

client = MQTT::Client.connect(host: ENV['MQTT_HOST'], port: ENV['MQTT_PORT'], ssl: ENV['MQTT_SSL'], username: ENV['MQTT_USERNAME'], password: ENV['MQTT_PASSWORD'])

client.subscribe 'signrgb'

client.get('signrgb') do |topic,message|
  
end

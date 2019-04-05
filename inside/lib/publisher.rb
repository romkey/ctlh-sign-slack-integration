require 'mqtt'
require 'projector'
require 'json'

class Publisher
  def initialize(options)
    @options = options
    @p1 = Projector.new("192.168.15.38")
    @p2 = Projector.new("192.168.15.39")
  end

  def connect
    @mqtt = MQTT::Client.connect(host: @options['MQTT_SERVER'] || ENV['MQTT_SERVER'],
                                 port: @options['MQTT_PORT'] || ENV['MQTT_PORT'],
                                 username: @options['MQTT_USERNAME'] || ENV['MQTT_USERNAME'],
                                 password: @options['MQTT_PASSWORD'] || ENV['MQTT_PASSWORD'])
  end

  def run!
    connect

    loop do
      begin
        temperatures1 = @p1.temperatures
      rescue
        tempeartures1 = [ nil, nil, nil ]
      end

      begin
        temperatures2 = @p2.temperatures
      rescue
        tempeartures2 = [ nil, nil, nil ]
      end

      sleep 1

      begin
        status1 = @p1.status
      rescue
        status1 = nil
      end

      begin
        status2 = @p2.status
      rescue
        status2 = nil
      end

      projector_info = {
        projectors: [ { name: "main room", temperatures: temperatures1, status: status1 },
                      { name: "crafts room", temperatures: temperatures2, status: status2 } ],
        timestamp: Time.now.to_i
      }

      puts JSON.generate(projector_info)
      @mqtt.publish("/projectors", JSON.generate(projector_info), true)

      sleep 60
    end
  end
end

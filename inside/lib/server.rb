require 'mqtt'
require 'sign'
require 'projector'
require 'buckets'

class Server
  def initialize(options)
    @options = options
  end

  def connect
    @client = MQTT::Client.connect(host: @options['MQTT_HOSTNAME'],
                                 port: @options['MQTT_PORT'],
                                 username: @options['MQTT_USERNAME'],
                                 password: @options['MQTT_PASSWORD'])
    puts 'connected'
  end

  def run!
    loop do
      connect

      @client.subscribe 'signrgb'
      @client.subscribe 'projector/cmd'
      @client.subscribe 'buckets/cmd'

      @client.get('#') do |topic,message|
        puts "topic: #{topic}, msg #{message}"

        case topic
        when 'signrgb'
          Sign.command message
        when '/projectors/cmd'
          Projector.command message
        when 'buckets/cmd'
          Buckets.command message
        end
      end
    end
  end

end

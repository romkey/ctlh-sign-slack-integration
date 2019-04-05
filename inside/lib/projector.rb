#require 'pj_link'
require 'socket'

class Projector
  def initialize(ip_address)
    @ip_address = ip_address
  end
  
  def _cmd!(cmd)
    t = TCPSocket.new @ip_address, 10000

    while data = t.recv(1024)
      break if data.include? 'WORD'
    end

    t.write "\r\n"

    while data = t.recv(1024)
      break if data.include? 'Hello'
    end

    t.write "#{cmd}\r\n"

    results = t.recv(1024)
    t.close

    puts "_cmd -> #{results}"

    results.chomp
  end

  def self.command(message)
    puts "Projector.command #{message}"

    case message
    when 'on'
      p = Projector.new '192.168.15.38'
      p.on
    when 'off'
      p = Projector.new '192.168.15.38'
      p.off
    end
  end

  def on
    _cmd! 'C00'
  end

  def off
    _cmd! 'C01'
  end

  def temperatures
    temps = _cmd! 'CR6'
    temps.lstrip.split(/\s+/).map { |t| t.to_f }
  end

  def model
    _cmd! 'CR5'
  end

  def status
    status = _cmd! 'CR0'

    case status
    when '00'
      'Power on'
    when '80'
      'Standby'
    when '40'
      'Countdown'
    when '20'
      'Cooling'
    when '10'
      'Power malfunction'
    when '28'
      'Cooling down at the temperature anomaly'
    when '24'
      'Cooling down at Power Management mode'
    when '04'
      'Power management mode after cooling down'
    when '21'
      'Cooling down after the projector is turned off when the lamps are out'
    when '81'
      'Stand-by mode after Cooling down when the lamps are out.'
    when '88'
      'Stand-by mode after Cooling down at the temperature  anomaly'
    when '2C'
      'Cooling down when the projector is turned off by the shutter management function.'
    when '8C'
      'Stand-by mode after cooling down by the shutter management function.'
    else
      "Unknown status #{status}"
    end
  end
end

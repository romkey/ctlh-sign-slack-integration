require 'net/http'
require 'uri'

class Sign
  def self.cmd(red, green, blue)
    url = "http://192.168.15.99/?r=#{red}&g=#{green}&b=#{blue}"

    puts url

    uri = URI.parse url
    res = Net::HTTP.get uri
#    `curl #{url}`
  end

  def self.command(message)
    m = message.match /^C([\dabcdef]{2})([\dabcdef]{2})([\dabcdef]{2})$/i
    return unless m

    red = m[1]
    green = m[2]
    blue = m[3]

    Sign.cmd(red, green, blue)
  end
end

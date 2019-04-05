require 'net/http'
require 'uri'

def sign(red, green, blue)
  url = "http://192.168.15.99/?r=#{red}&g=#{green}&b=#{blue}"

  uri = URI.parse url

  Net::HTTP.get  uri
end


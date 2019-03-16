# coding: utf-8
$LOAD_PATH.unshift('./vendor')

require 'json'

def k_to_f(temp)
  ((temp - 273.15) * 9/5 + 32).to_i
end

def weather_command(username, channelname, cmd)
  options = { units: "metric", APPID: ENV['OPENWEATHERMAP_APPID'] }

  response = Net::HTTP.get_response('api.openweathermap.org', "/data/2.5/weather?lat=45.57815&lon=-122.681693&APPID=#{ENV['OPENWEATHERMAP_APPID']}")
  if response.is_a?(Net::HTTPSuccess)
    current = JSON.parse response.body
    "Currently #{current['weather'][0]['main'].downcase}, #{k_to_f(current['main']['temp'].to_f)}°F and #{current['main']['humidity']}% humidity, with temperatures ranging from #{k_to_f(current['main']['temp_min'].to_f)}°F to #{k_to_f(current['main']['temp_max'].to_f)}°F.\n\nWeather data from https://openweathermap.org/"
  else
    "Network failure while trying to contact openweathermap.org: #{response.message} - #{url}"
  end
end

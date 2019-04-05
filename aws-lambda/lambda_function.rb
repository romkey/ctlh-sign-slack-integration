$LOAD_PATH.unshift('./vendor')

require 'json'
require 'cgi'
require 'net/http'
require './sign'
require './projector'
require './aqua'
require './buckets'
require './weather'

def help
  <<END_OF_HELP
usage: /sign on | off | R G B | hexRGB | possibly-some-other-stuff
examples:
  /sign b784a7

Note that setting the sign too bright can make it flicker.
  
Code is at https://github.com/romkey/ctlh-sign-slack-integration
END_OF_HELP
end

# add these lines to help once they're implemented
#  /sign 183 132 167
#  /sign 0xb7 0x84 0xa7

def lambda_handler(event:, context:)
  if ENV['IFTTT_API_KEY']
    str = Net::HTTP.get('maker.ifttt.com', "/trigger/ctlh-sign/with/key/#{ENV['IFTTT_API_KEY']}")
  end

  command = URI.decode(event["body"]).split('&')
  params = CGI::parse(URI.decode(event["body"]))
  
  username = params["user_name"][0]
  channel_name = params["channel_name"][0]
  command = params["command"][0]
  arguments = params["text"][0]
  
  case command
  when '/sign'
    reply = sign_command username, channel_name, arguments
  when '/weather'
    reply = weather_command username, channel_name, arguments
  when '/hydro'
    reply = aqua_command username, channel_name, arguments
  when '/planthacker'
    reply = aqua_command username, channel_name, arguments
  when '/projectors'
    reply = projectors_command username, channel_name, arguments
  when '/projector'
    reply = projectors_command username, channel_name, arguments
  when '/proj'
    reply = projectors_command username, channel_name, arguments
  end

  reply = "No response" if reply.nil?
  reply += "\n\n_Want to hack this?_ Source is at https://github.com/romkey/ctlh-slack-integrations"

  { statusCode: 200, body: reply }
end

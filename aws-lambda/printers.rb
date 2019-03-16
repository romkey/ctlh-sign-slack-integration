$LOAD_PATH.unshift('./vendor')

require 'cgi'
require 'mqtt'
require 'color'

def printers_command(body)
  mqtt.subscribe "/printers"

  reply 
end

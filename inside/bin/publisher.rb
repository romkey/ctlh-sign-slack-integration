#!/usr/bin/env ruby

$LOAD_PATH.unshift( File.expand_path('./lib'))

require 'optparse'
require 'dotenv/load'

require 'publisher'

options        = {}

options['MQTT_HOSTNAME'] = ENV['MQTT_HOSTNAME']
options['MQTT_PORT'] = ENV['MQTT_PORT']
options['MQTT_USERNAME'] = ENV['MQTT_USERNAME']
options['MQTT_PASSWORD'] = ENV['MQTT_PASSWORD']


version        = "0.0.1"
daemonize_help = "run daemonized in the background (default: false)"
pidfile_help   = "the pid filename"
logfile_help   = "the log filename"
include_help   = "an additional $LOAD_PATH"
debug_help     = "set $DEBUG to true"
warn_help      = "enable warnings"

op = OptionParser.new
op.banner =  "An example of how to daemonize a long running Ruby process."
op.separator ""
op.separator "Usage: server [options]"
op.separator ""

op.separator "Process options:"
op.on("-d", "--daemonize",   daemonize_help) {         options[:daemonize] = true  }
op.on("-p", "--pid PIDFILE", pidfile_help)   { |value| options[:pidfile]   = value }
op.on("-l", "--log LOGFILE", logfile_help)   { |value| options[:logfile]   = value }
op.separator ""

op.separator "Ruby options:"
op.on("-I", "--include PATH", include_help) { |value| $LOAD_PATH.unshift(*value.split(":").map{|v| File.expand_path(v)}) }
op.on(      "--debug",        debug_help)   { $DEBUG = true }
op.on(      "--warn",         warn_help)    { $-w = true    }
op.separator ""

op.separator "Common options:"
op.on("-h", "--help")    { puts op.to_s; exit }
op.on("-v", "--version") { puts version; exit }
op.separator ""

op.parse!(ARGV)

Publisher.new(options).run!

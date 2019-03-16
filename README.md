# Slack Integration for CTL-H Sign

This repo contains the code which allows a Slack user to issue commands like `/sign 0 0 255` to light up the CTL-H sign in all green.

Building a Slack "/" command is straightforward. You configure Slack to post to a URL when someone issues the command. Slack passes the endpoint JSON giving information about the request (who issued it, any parameters, etc).

The CTL-H sign is on CTL-H's network and can easily be controlled by passing RGB values to another API endpoint on the sign's controller. This controller is not accessible from the Internet. So the problem becomes, how do we communicate between Slack and the sign controller, preferably for free?

This solution uses an AWS Lambda function which decodes the Slack request and the posts to an MQTT broker. A small device lives on the CTL-H network and subscribes to the MQTT broker, waiting requests. It then posts the requests to the sign's controller.

There are three parts to the Slack integration:

1. Slack must be configured to know about the `/sign` command. Slack will automatically trigger a configured webhook when this command is run.

2. An AWS Lambda function is triggered by the webhook that Slack pings when the `/sign` command is run. This function will publish the parameters to an MQTT broker.

3. Code running on an ESP8266 sitting on CTL-H's network, which subscribes to the MQTT broker and updates the sign.


## About the Sign

The CTL-H sign has a controller which is reachable only on the CTL-H network. You can set its colors by going to `http://192.168.15.99/?r=R&g=G&b=B` where R, G and B are red, green and blue intensities from 0 to 255.

The sign controller is not accessible from outside of CTL-H's network.

## Cloud MQTT

First we create a free tier ("Active Cat") account on [CloudMQTT](https://cloudmqtt.com). The free tier allows 5 simultaneous connections and 10Kbps of bandwidth through the broker. This should be more than enough for controlling the sign through Slack. Beware, it's easy to mismanage connections so that the five are consumed by connections that haven't timed out yet. Be careful to disconnect when done. If done properly there should only ever be two connections active at once - one from the AWS Lambda function (if active) and one from the on-site ESP8266.

You'll need the server name, port number, username and password for the next steps.

## AWS Lambda

AWS Lambda allows you to run a small piece of code for a short time without deploying or provisioning an entire server for it. Lamba supports C#, Go, Java, NodeJS, Python and Ruby. Since I'm most proficient with Ruby I decided to stick with it.

Ruby requires an external gem to talk to an MQTT server, so the main challenge here is making the gem available to the Lambda code. The easiest way is to use bundler to download the gem, then zipping up the whole project and uploading it to AWS as the Lambda.

To set this up, sign into AWS Lambda and create a new function:
- Name: doesn't matter, I choose "CTLH-Slack-Sign-Integration"
- Runtime: Ruby 2.5 or whatever the latest version of Ruby is
- Role: "Create a new role from one or more templates"
- Policy Templates: "Simple microservice permissions"

Then download the gems in the `aws-lambda` directory by running:
```bundle install --deployment```

Zip the files in the directory and upload the zip file as the lambda function.


## Slack Configuration

The final step! This is where you'll tell Slack that you're adding a `/` command and give it the API endpoint to call when the command is issued.

You'll need the AWS API endpoint from the previous step.

# Limitations

We're using the free tier of four commercial services:
- AWS Lambda
- AWS API
- CloudMQTT
- Slack

## AWS Lambda Limitations

The AWS Lambda code is the least hackable part of the project - modifying it requires access to my AWS account. It's also where most of the work happens - it parses the commands from Slack and translates them into simpler operations that the ESP8266 handles. The ESP8266 is much more hackable - anyone with physical access to it can update the code running on it. It would be nice if the AWS Lambda code were simply glue that shuttled info back and forth between Slack and the ESP8266.

Because we want to keep the AWS Lambda code running for as little time as possible, we don't currenlty retrieve any kind of status from the ESP8266. The ESP8266 could publish its status to CloudMQTT after an operation and the Lambda code could wait for it and return the status to Slack. That would make the code more complex and keep the Lambda running for longer.

Our use should easily fit within the AWS Lambda free tier, which as of this moment will be perpetual.

## AWS API

Unfortunately the free tier for AWS API is only available for the its first year of use. After that (at this time) it will cost $3.50/month. 

## CloudMQTT

We have much less headroom with the free tier of CloudMQTT. The free tier allows 10Kbps of MQTT bandwidth and no more than 5 simultaneous clients. This means it's not appropriate for frequently moving lots of data, and that we shouldn't expect to connect many ESP8266's to it. We should stuff as much functionality in a single ESP8266 as possible in order to not hit the limit on simulaenous clients.

## Slack Limitations

Free Slack teams allow a maximum of ten apps, so we want to combine all of ^H's Slack integrations into a single app as much as possible.

# ESP8266

The ESP8266 sits at CTL-H and subscribes to topics on the MQTT server. It inteprets data it receives on the topics it listens to and uses that to take actions inside CTL-H, like change the RGB settings of the sign.

The ESP8266 is running an Arduino program that's built and uploaded using PlatformIO. You can also build it using the Arduino IDE but you'll need to do some renaming.

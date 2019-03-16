#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#endif

#ifdef ESP32
#include <HTTPClient.h>
#endif

#include <IPAddress.h>

#include "projector.h"
#include "notify.h"

#include "config.h"

#define PROJECTOR_URL_MAX_LENGTH sizeof("http:///?r=&g=&b=") + 3*3 + 3*4 + 3 +1
#define TELNET_PORT 23

typedef struct {
  const char* command;
  const char* telnet_command;
  const char* http_command;
} projector_command_t;

void projector_slack_command(const char* command) {
  unsigned length = strlen(command);

  if(command[0] == 'C') {
  } else {
    Serial.printf("invalid color length %d\n", length);
  }
}

bool projector_telnet(char* command) {
  WiFiClient telnet;
  IPAddress projector;

  projector.fromString(PROJECTOR_ADDRESS);

  telnet.connect(projector, TELNET_PORT);
  telnet.print("\r");

  while(telnet.connected() || telnet.available()) {
    if(telnet.available()) {
      String line = telnet.readStringUntil('\r');

      if(line == "HELLO\r")
	break;
      else {
	telnet.stop();
	return false;
      }
    }
  }
    telnet.print(command);
    telnet.print("\r");
    
    telnet.stop();
}

bool projector_http(const char* command) {
  char buf[PROJECTOR_URL_MAX_LENGTH];
  bool outcome = true;

  snprintf(buf, PROJECTOR_URL_MAX_LENGTH, "http://%s/%s", SIGN_ADDRESS, command);

  Serial.print("SIGN URL ");
  Serial.println(buf);

  // actually control the sign here
  HTTPClient http;

  http.begin(buf);
  int httpCode = http.GET();
  if(httpCode < 0) {
    notify_error(String("http error") + buf + String(httpCode));
    outcome = false;
  }

  http.end();

  return outcome;
}

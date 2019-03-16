#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#endif

#ifdef ESP32
#include <HTTPClient.h>
#endif

#include "sign.h"
#include "led.h"
#include "notify.h"

#include "config.h"

#define SIGN_URL_MAX_LENGTH sizeof("http:///?r=&g=&b=") + 3*3 + 3*4 + 3 +1

void sign_slack_command(const char* command) {
  unsigned length = strlen(command);

  if(command[0] == 'C') {
    if(length == 7) {
      uint8_t red, green, blue;
      uint32_t number = strtol(&command[1], NULL, 16);

      red = number >> 16;
      green = number >> 8 & 0xFF;
      blue = number & 0xFF;
    
#ifdef ESP8266
      led_control(red, green, blue);
#endif

      sign_control(red, green, blue);
    } else {
      Serial.printf("invalid color length %d\n", length);
    }
  }
}

bool sign_control(uint8_t red, uint8_t green, uint8_t blue) {
  char buf[SIGN_URL_MAX_LENGTH];
  bool outcome = true;

  snprintf(buf, SIGN_URL_MAX_LENGTH, "http://%s/?r=%d&g=%d&b=%d", SIGN_ADDRESS, red, green, blue);

  Serial.printf("CONTROL_SIGN %02x, %02x, %02x\n", red, green, blue);
  Serial.print("SIGN URL ");
  Serial.println(buf);

  // actually control the sign here
  HTTPClient http;

  http.begin(buf);
  int httpCode = http.GET();
  if(httpCode < 0) {
    Serial.printf("sign_control() http.GET() -> %d\n", httpCode);
    notify_error(String("http error") + buf + String(httpCode));
    outcome = false;
  } else {
    Serial.println("sign_control() http.GET() SUCCESS!");
  }

  http.end();

  return outcome;
}

void sign_debug() {
  Serial.println("SIGN sequence");
  Serial.println("...off");

#ifdef ESP8266
  led_control(0, 0, 0);
#endif

  delay(10000);
  Serial.println("...red");

#ifdef ESP8266
  led_control(255, 0, 0);
#endif

  delay(10000);
  Serial.println("...green");

#ifdef ESP8266
  led_control(0, 255, 0);
#endif

  delay(10000);
  Serial.println("...blue");

#ifdef ESP8266
  led_control(0, 0, 255);
#endif

  delay(10000);
  Serial.println("...white");

#ifdef ESP8266
  led_control(255, 255, 255);
#endif

  delay(10000);
  Serial.println("SIGN sequence done");
}

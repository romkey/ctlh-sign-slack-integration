#include "config.h"
#include "version.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>

WiFiClient wifi_client;

void mqtt_callback(char* topic, byte* payload, unsigned int length);
void control_led(uint8_t red, uint8_t green, uint8_t blue);
void wifi_blink();

void debug_led();
void debug_sign();

Ticker wifi_connecting_led_ticker;

void setup() {
  delay(500);

  // yes, this is a weird baud rate. this is the speed that the ESP8266 bootloader outputs serial data at
  // so if we use this speed then the serial monitor can see both output from this application and from the bootloader
  Serial.begin(74880);
  Serial.print("trying to connect to wifi");

  Serial.println("connected");

  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  wifi_connecting_led_ticker.attach_ms(200, wifi_blink);

  WiFi.mode(WIFI_STA);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  wifi_connecting_led_ticker.detach();

  //  debug_led();
  Serial.println("[mqtt]");
}

void loop() {

}

void wifi_blink() {
  static bool onoff = 0;

  if(onoff) {
    onoff = 0;
    control_led(0, 0, 0);
  } else {
    onoff = 1;
    control_led(255, 0, 0);
  }
}

bool control_bucket(uint8_t red, uint8_t green, uint8_t blue) {
#define SIGN_URL_MAX_LENGTH sizeof("http:///?r=&g=&b=") + 3*3 + 3*4 + 3 +1
  char buf[SIGN_URL_MAX_LENGTH];

  snprintf(buf, SIGN_URL_MAX_LENGTH, "http://%s/?r=%d&g=%d&b=%d", SIGN_ADDRESS, red, green, blue);

  Serial.printf("CONTROL_SIGN %02x, %02x, %02x\n", red, green, blue);
  Serial.print("SIGN URL ");
  Serial.println(buf);

  // actually control the sign here
  HTTPClient http;

  http.begin(buf);
  int httpCode = http.GET();
  if(httpCode < 0)
    ifttt.trigger("http error", buf, String(httpCode).c_str());

  http.end();

  return true;
}

void control_led(uint8_t red, uint8_t green, uint8_t blue) {
  Serial.printf("CONTROL_LED %02x, %02x, %02x\n", red, green, blue);

  analogWrite(LED_RED_PIN, map(255 - red, 0, 255, 0, PWMRANGE));
  analogWrite(LED_GREEN_PIN, map(255 - green, 0, 255, 0, PWMRANGE));
  analogWrite(LED_BLUE_PIN, map(255 - blue, 0, 255, 0, PWMRANGE));
}

void mqtt_callback(char* command) {
  if(command[0] == 'C') {
    if(length == 7) {
      uint8_t red, green, blue;
      uint32_t number = strtol(&command[1], NULL, 16);

      red = number >> 16;
      green = number >> 8 & 0xFF;
      blue = number & 0xFF;
    
      control_led(red, green, blue);
      control_sign(red, green, blue);
    } else {
      Serial.printf("invalid color length %d\n", length);
    }
  }

  Serial.println(command);
}

void debug_led() {
  Serial.println("LED sequence");
  Serial.println("...off");
  control_led(0, 0, 0);
  delay(5000);
  Serial.println("...red");
  control_led(255, 0, 0);
  delay(5000);
  Serial.println("...green");
  control_led(0, 255, 0);
  delay(5000);
  Serial.println("...blue");
  control_led(0, 0, 255);
  delay(5000);
  Serial.println("...white");
  control_led(255, 255, 255);
  delay(5000);

  Serial.println("...mixed");
  control_led(127, 50, 200);
  delay(5000);

  Serial.println("LED sequence done");
}

void debug_sign() {
  Serial.println("SIGN sequence");
  Serial.println("...off");
  control_led(0, 0, 0);
  delay(10000);
  Serial.println("...red");
  control_led(255, 0, 0);
  delay(10000);
  Serial.println("...green");
  control_led(0, 255, 0);
  delay(10000);
  Serial.println("...blue");
  control_led(0, 0, 255);
  delay(10000);
  Serial.println("...white");
  control_led(255, 255, 255);
  delay(10000);
  Serial.println("SIGN sequence done");
}

#if 0
void update_firmware() {
  ESPhttpUpdate("hostname", 80, "pathname");
}
#endif

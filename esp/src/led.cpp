#include <Arduino.h>

#ifdef ESP8266

#include "led.h"

#include "config.h"

void led_control(uint8_t red, uint8_t green, uint8_t blue) {
  Serial.printf("LED_CONTROL %02x, %02x, %02x\n", red, green, blue);

  analogWrite(LED_RED_PIN, map(255 - red, 0, 255, 0, PWMRANGE));
  analogWrite(LED_GREEN_PIN, map(255 - green, 0, 255, 0, PWMRANGE));
  analogWrite(LED_BLUE_PIN, map(255 - blue, 0, 255, 0, PWMRANGE));
}

void led_debug() {
  Serial.println("LED sequence");
  Serial.println("...off");
  led_control(0, 0, 0);
  delay(5000);
  Serial.println("...red");
  led_control(255, 0, 0);
  delay(5000);
  Serial.println("...green");
  led_control(0, 255, 0);
  delay(5000);
  Serial.println("...blue");
  led_control(0, 0, 255);
  delay(5000);
  Serial.println("...white");
  led_control(255, 255, 255);
  delay(5000);

  Serial.println("...mixed");
  led_control(127, 50, 200);
  delay(5000);

  Serial.println("LED sequence done");
}

#endif

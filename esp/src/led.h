#ifdef ESP8266

#pragma once

// functions for controlling an RGB LED indicator on the ESP

void led_control(uint8_t red, uint8_t green, uint8_t blue);
void led_debug();

#endif

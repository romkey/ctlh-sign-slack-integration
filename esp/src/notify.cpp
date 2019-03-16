#include <Arduino.h>

#include "config.h"

#include <IFTTTWebhook.h>

static IFTTTWebhook ifttt(IFTTT_TRIGGER_NAME, IFTTT_API_KEY);

void notify_info(const char* msg) {
  Serial.printf("NOTIFY INFO %s\n", msg);
  ifttt.trigger(msg);
}

void notify_info(String str) {
  notify_info(str.c_str());
}


void notify_error(const char* msg) {
  Serial.printf("NOTIFY ERROR %s\n", msg);
  ifttt.trigger(msg);
}

void notify_error(String str) {
  notify_info(str.c_str());
}


void notify_urgent(const char* msg) {
  Serial.printf("NOTIFY URGENT %s\n", msg);
  ifttt.trigger(msg);
}

void notify_urgent(String str) {
  notify_info(str.c_str());
}

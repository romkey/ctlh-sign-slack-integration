#include "config.h"
#include "version.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>
#endif

#include <Ticker.h>

#include <PubSubClient.h>

#include "sign.h"
#include "projector.h"
#include "buckets.h"
#include "aqua.h"
#include "furball.h"
#include "printers.h"
#include "led.h"

#include "notify.h"

#ifdef ESP8266
ESP8266WiFiMulti wifiMulti;
#endif

#ifdef ESP32
WiFiMulti wifiMulti;
#endif

static WiFiClient wifi_client;
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void wifi_blink();

static PubSubClient mqtt_client(MQTT_SERVER, MQTT_PORT, mqtt_callback, wifi_client);

#if 0
static Ticker wifi_connecting_led_ticker;
#endif

void setup() {
  delay(500);

#ifdef ESP8266
  // yes, this is a weird baud rate. this is the speed that the ESP8266 bootloader outputs serial data at
  // so if we use this speed then the serial monitor can see both output from this application and from the bootloader
  Serial.begin(74880);
#endif

#ifdef ESP32
  Serial.begin(115200);
#endif

  Serial.print("trying to connect to wifi");

#if 0
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  wifi_connecting_led_ticker.attach_ms(200, wifi_blink);
#endif

#ifdef ESP8266
  WiFi.hostname(HOSTNAME);
#endif

  WiFi.mode(WIFI_STA);

  wifiMulti.addAP(WIFI_SSID1, WIFI_PASSWORD1);
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
  wifiMulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);

  while(wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

#if 0
  wifi_connecting_led_ticker.detach();
#endif

  Serial.println("connected");

  MDNS.begin(HOSTNAME);

  notify_info("restart");

  //  debug_led();

  mqtt_client.connect("CTLHSignClient", MQTT_USERNAME, MQTT_PASSWORD);
  mqtt_client.setCallback(mqtt_callback);
  mqtt_client.subscribe("signrgb");
  Serial.println("[mqtt]");
}

void loop() {
  mqtt_client.loop();

  static unsigned long next_notify_heartbeat = 0;
  if(millis() > next_notify_heartbeat) {
    next_notify_heartbeat += 60 * 60 * 1000;

    notify_info(String("uptime ") + String(millis()) + ", free heap " + String(ESP.getFreeHeap()));
  }

  static unsigned long next_printers_update = 0;
  if(millis() > next_printers_update) {
    next_printers_update += 60 * 1000;

    printers_update();
  }

  static unsigned long next_heartbeat = 0;
  if(millis() > next_heartbeat) {
    next_heartbeat += HEARTBEAT_FREQUENCY;

    char heartbeat_msg[200];
    snprintf(heartbeat_msg, 200, "{\"uptime\": %lu, \"freeheap\": %u, \"src\": \"slack-integration-esp\" }", millis(), ESP.getFreeHeap());
    mqtt_client.publish("/heartbeat", heartbeat_msg, true);
  }
}

void wifi_blink() {
#ifdef ESP2866
  static bool onoff = 0;

  if(onoff) {
    onoff = 0;
    led_control(0, 0, 0);
  } else {
    onoff = 1;
    led_control(255, 0, 0);
  }
#endif
}


void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("got a callback!");
  Serial.println(length);
  Serial.println(topic);

  char command[32];
  length = constrain(length, 0, 32);

  memcpy(command, payload, length);

  length = constrain(length, 0, 31);
  command[length] = '\0';

  Serial.printf("TOPIC: %s COMMAND: %s\n", topic, command);

  if(strcmp(topic, "signrgb") == 0)
    sign_slack_command(command);
  else if (strcmp(topic, "projector") == 0)
    projector_slack_command(command);
  else
    notify_error(String("unknown topic ") + topic + ", payload " + command);
}


#if 0
void update_firmware() {
  ESPhttpUpdate("hostname", 80, "pathname");
}
#endif

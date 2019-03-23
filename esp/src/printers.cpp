// unlike other modules, printers just polls the printers periodically and publishes an update on MQTT

#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#endif

#ifdef ESP32
#include <HTTPClient.h>
#endif

#include <OctoPrintAPI.h>

#include "printers.h"
#include "notify.h"

#include "config.h"

static WiFiClient tcp;

// static IPAddress ip1(OCTOPRINT_IP);
static IPAddress ip1(10, 0, 1, 104);

static OctoprintApi octoprint1(tcp, ip1, OCTOPRINT_PORT, OCTOPRINT_API_KEY);

void printers_update() {
#if 0
  octoprint1._debug = true;

  if(octoprint1.getPrintJob()) {
    Serial.println(octoprint1.printJob.printerState);
  } else {
    Serial.println("octoprint API failed");
    Serial.println(octoprint1.httpStatusCode);
    Serial.println(octoprint1.httpErrorBody);
  }

  if(octoprint1.getOctoprintVersion()) {
    Serial.println(octoprint1.octoprintVer.octoprintApi);
    Serial.println(octoprint1.octoprintVer.octoprintServer);
  } else {
    Serial.println("Octoprint version failed");
    Serial.println(octoprint1.httpStatusCode);
    Serial.println(octoprint1.httpErrorBody);
  }

  if(octoprint1.getPrinterStatistics()) {
    Serial.println(octoprint1.printerStats.printerState); 
    Serial.println(octoprint1.printerStats.printerBedTempActual);
    Serial.println(octoprint1.printerStats.printerTool0TempActual);
  } else {
    Serial.println("Octoprint stats failed");
    Serial.println(octoprint1.httpStatusCode);
    Serial.println(octoprint1.httpErrorBody);
  }
#endif
}

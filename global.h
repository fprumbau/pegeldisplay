#ifndef GLOBAL_H
#define GLOBAL_H

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Update.h>
#include <time.h>

#include "MyWifi.h"
#include "OTA.h"

extern bool debug;

extern MyWifi myWifi;
extern WiFiUDP udp;
extern NTPClient timeClient;

extern AsyncWebServer server;   
extern OTA updater; //Over-the-air updater

#endif

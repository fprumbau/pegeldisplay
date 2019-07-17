#include "global.h"

#include "MyWifi.h"
#include "OTA.h"

bool debug = false;

MyWifi myWifi;
AsyncWebServer server(80);

WiFiUDP udp;
NTPClient timeClient(udp);
OTA updater;

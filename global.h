#ifndef GLOBAL_H
#define GLOBAL_H

#include <GxEPD.h>
//#include <GxGDEW0154Z17/GxGDEW0154Z17.h>  // 1.54" b/w/r 152x152
#include <GxGDEP015OC1/GxGDEP015OC1.h>      // 1.54" b/w 200x200

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

#include "html.h"
#include "fonts.h"

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Update.h>
#include <time.h>

#include "MyWifi.h"
#include "OTA.h"
#include "SMA.h"
#include "display.h"

extern bool debug;

extern MyWifi myWifi;
extern WiFiUDP udp;
extern NTPClient timeClient;

extern GxIO_Class io;
extern GxEPD_Class display;

extern Screen screen;

extern AsyncWebServer server;   
extern OTA updater; //Over-the-air updater

extern const int smaMeasurementsBeforSwitchoff;
extern SMA sma;  //read SMA energy meter broadcast messages 
extern WiFiUDP udp;
extern long lastUdpRead; //letztes, erfolgreiches lesen eines UDP-Paketes (WiFi isAlive)
extern long lastUdpReset; //verhindert Stackoverflow, senden von zu vielen Websocketpaketen 0.9.9.24
extern long lastPingUdp;
extern float bezug;
extern float lieferung;
extern float netto;
extern long lastUpdate;
extern bool doScreenUpdate;
extern int pegel;
extern int temperature;
extern String timeStamp;
extern long lastReceivedPegelMillis;
#endif

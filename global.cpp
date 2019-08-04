#include "global.h"

bool debug = false;

MyWifi myWifi;
AsyncWebServer server(80);

WiFiUDP udp;
NTPClient timeClient(udp);
OTA updater;

GxIO_Class io(SPI, /*CS=5*/ 15, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

Screen screen;

const int smaMeasurementsBeforSwitchoff = 10;
SMA sma;
long lastUdpRead = -1;
long lastUdpReset = -1;
long lastPingUdp = -1;
float bezug=0.0;
float lieferung=0.0;
float netto=0.0;
long lastUpdate=0;
bool doScreenUpdate=false;
int pegel=0;
int temperature=0;
String timeStamp;
long lastReceivedPegelMillis = -1;

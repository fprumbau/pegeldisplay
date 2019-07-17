
// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

#include "html.h"
#include "global.h"

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

#define uS_TO_mS_FACTOR 1000 // Umwandlungsfaktor Mikrosekunden zu Sekunden
#define TIME_TO_SLEEP_MILLIS 60000 //Eine

#include "fonts.h"


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

GxIO_Class io(SPI, /*CS=5*/ 15, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

//Temperatursensoren
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 25

//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//float Celcius=0;

//Eine so definierte Variable ueberlebt den Deep-Sleep
//RTC_DATA_ATTR int bootCount = 0;
int pegel=0;
int temperature=0;
String timeStamp;

void setup() {
  
  Serial.begin(115200); 

  //pinMode(ONE_WIRE_BUS,INPUT_PULLUP);
  //sensors.begin();

  myWifi._ssid="Perry";
  myWifi._password="5785340536886787";

  // etabliere Wifi Verbindung
  myWifi.connect();
  
  Serial.println();
  Serial.println("setup");
  
  delay(100);
  display.init(115200);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "Pegeldisplay, rufe /update zur Aktualisierung oder /restart zum Neustart!");
  });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "Restarte ESP in 3s...");
    delay(3000);
    ESP.restart();
  });

  server.begin();

  // initialize other the air updates
  updater.init("pegeldisplay");
}

void loop() {

  if(!updater.stopForOTA) {
    //sensors.requestTemperatures();
    //delay(1000); //750ms...1000ms
    //Celcius = sensors.getTempCByIndex(0);
    //Serial.print(" C  ");
    //Serial.println(Celcius);
  
    readData();
    yield();
  
    if(pegel>0) { //Wenn keine Daten gelesen werden konnten, sollte auch kein Screenrefresh erfolgen
      showScreen();
      yield();
    } else {
      Serial.print("Pegel ist 0, kein Screenupdate: ");
      Serial.println(pegel);
    }
    //bootCount++;
    
    delay(TIME_TO_SLEEP_MILLIS); //ist immer online, verbraucht bei 60000ms in 10Minuten 17mAH, d.h. in 24h sind das 2,4AH (==Batterie leer), ca. 300mW
    
    //oder ca.31mW während des Schlafens, d.h. in 10Minuten 5mAH, dh. in 24h sind das 720mAh, da hält der Akku 2 1/2 Tage
    //oder mit 2Minuten Schlafenszeit:  
    
    //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_MILLIS * uS_TO_mS_FACTOR);
    //esp_deep_sleep_start();
  }

  //Restart erforderlich, wird durch updater-Objekt nach Upload einer neuen Firmware geregelt
  if(updater.restartRequired) {
    delay(2000);
    ESP.restart();
  }
 
}

void readData() {

   Serial.println(F("readData()"));
   
   // Send HTTP request
   HTTPClient http;
   http.begin(F("http://pegel/data"));
   http.addHeader(F("User-Agent"), F("Mozilla/5.0 (Linux; Android 9.0.0; VS985 4G Build/LRX21Y; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/58.0.3029.83 Mobile Safari/537.36"));
   http.addHeader(F("Content-Type"), F("application/json"));
   
   int httpCode = http.GET();
   if (httpCode > 0) { //Check for the returning code
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
        
        Serial.println(F("Deserialize to doc"));
        DynamicJsonDocument doc(512);
        deserializeJson(doc, payload); //TODO from website???

        if(debug) {
          Serial.println(F("SerializeJsonPretty to Serial"));
          serializeJsonPretty(doc, Serial);
        }
        
        pegel = doc["p"];
        temperature = doc["t"];
        
    } else {
        Serial.println(httpCode);
        Serial.println(F("Error on HTTP request"));
    }
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  int hours = timeClient.getHours() + 2; //UTC Korrektur
  int mins = timeClient.getMinutes();
  int secs = timeClient.getSeconds();
  timeStamp=String(hours);
  timeStamp+=':';
  if(mins<10) timeStamp+='0';
  timeStamp+=mins;
  timeStamp+=':';
  if(secs<10) secs+='0';
  timeStamp+=secs;
    
   yield();
   http.end();
}

void showScreen() {
  
  display.fillScreen(GxEPD_WHITE);
  
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(0, 65);
  display.setFont(&Lato_Bold_80);
  display.print(String(pegel));
  display.print("cm");
  
  display.setCursor(0, 136);
  display.setFont(&Lato_Bold_80);
  display.print(String(temperature));
  display.print("C");
  
  display.setFont(&FreeSans18pt7b);
  display.setCursor(0, 168);
  display.print(timeStamp);
  display.print(" Uhr");

  display.setFont(&FreeSans12pt7b);
  display.setCursor(0, 196);
  String footer = F("Pegel&Temp v.");
  footer += VERSION;
  display.print(footer);
  display.update();
}

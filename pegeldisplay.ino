#include "global.h"

TaskHandle_t Task0;

#define uS_TO_mS_FACTOR 1000 // Umwandlungsfaktor Mikrosekunden zu Sekunden

//Temperatursensoren
//#include <OneWire.h>
//#include <DallasTemperature.h>
//#define ONE_WIRE_BUS 25

//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//float Celcius=0;

//Eine so definierte Variable ueberlebt den Deep-Sleep
//RTC_DATA_ATTR int bootCount = 0;

void setup() {
  
  Serial.begin(115200); 

  //pinMode(ONE_WIRE_BUS,INPUT_PULLUP);
  //sensors.begin();

  myWifi._ssid="Perry";
  myWifi._password="5785340536886787";

  // etabliere Wifi Verbindung
  myWifi.connect();
  
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

  //neuer Taskcode Multicore
  xTaskCreatePinnedToCore(loop0, "Task0", 5000, NULL, 0, &Task0, 0);
}

/**********************************************************************/
/*                                                                    */
/*                 Loop0 (Core0)                                      */
/*                                                                    */
/**********************************************************************/
void loop0(void * parameter) {
  //Der normale Arduino loop laeuft automatisch in einer Schleife; die Schleife DARF nicht leer sein!!!
  for(;;) {
      doScreenUpdate = sma.read();  //energymeter lesen, wenn upd-Paket vorhanden, dann auswerten und beide Charger steuern
      vTaskDelay(10);   //if not: Task watchdog got triggered. The following tasks did not feed the watchdog in time
    //xSemaphoreGive(semaphore);
  }
}

void loop() {

  if(!updater.stopForOTA) {
    //sensors.requestTemperatures();
    //delay(1000); //750ms...1000ms
    //Celcius = sensors.getTempCByIndex(0);
    //Serial.print(" C  ");
    //Serial.println(Celcius);

    doScreenUpdate = false;

    long now = millis();
    if(now - lastUpdate > 5000) {
      
        //Serial.println(F("Trying next read..."));
        //Serial.print(F("Free heap: "));
        //Serial.println(ESP.getFreeHeap()); 
        
        lastUpdate = now;
        readData();
        yield();
        if(pegel>0) { //Wenn keine Daten gelesen werden konnten, sollte auch kein Screenrefresh erfolgen
          lastUpdate = now;
          doScreenUpdate = true;
          yield();
        } else {
          Serial.print(F("Pegel ist 0, kein Screenupdate: "));
          Serial.println(pegel);
        }        
    }

    if(doScreenUpdate) {
      screen.show();
    }

    //bootCount++;
    
    //delay(TIME_TO_SLEEP_MILLIS); //ist immer online, verbraucht bei 60000ms in 10Minuten 17mAH, d.h. in 24h sind das 2,4AH (==Batterie leer), ca. 300mW
    
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
   
   // Send HTTP request
   HTTPClient http;
   http.begin(F("http://pegel/data"));
   http.addHeader(F("User-Agent"), F("Mozilla/5.0 (Linux; Android 9.0.0; VS985 4G Build/LRX21Y; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/58.0.3029.83 Mobile Safari/537.36"));
   http.addHeader(F("Content-Type"), F("application/json"));

   yield();
   int httpCode = http.GET();
   if (httpCode > 0) { //Check for the returning code
        String payload = http.getString();
        //Serial.println(httpCode);
        //Serial.println(payload);
       
        //Serial.println(F("Deserialize to doc"));
        DynamicJsonDocument doc(512);
        deserializeJson(doc, payload); //TODO from website???

        yield();
        if(debug) {
          Serial.println(F("SerializeJsonPretty to Serial"));
          serializeJsonPretty(doc, Serial);
        }
        
        pegel = doc["p"];
        temperature = doc["t"];

        lastReceivedPegelMillis = millis();
        
    } else {
        Serial.println(httpCode);
        Serial.println(F("Error on HTTP request"));
    }
    yield();
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
    timeStamp+=F(" Uhr");
      
    yield();
    http.end();
}

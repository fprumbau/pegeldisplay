#include "display.h"

#include "global.h"

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

void Screen::show() {
  
  //display.update(); 
  if(updateCount ==-1 || updateCount>1000) {
    updateCount=0;
    Serial.println(F("Full screen update"));
    display.fillScreen(GxEPD_WHITE);
    display.update(); 
  } else {
    updateCount++;
    display.fillScreen(GxEPD_WHITE);
    
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(0, 65);
    display.setFont(&Lato_Bold_80);
    display.print(String(pegel));
    display.print(F("cm"));
    
    display.setCursor(0, 136);
    display.setFont(&Lato_Bold_80);
    display.print(String(temperature));
    display.print(F("C"));
    
    display.setFont(&FreeSans12pt7b);
    display.setCursor(0, 160);
    display.print(timeStamp);
  
    //Vor wie vielen Sekunden konnte der letzte Pegelwert gelesen werdne
    //long millisSince = millis() - lastReceivedPegelMillis;
    //int seconds = millisSince / 1000;
    //display.print(seconds);
    //display.print(F(" Sekunden"));
  
    display.setFont(&FreeSans18pt7b);
    display.setCursor(0, 196);
    String footer = String((char*)0);
    footer+="In: ";
    if(netto>0) {
      footer+="+";
    }
    footer+=String(netto,0);
    footer+=F(" W");
    //footer += VERSION;
    display.print(footer);    
    display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, true);
  }
}

#include "global.h"

void SMA::init() {

  // Multicast
  IPAddress ipMulti(239,12,255,254);
  unsigned int portMulti = 9522;      
  
  udp.beginMulticast(ipMulti, portMulti);
  Serial.println(F("\nUDP init fertig!\n"));

  wlb[8]='\0';
  wll[8]='\0';
  initialized = true;
}

void SMA::reset() {
  udp.stop();
  IPAddress ipMulti(239,12,255,254);
  unsigned int portMulti = 9522;      
  
  udp.beginMulticast(ipMulti, portMulti);
  Serial.println(F("UDP reinitialized"));
}

void SMA::ping() {
   Serial.println(F("Ping udp!"));
   udp.beginPacket(udp.remoteIP(), udp.remotePort());
   uint8_t buffer[8] = "pingupd";
   udp.write(buffer,7);
   udp.endPacket();
}

bool SMA::read() {

  if(!initialized) {
    return false;
  }

  
  /*
  //da der UDP-Empfang alle 5Minuten zusammenbricht, jetzt ein Ping jede Minute
  long now = millis();
  if(now-lastPingUdp > 600000) {
    ping();
    lastPingUdp = now;
  }*/
  
  int packetSize = udp.parsePacket();
  if(packetSize) {

    lastUdpRead = millis(); //Dient als WiFi isAlive
 
    // read the packet into buf and get the senders IP addr and port number
    udp.read(buf,packetSize);
    yield();

    for (std::size_t i = 0; i != packetSize; ++i) {
        hex[2*i  ] = lookup[ buf[i] >> 4  ];
        hex[2*i+1] = lookup[ buf[i] & 0xF ];
    }

    for(int v=0; v<8; v++) {
        //Summe Wirkleistung Bezug, 64...71
        wlb[v]=hex[v+64];
        //Summe Wirkleistung Lieferung, 104...111          
        wll[v]=hex[v+104];
    }

    bezug = strtol(wlb, NULL, 16)/10.0; //in Watt
    lieferung = strtol(wll, NULL, 16)/10.0; //in Watt
    netto = lieferung - bezug;

    if(debug) {
      Serial.print(F("Netto: "));
      Serial.println(netto);
    }

    yield();
    return true;
    
  } else {
      //Wird 2 Minuten kein udp Paket des Energymeters gelesen, dann initialisiere WiFi-Reconnect
      long now = millis();
      long lastUdp = now - lastUdpRead;
      //wurde lastUdp millis kein Paket mehr empfangen UND liegt der letzte Resetversuch mind. 10s zurück
      if(lastUdp > 15000 && (now - lastUdpReset) > 10000) {         
          yield();
          reset();
          lastUdpReset = now;
          String msg((char *)0);
          msg.reserve(42);
          msg += F("Last WiFi UPD-Packet read ");
          msg += lastUdp;
          msg += F("ms ago");
          Serial.println(msg);
      }
      return false;
  }
}

/* 
  C64 Emulator, works with esp8266 using webbrowser as display
  see
  http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932
  based upon
  http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065
  
  Remarks speed:
  max speed is 5 frames per second, this amounts to 5 kB (kilobytes) per second
  Reduced the number of RAM bytes send from esp8266 to zero and
  als reduced the processing of this info in browser to zero bytes,
  but this has no effect on framerate (still 5 fps)
  slow speed possibly due to protocolo XMLHTTP!
  Serial debug works at 115200 baudrate, so 14kB per second and would make 14 fps possible!

  ### Android phones are incompatible due missing keyboard...
  
  ### Code is now prepared for
  - Arduino IDE 1.8.10
  - esp8266 board tools version 2.4.2 (important!) 
  - LwIP 1.4 Higher bandwidth
  - CPU frequency 160MHz
  
  This sketch will create free WiFi-AP named Commodore 64 - tap on it and enjoy
*/


#include "esp8266-c64.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "webpages.h"

//const char* ssid = "DeltaFlyer";
const char* ssid = "Sonixwave";
const char* password = "";

ESP8266WebServer server(80);

uint8_t *RAM;//[RAM_SIZE];
uint8_t *HIGHVIC;//[16];//color of background/border/sprites are situated here

void setupMemory() {
  RAM = (uint8_t*)malloc(RAM_SIZE);
  HIGHVIC = (uint8_t*)malloc(16);
}

void serout(uint8_t val) {
		Serial.write(val);
}

void setup () {
  delay(1000);
  Serial.begin (115200);
  Serial.println("Setup Start");
  setupMemory();
}

extern void irq6502();

extern CIATimer CIA_A_Timer;
extern CIATimer CIA_B_Timer;
extern unsigned char CIA_A_CRA;

#define STEPS 10
extern void write6502(uint16_t addr, uint8_t value);
extern uint8_t read6502(uint16_t addr);

void loop () {
  init_fase();  
  server.handleClient();
  //APloop();
  exec6502(EXECUTES_PER_RUN);  
   
 if(read6502(0xDC0E)&0x1) {
    CIA_A_Timer.b16-=STEPS;
      {
        char buff[0xff];
        sprintf(buff,"%x %x%x",CIA_A_Timer.b16, CIA_A_Timer.b[0], CIA_A_Timer.b[1]);
        Serial.println(buff);
      }

    if((long)(CIA_A_Timer.b16-STEPS)<=0) {
       Serial.println("CIA A Expired");
       write6502(0xDC0D,1);
       write6502(0xDC0E,0);
       irq6502();
    }
 }
  //

}

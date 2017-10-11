/*
  ===========================================
       Copyright (c) 2017 Luca Nervetti
              github.com/lunix80
  ===========================================

Connections: 

ESP01       IR Led
Vcc     ->    (+)
Gpio0   ->    (-)
  
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/*****************************************************************
      WiFi Settings:
        IP address for ESP01 device
        Gateway and Subnet        
*/

#include "credentials.h"


IPAddress       ip(192,168,0,36);  
IPAddress       gateway(192,168,0,1);
IPAddress       subnet(255,255,255,0);


/*****************************************************************
      MqTT Settings:
        Broker IP and Port 
        Client ID
        Topic for public status
        Topic to subscribe
     
*/
const char mqtt[]       =   "192.168.0.165";
const int  mqtt_port    =   1883;


const char client_id[]  =  "ESP01-192-168-0-36";
const char topic[]      =  "home/sky";
const char topicsub[]   =  "home/sky/command";



WiFiClient espClient;
PubSubClient client(espClient);


/*****************************************************************
      XMP1 Protocol Settings        
      http://www.hifi-remote.com/wiki/index.php?title=XMP
      https://github.com/torvalds/linux/blob/master/drivers/media/rc/ir-xmp-decoder.c
*/

#define XMP_UNIT            136     /* us */
#define XMP_LEADER          210     /* us */
#define XMP_NIBBLE_PREFIX   760     /* us */
#define XMP_HALFFRAME_SPACE 13800   /* us */
#define XMP_TRAILER_SPACE   80400   /* us should be 80ms but not all dureation supliers can go that high */


#define IRpin   0           // gpio0 --> (-) IR Led
#define KHZ     36          // 36 Khz protocol

/*****************************************************************
      Sky Remote Settings       
*/
#define OEM      0x44
#define SUBADDR  0x20
#define DEVICE   0x31
// #define OBC1        
// #define OBC2     0

const uint8_t halfPeriodicTime = 13; // 500 / KHZ - 1; // -1 to compensate digitalWrite command time

void setup() {
   pinMode(IRpin, OUTPUT);
   digitalWrite(IRpin, HIGH);   // we set pin HIGH because:   IR led (+) on VCC and IR led (-) on IRpin

   WiFi_Connect();
   client.setServer(mqtt, mqtt_port);
   client.setCallback(mqtt_callback);   
}

void loop() {
  if (!client.connected()) {  
    MqTT_Connect(); 
  }
  client.loop();
}


void WiFi_Connect(){
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    WiFi.config(ip, gateway, subnet);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      delay(5000);
      ESP.restart();  
    }
}


void MqTT_Connect(){
  while (!client.connected()) {
    
       if (WiFi.status() != WL_CONNECTED){
           WiFi_Connect();
       }
       
       if (client.connect(client_id)) {  
          client.publish(topic, "ready");
          client.subscribe(topicsub);
        } else {
          delay(5000);    // Wait 5 seconds before retrying
      }
    }
}


/*****************************************************************
      
*/
void mqtt_callback(char* topic, byte* payload, unsigned int length) {

  payload[length] = '\0';
  int i = strtol((char*)payload, 0 , 16); // Transform Hex String to Int
  sendCommand(i);
  
}

/*****************************************************************
      
*/
void sendCommand(unsigned int COMMAND){

  
  unsigned int F1[8] = { 
          SUBADDR >> 4,         // SUBADDRESS
          0,                    // CHECKSUM 
          SUBADDR & 0x0F,       // SUBADDRESS
          15,                   // TOGGLE
          OEM >> 4,             // OEM
          OEM & 0x0F,           // OEM
          DEVICE >> 4,          // DEVICE 
          DEVICE & 0x0F,        // DEVICE 
  };
  
  unsigned int F2[8] = { 
          SUBADDR >> 4,         // SUBADRESS
          0,                    // CHECKSUM
          0,                    // TOGGLE
          SUBADDR & 0x0F,       // SUBADDRESS
          COMMAND >> 4 ,        // OBC1
          COMMAND & 0x0F,       // OBC1
          0,                    // OBC2
          0                     // OBC2
              
  };

  
  F1[1] = checksum(F1);     // CHECKSUM
  
  sendRaw(F1, sizeof(F1) / sizeof(F1[0])); 
  
  mark(XMP_LEADER);
  space(XMP_HALFFRAME_SPACE);

  F2[1] = checksum(F2);     // CHECKSUM
  
  sendRaw(F2, sizeof(F2) / sizeof(F2[0]));
   
  mark(XMP_LEADER);
  space(XMP_TRAILER_SPACE);

  sendRaw(F1, sizeof(F1) / sizeof(F1[0])); 

  mark(XMP_LEADER);
  space(XMP_HALFFRAME_SPACE);
  
  F2[2] = 8;                // SET TOGGLE
  F2[1] = checksum(F2);     // CHECKSUM

  sendRaw(F2, sizeof(F2) / sizeof(F2[0]));

  mark(XMP_LEADER);
  space(0);


}

/*****************************************************************
      
*/

uint8_t checksum(unsigned int F[]){
  
  return ~(0x0F + F[0] + F[2] + F[3] + F[4] + F[5] + F[6] + F[7]) & 0x0F;

}


/*****************************************************************
  
*/

void sendRaw(unsigned int buf[], unsigned int len){ 
  
  for (int i = 0;  i < len;  i++) {
  
    mark(XMP_LEADER);
    space(XMP_NIBBLE_PREFIX + (buf[i] * XMP_UNIT));  
    
  }
  
}



/*****************************************************************
    
*/
void mark(unsigned int time) {
   long beginning = micros();
   while(micros() - beginning < time){
   digitalWrite(IRpin, LOW);
   delayMicroseconds(halfPeriodicTime);
   digitalWrite(IRpin, HIGH);
   delayMicroseconds(halfPeriodicTime); //38 kHz -> T = 26.31 microsec (periodic time), half of it is 13
  }
}


/*****************************************************************
      
*/
void space(unsigned long  time) {
  digitalWrite(IRpin, HIGH); // REVERSE
  if (time > 0) custom_delay_usec(time);
}

/*****************************************************************
  
*/

void custom_delay_usec(unsigned long uSecs) {
  if (uSecs > 4) {
    unsigned long start = micros();
    unsigned long endMicros = start + uSecs - 4;
    if (endMicros < start) { // Check if overflow
      while ( micros() > start ) {}
    }
    while ( micros() < endMicros ) {}
  }
}


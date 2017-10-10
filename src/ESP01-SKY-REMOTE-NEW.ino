#include <ESP8266WiFi.h>
#include <PubSubClient.h>


/*****************************************************************
      WiFi Settings         
*/

const char ssid[]     =   "wiFi_Home";
const char pass[]     =   "02032010giulio";

IPAddress       ip(192,168,0,36);  
IPAddress       gateway(192,168,0,1);
IPAddress       subnet(255,255,255,0);


/*****************************************************************
      MqTT Settings        
*/
const char mqtt[]     =   "192.168.0.165";
const int  mqtt_port  =   1883;

const char client_id[]  =  "ESP01-192-168-0-36";
const char topic[]    =   "home/sky";
const char topicsub[] =   "home/sky/command";



WiFiClient espClient;
PubSubClient client(espClient);


#define XMP_UNIT            136 /* us */
#define XMP_LEADER          210 /* us */
#define XMP_NIBBLE_PREFIX   760 /* us */
#define XMP_HALFFRAME_SPACE 13800 /* us */
#define XMP_TRAILER_SPACE   80400 /* us should be 80ms but not all dureation supliers can go that high */

#define IRpin   0  
#define KHZ     33

#define OEM      0x41;
#define SUBADDR  0x20;
#define DEVICE   0x31;
// #define OBC1    
// #define OBC2     0;


uint8_t halfPeriodicTime = 500 / KHZ - 1; // -1 to compensate digitalWrite command time

uint8_t codes[36] = { // SKY REMOTE COMMAND CODES
0x00,
0x01,
0x02,
0x03,
0x04,
0x05,
0x06,
0x07,
0x08,
0x09,
0x0D, // P+       a
0x0E, // P-       b
0x0F, // POWER    c
0x55, // INFO     d
0x21, // UP       e
0x22, // DOWN     f
0x23, // LEFT     g 
0x24, // RIGHT    h
0x25, // OK       i
0x26, // ESC      j
0x30, // REW      k
0x31, // PAUSE    l
0x32, // FWD      m
0x33, // PLAY     n
0x34, // REC      o
0x35, // STOP     p
0x40, // RED      q
0x41, // GREEN    r
0x42, // YELLOW   s
0x43, // BLUE     t
0x56, // P-txt    u

0x20, // MENU     v   
0x50, // GUIDA    w
0x52, // PRIMA    x
0x53, // SKY      y
0x57  // MY       z
};

char ref[] = "0123456789abcdefghijklmnopqrstuvwxyz";
 

void setup() {
   pinMode(IRpin, OUTPUT);
   digitalWrite(IRpin, HIGH);   // we set pin HIGH because:   IR led (+) on VCC and IR led (-) on IRpin

   WiFi_Connect();
   client.setServer(mqtt, mqtt_port);
   client.setCallback(mqtt_in);

   
}

void loop() {
  if (!client.connected()) {  MqTT_Connect(); }
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
          // Wait 5 seconds before retrying
          delay(5000);
      }
    }
}




void mqtt_in(char* topic, byte* payload, unsigned int length) {

 /*
payload[length] = '\0';
String s = String((char*)payload);
int i= s.toInt();
*/

  int k = findPos(ref, sizeof(ref), (char)payload[0]);
  if (k >= 0){
    sendKey(codes[k]);
   //  client.publish(topic, String(k));
  }
  
}


void sendKey(unsigned int COMMAND){


                        //  S  C  S      O  O  D  D
  // unsigned int frame1[8] = { 2, 3, 0, 15, 4, 4, 3, 1 };
                           //  S  C  T   S  F  F  F  F
  // unsigned int frame2[8] = { 2, 0, 0,  0, 0, 0, 0, 0};



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

  // CHECKSUM
  F1[1] = checksum(F1);
  
  sendRaw(F1, sizeof(F1) / sizeof(F1[0]), 36);  
  
  mark(XMP_LEADER);
  space(XMP_HALFFRAME_SPACE);

  F2[1] = checksum(F2);
  
  sendRaw(frame2, sizeof(frame2) / sizeof(frame2[0]), 36);

  mark(XMP_LEADER);
  space(XMP_HALFFRAME_SPACE);

  sendRaw(frame1, sizeof(frame1) / sizeof(frame1[0]), 36); 

  mark(MARK);
  space(12900);
  
  F2[2] = 8;
  F2[1] = checksum(F2);

  sendRaw(frame2, sizeof(frame2) / sizeof(frame2[0]), 36); 

  mark(MARK);
  space(0);


}


uint8_t checksum(unsigned int F){
  
      return ~(0x0F + F[0] + F[2] + F[3] + F[4] + F[5] + F[6] + F[7]) & 0x0F;

}

int findPos(const char* f, size_t f_size, char s){

  int ret = -1;
  int i = 0;
  bool found = false;
  
  while (i < f_size && !found){

    found = (f[i++] == s);
    
  }

  if (found){
    ret = i - 1;
  }

  return ret;
}

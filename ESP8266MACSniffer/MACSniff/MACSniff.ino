// by Ray Burnette 20161013 compiled on Linux 16.3 using Arduino 1.6.12
//Hacked by Kosme 20170520 compiled on Ubuntu 14.04 using Arduino 1.6.11

// sending MAC data through MQTT by Ferri 20190921 Ubuntu 19.04 & Arduino 1.8.9

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "./functions.h"

#define disable 0
#define enable  1
unsigned int channel = 1;

unsigned long lastupload;
unsigned long timer;

//const char* ssid     = "";         // The SSID (name) of the Wi-Fi network you want to connect to
//const char* password = "";
const char* ssid     = "";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "";

const char* mqttServer = "";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(57600);
  Serial.println(F("MAC Sniffer and MQTT client logger by FerriTheMaker https://github.com/ferrithemaker/Jumble"));
  Serial.println(F("Based on ESP8266 enhanced sniffer by Kosme https://github.com/kosme"));
  enablesniffer();
  lastupload = millis();
}

void loop() {
  timer = millis();
  if (sniffing == false || timer-lastupload > 900000) { // set a time limit to send the data to MQTT server (15 min)
    sendMQTTdata();
    lastupload = millis(); 
    sniffing = true;
  }
  if (sniffing == true) {
    channel = 1;
    wifi_set_channel(channel);
    while (true) {
      nothing_new++;                          // Array is not finite, check bounds and adjust if required
      if (nothing_new > 100) {
        nothing_new = 0;
        channel++;
        if (channel == 15) break;             // Only scan channels 1 to 14
          wifi_set_channel(channel);
        }
      delay(1);  // critical processing timeslice for NONOS SDK! No delay(0) yield()
    }
  }
}


void enablesniffer() {
  wifi_set_opmode(STATION_MODE);            // Promiscuous works only with station mode
  wifi_set_channel(channel);
  wifi_promiscuous_enable(disable);
  wifi_set_promiscuous_rx_cb(promisc_cb);   // Set up promiscuous callback
  wifi_promiscuous_enable(enable);
}

void sendMQTTdata() {
  wifi_promiscuous_enable(disable);
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); 
  Serial.println(" ...");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); 
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
  for (int i=0;i<MAXlist;i++) {
          char strMAC[11]= {lastMACs[i][0],lastMACs[i][1],lastMACs[i][2],lastMACs[i][3],lastMACs[i][4],lastMACs[i][5],lastMACs[i][6],lastMACs[i][7],lastMACs[i][8],lastMACs[i][9],'\0'};
          if (strMAC!="0000000000") { // if MAC is all 0's skip
            client.publish("esp/sniffer", strMAC);
            delay(200);
          }
          for (int i2=0;i2<10;i2++) lastMACs[i][i2]=0x00; // clean the array (fill with 0's)
          MACindex = 0;
  }
  
  // reenable sniffing
  enablesniffer();
}

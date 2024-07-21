#include <SPI.h>
#include <LoRa.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Lora settings
#define ss 15
#define rst 16
#define dio0 4
volatile bool doRead = false;      // Flag set by callback to perform read process in main loop
volatile int incomingPacketSize;
String outgoing;              // outgoing message
byte msgCount = 0;                 // count of outgoing messages
byte localAddress = 0xBB;          // address of this device
byte destination = 0xFF;           // destination to send to
long lastSendTime = 0;             // last send time
int interval = 2000;               // interval between sends
String msg;

//Wifi settings
char ssid[] = "******";
char password[] = "********";
char mqtt_server[] = "broker.emqx.io";

//MQTT settings
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;

void setup() {
  Serial.begin(115200);
  delay(3000);

  //Start LORA
  Serial.println("LoRa Receiver / Relay");
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(100);
    while (1);
  }
  //LoRa.setSpreadingFactor(10);             // Ranges from 6-12, default 7 see API docs
  //LoRa.setSignalBandwidth(62.5E3);         // Supported values are 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3.
  //LoRa.setCodingRate4(8);
   
  LoRa.onReceive(onReceive);               // register the receive callback
  LoRa.receive();                          // put the radio into receive mode
  Serial.println("LoRa init succeeded.");
  
  // Connect to WiFi
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT
  client.setServer(mqtt_server, 1883);    // Connect to a MQTT
   
}

void loop()                              //main loop
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Ensure the MQTT client is processing any incoming messages and maintaining the connection
   
}
 
void onReceive(int packetSize) {
  // set flag for new message
  Serial.println("Got message, setting flag");
 // doRead = true;
  incomingPacketSize = packetSize;

  readMessage();
}

void readMessage() {
  // received a packet
  //if (packetSize == 0) return;          // if there's no packet, return

  String incoming = "";                  // payload of packet
  while (LoRa.available()) {             // can't use readString() in callback, so
    incoming += (char)LoRa.read();       // add bytes one by one
  }
  msg = incoming;
  publishMQTT();
  // print details to serial monitor
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
}

void publishMQTT() {
   
  int str_len = msg.length() + 1;
  char value[str_len];
  msg.toCharArray(value, str_len);

  client.publish("/teguhtopik/p/temp2", value,true); 
  Serial.println("terkirim: " + msg +" ===");

}

void reconnect() {                    // Loop until we're reconnected to MQTT
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Clientmmm", "", "")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      //delay(5000);
    }
  }
}

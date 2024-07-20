#include <SPI.h>
#include <LoRa.h>

int counter = 0;
#define ss 10
#define rst 9
#define dio0 2



void setup() {
  Serial.begin(9600);
  LoRa.setPins(ss, rst, dio0);
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  Serial.print("Sending packetuno: ");
  Serial.println(counter);
  

  // send packet
  LoRa.beginPacket();
  LoRa.print("Pgj");//Lokasi
  LoRa.print(" ");
  LoRa.print("s1");//data sensor Kelembapan
  LoRa.print(" ");
  LoRa.print("s2"); // Data sensor Ultrasonik
  LoRa.print(" ");
  LoRa.print(" s3"); // Data sensor gas
  LoRa.print(" ");
  LoRa.print(" s4");// data sensor suhu
  LoRa.print(" ");
  LoRa.print(" s5");// data sensor berat
  LoRa.print(" ");
   
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}

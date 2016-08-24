// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa
// Tested with Teensy 3.2 on WiNoLoRa node by Drien's aka Adrien van den Bossche <vandenbo@univ-tlse2.fr>

#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95(SS,9);
int led = 13;

void setup() 
{
  pinMode(led, OUTPUT);     
  Serial.begin(9600);
  SPI.setSCK(14);
  for (int i=0; i<3; i++) {
    // Nice blink at boot
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
  }
  if (!rf95.init()) {
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    Serial.println("init failed");  
    digitalWrite(led, HIGH);
    while(1) {
      digitalWrite(led, HIGH);
      delay(50);
      digitalWrite(led, LOW);
      delay(50);
    }
  }

  // Set PHY parameters
  // Modulation and bandwidth
  // rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128); // Radio default
  // rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096); // slow and reliable?
  rf95.setModemConfig(RH_RF95::Bw31_25Cr48Sf512); // Drien's favorite

  // Frame preamble lenght
  rf95.setPreambleLength(8); // Default is 8

  // Frequency
  // rf95.setFrequency(434.0);
  rf95.setFrequency(868.0);

  // TX power
  // rf95.setTxPower(13);
  rf95.setTxPower(10);
}


void loop()
{
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  uint8_t data[] = "Hello World!";
  digitalWrite(led, HIGH);
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
  digitalWrite(led, LOW);
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.print(rf95.lastRssi(), DEC);
      Serial.println("dBm");
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
  delay(20000); // Every 20 seconds
}



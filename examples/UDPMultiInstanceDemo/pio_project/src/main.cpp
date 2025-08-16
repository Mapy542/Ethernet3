/*
 * UDP Multi-Instance Demo
 * 
 * This example demonstrates how to use UDP with specific Ethernet3Class instances
 * instead of the global Ethernet instance.
 * 
 * Circuit:
 * * Ethernet shield attached to pins 10, 11, 12, 13
 */

#include <SPI.h>
#include <Ethernet3.h>
#include <EthernetUdp2.h>

// MAC address and IP configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// Create specific Ethernet3Class instance
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // W5500 on CS pin 10

// Create UDP instances - one uses global Ethernet, one uses specific instance
EthernetUDP udpGlobal;        // Uses global Ethernet instance (backward compatibility)
EthernetUDP udpSpecific(&eth1); // Uses specific eth1 instance

unsigned int localPort1 = 8888;  // port for global UDP
unsigned int localPort2 = 8889;  // port for specific UDP

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
char replyBuffer[] = "Hello from multi-instance UDP!";

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  
  Serial.println("UDP Multi-Instance Demo");
  Serial.println("=======================");
  
  // Initialize the specific Ethernet instance
  Serial.print("Initializing specific Ethernet instance... ");
  eth1.begin(mac, ip);
  Serial.println("Done!");
  
  // Initialize global Ethernet instance for comparison
  Serial.print("Initializing global Ethernet instance... ");
  Ethernet.begin(mac, ip);
  Serial.println("Done!");
  
  // Start UDP on both instances
  Serial.print("Starting UDP on global instance (port ");
  Serial.print(localPort1);
  Serial.print(")... ");
  if (udpGlobal.begin(localPort1)) {
    Serial.println("Success!");
  } else {
    Serial.println("Failed!");
  }
  
  Serial.print("Starting UDP on specific instance (port ");
  Serial.print(localPort2);
  Serial.print(")... ");
  if (udpSpecific.begin(localPort2)) {
    Serial.println("Success!");
  } else {
    Serial.println("Failed!");
  }
  
  Serial.println("\nDemo ready! Send UDP packets to:");
  Serial.print("- Port ");
  Serial.print(localPort1);
  Serial.println(" for global instance");
  Serial.print("- Port ");
  Serial.print(localPort2);
  Serial.println(" for specific instance");
}

void loop() {
  // Check for packets on global UDP instance
  int packetSize1 = udpGlobal.parsePacket();
  if (packetSize1) {
    Serial.print("[GLOBAL] Received packet size ");
    Serial.print(packetSize1);
    Serial.print(" from ");
    Serial.print(udpGlobal.remoteIP());
    Serial.print(":");
    Serial.println(udpGlobal.remotePort());
    
    // Read and echo
    udpGlobal.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    udpGlobal.beginPacket(udpGlobal.remoteIP(), udpGlobal.remotePort());
    udpGlobal.write(replyBuffer);
    udpGlobal.endPacket();
  }
  
  // Check for packets on specific UDP instance
  int packetSize2 = udpSpecific.parsePacket();
  if (packetSize2) {
    Serial.print("[SPECIFIC] Received packet size ");
    Serial.print(packetSize2);
    Serial.print(" from ");
    Serial.print(udpSpecific.remoteIP());
    Serial.print(":");
    Serial.println(udpSpecific.remotePort());
    
    // Read and echo
    udpSpecific.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    udpSpecific.beginPacket(udpSpecific.remoteIP(), udpSpecific.remotePort());
    udpSpecific.write(replyBuffer);
    udpSpecific.endPacket();
  }
  
  delay(10);
}
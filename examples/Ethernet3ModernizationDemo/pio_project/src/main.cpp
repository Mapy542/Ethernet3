/*
 Ethernet3 Library Modernization Demo
 
 This example demonstrates the new features added to Ethernet3:
 - linkActive() function for checking physical link status
 - UDP multicast support with automatic group management
 
 Based on the UDPSendReceiveString example but enhanced with multicast capabilities.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 */

#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>

// Enter a MAC address and IP address for your controller below.
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

unsigned int localPort = 8888;      // local port to listen on
IPAddress multicastGroup(239, 255, 0, 1);  // Multicast group to join

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Ethernet.begin(mac, ip);
  
  // Check if the Ethernet hardware is connected.
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  
  // Wait for link to be established
  Serial.print("Waiting for link...");
  while (!Ethernet.linkActive()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Link established!");

  // Check for Ethernet cable connection
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start UDP for multicast
  Serial.print("Starting UDP multicast on group ");
  Serial.print(multicastGroup);
  Serial.print(" port ");
  Serial.println(localPort);
  
  if (Udp.beginMulticast(multicastGroup, localPort)) {
    Serial.println("UDP multicast started successfully");
    
    // Join the multicast group
    if (Udp.joinMulticastGroup(multicastGroup)) {
      Serial.println("Joined multicast group successfully");
    } else {
      Serial.println("Failed to join multicast group");
    }
  } else {
    Serial.println("Failed to start UDP multicast");
  }
}

void loop() {
  // Check link status periodically
  static unsigned long lastLinkCheck = 0;
  if (millis() - lastLinkCheck > 5000) {
    if (Ethernet.linkActive()) {
      Serial.println("Link is active");
    } else {
      Serial.println("Link is down!");
    }
    lastLinkCheck = millis();
  }

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
  
  // Send a multicast message every 10 seconds
  static unsigned long lastMulticastSend = 0;
  if (millis() - lastMulticastSend > 10000) {
    Serial.println("Sending multicast message...");
    
    Udp.beginPacket(multicastGroup, localPort);
    Udp.print("Hello from Ethernet3 with multicast support! Time: ");
    Udp.print(millis());
    Udp.endPacket();
    
    lastMulticastSend = millis();
  }
  
  delay(10);
}
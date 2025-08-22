/*
  MulticastReceiver.ino
  
  This sketch demonstrates how to receive UDP multicast packets using the Ethernet3 library.
  It joins a multicast group and listens for incoming multicast messages.
  
  Multicast allows one sender to transmit data to multiple receivers simultaneously.
  Multicast addresses are in the range 224.0.0.0 to 239.255.255.255.
  
  This example:
  - Joins multicast group 239.255.0.1 on port 12345
  - Receives and displays multicast messages
  - Prints sender information for each received packet
  
  Compatible with MulticastSender.ino example.
  
  Created for Ethernet3 library
  This code is in the public domain.
*/

#include <Ethernet3.h>
#include <EthernetUdp2.h>  // UDP library with multicast support
#include <SPI.h>           // needed for Arduino versions later than 0018

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

// Init the desired ethernet chip
W5500 chip(10);  // 10 is the CS pin for the W5500 chip
// W5100 chip(10);

EthernetClass Ethernet(&chip);

// Multicast configuration
IPAddress multicastIP(239, 255, 0, 1);  // Multicast group address
unsigned int multicastPort = 12345;     // Multicast port

// Buffer for receiving data
char packetBuffer[255];  // buffer to hold incoming packet

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp(&Ethernet, &chip);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Ethernet3 Multicast Receiver Example");
  Serial.println("====================================");
  
  // Start the Ethernet connection
  Ethernet.begin(mac, ip);
  
  // Print local IP address
  Serial.print("Local IP address: ");
  Serial.println(Ethernet.localIP());
  
  // Begin multicast listening
  if (Udp.beginMulticast(multicastIP, multicastPort)) {
    Serial.print("Successfully joined multicast group: ");
    Serial.print(multicastIP);
    Serial.print(" on port ");
    Serial.println(multicastPort);
  } else {
    Serial.println("Failed to join multicast group!");
    Serial.println("Check your network configuration and multicast IP address.");
    return;
  }
  
  // Additional multicast group join (optional)
  if (Udp.joinMulticastGroup(multicastIP)) {
    Serial.println("Multicast group join confirmed");
  }
  
  Serial.println("Listening for multicast messages...");
  Serial.println("Send messages using the MulticastSender example or any multicast sender.");
  Serial.println();
}

void loop() {
  // Check if there's data available
  int packetSize = Udp.parsePacket();
  
  if (packetSize) {
    Serial.print("Received multicast packet of size ");
    Serial.print(packetSize);
    Serial.print(" bytes");
    
    // Print sender information
    Serial.print(" from ");
    IPAddress remote = Udp.remoteIP();
    Serial.print(remote);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());
    
    // Read the packet into packetBuffer
    int len = Udp.read(packetBuffer, sizeof(packetBuffer) - 1);
    if (len > 0) {
      packetBuffer[len] = '\0';  // Null terminate the string
      Serial.print("Contents: ");
      Serial.println(packetBuffer);
    }
    
    Serial.println("---");
  }
  
  delay(10);  // Small delay to prevent overwhelming the serial output
}

/*
  Testing Notes:
  =============
  
  1. Upload this sketch to one or more Arduino boards
  2. Use the MulticastSender.ino example on another board or device
  3. All receivers will receive the same multicast messages
  
  Network Requirements:
  - All devices must be on the same network/subnet
  - Network switches/routers must support multicast forwarding
  - Some networks may require IGMP configuration
  
  Troubleshooting:
  - If no packets are received, check your network configuration
  - Ensure the multicast IP (239.255.0.1) is valid and not filtered
  - Verify that sender and receiver use the same multicast group and port
  - Some networks may block multicast traffic by default
*/
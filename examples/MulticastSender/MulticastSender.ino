/*
  MulticastSender.ino
  
  This sketch demonstrates how to send UDP multicast packets using the Ethernet3 library.
  It periodically sends messages to a multicast group that can be received by multiple devices.
  
  Multicast allows one sender to transmit data to multiple receivers simultaneously.
  Multicast addresses are in the range 224.0.0.0 to 239.255.255.255.
  
  This example:
  - Sends messages to multicast group 239.255.0.1 on port 12345
  - Sends a message every 5 seconds with timestamp and counter
  - Uses regular UDP socket to send to multicast address
  
  Compatible with MulticastReceiver.ino example.
  
  Created for Ethernet3 library
  This code is in the public domain.
*/

#include <Ethernet3.h>
#include <EthernetUdp2.h>  // UDP library with multicast support
#include <SPI.h>           // needed for Arduino versions later than 0018

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEC};  // Different MAC from receiver
IPAddress ip(192, 168, 1, 178);                      // Different IP from receiver

// Init the desired ethernet chip
W5500 chip(10);  // 10 is the CS pin for the W5500 chip
// W5100 chip(10);

EthernetClass Ethernet(&chip);

// Multicast configuration
IPAddress multicastIP(239, 255, 0, 1);  // Multicast group address
unsigned int multicastPort = 12345;     // Multicast port

// Message configuration
unsigned long messageCount = 0;
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 5000;  // Send every 5 seconds

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp(&Ethernet, &chip);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Ethernet3 Multicast Sender Example");
  Serial.println("==================================");
  
  // Start the Ethernet connection
  Ethernet.begin(mac, ip);
  
  // Print local IP address
  Serial.print("Local IP address: ");
  Serial.println(Ethernet.localIP());
  
  // Initialize UDP for sending
  if (Udp.begin(8888)) {  // Use a local port for sending
    Serial.println("UDP initialized for multicast sending");
  } else {
    Serial.println("Failed to initialize UDP!");
    return;
  }
  
  // Verify multicast IP is valid
  if (Udp.isMulticastGroup(multicastIP)) {
    Serial.print("Target multicast group: ");
    Serial.print(multicastIP);
    Serial.print(" on port ");
    Serial.println(multicastPort);
  } else {
    Serial.println("Invalid multicast IP address!");
    return;
  }
  
  Serial.println("Ready to send multicast messages...");
  Serial.print("Sending interval: ");
  Serial.print(sendInterval / 1000);
  Serial.println(" seconds");
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Check if it's time to send a message
  if (currentTime - lastSendTime >= sendInterval) {
    sendMulticastMessage();
    lastSendTime = currentTime;
  }
  
  delay(100);  // Small delay to prevent busy waiting
}

void sendMulticastMessage() {
  messageCount++;
  
  // Create message with timestamp and counter
  char message[100];
  unsigned long uptime = millis() / 1000;  // Uptime in seconds
  
  snprintf(message, sizeof(message), 
          "Hello from Multicast Sender! Message #%lu, Uptime: %lu seconds", 
          messageCount, uptime);
  
  // Send the multicast packet
  if (Udp.beginPacket(multicastIP, multicastPort)) {
    Udp.write((uint8_t*)message, strlen(message));
    
    if (Udp.endPacket()) {
      Serial.print("Sent message #");
      Serial.print(messageCount);
      Serial.print(" to ");
      Serial.print(multicastIP);
      Serial.print(":");
      Serial.println(multicastPort);
      Serial.print("Content: ");
      Serial.println(message);
    } else {
      Serial.println("Failed to send multicast packet!");
    }
  } else {
    Serial.println("Failed to begin multicast packet!");
  }
  
  Serial.println("---");
}

/*
  Testing Notes:
  =============
  
  1. Upload this sketch to an Arduino board
  2. Use the MulticastReceiver.ino example on one or more other boards/devices
  3. All receivers on the same network will receive the multicast messages
  
  Network Requirements:
  - All devices must be on the same network/subnet
  - Network switches/routers must support multicast forwarding
  - Some networks may require IGMP configuration
  
  Advanced Usage:
  - Multiple senders can send to the same multicast group
  - Different message types can use different multicast addresses
  - Consider using different ports for different types of data
  
  Troubleshooting:
  - If receivers don't get packets, check network multicast support
  - Ensure sender and receiver use the same multicast group and port
  - Verify that the multicast IP (239.255.0.1) is not filtered by network
  - Some firewalls may block multicast traffic
  
  Multicast Address Ranges:
  - 224.0.0.0 to 224.0.0.255: Reserved for local network control
  - 224.0.1.0 to 238.255.255.255: Internetwork control and data
  - 239.0.0.0 to 239.255.255.255: Administrative scoped (local use)
*/
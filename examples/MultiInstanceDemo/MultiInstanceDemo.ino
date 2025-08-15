/*
 Multi-Instance Ethernet3 Demo
 
 This example demonstrates the new multi-instance capabilities of Ethernet3:
 - Creating multiple Ethernet instances with different chips
 - Using W5100 and W5500 simultaneously (if available)
 - Independent network configurations per instance
 - Link status monitoring per instance
 
 Hardware Requirements:
 - Two Ethernet shields/modules (W5100 and W5500) or one for testing
 - Different CS pins for each shield
 
 Circuit:
 * First Ethernet shield (W5500) on CS pin 10
 * Second Ethernet shield (W5100) on CS pin 9  
 * Both shields share SPI bus (pins 11, 12, 13)
 */

#include <SPI.h>
#include <Ethernet3.h>  // Use new Ethernet3 header

// MAC addresses for each instance (must be unique)
byte mac1[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte mac2[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };

// IP addresses for static configuration
IPAddress ip1(192, 168, 1, 177);
IPAddress ip2(192, 168, 1, 178);

// Create multiple Ethernet instances
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // W5500 on CS pin 10
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);   // W5100 on CS pin 9

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("Ethernet3 Multi-Instance Demo");
  Serial.println("=============================");
  
  // Initialize first instance (W5500)
  Serial.print("Initializing W5500 on CS pin 10... ");
  if (eth1.begin(mac1, ip1)) {
    Serial.println("Success!");
    Serial.print("W5500 Hardware Status: ");
    Serial.println(eth1.hardwareStatus());
    Serial.print("W5500 IP Address: ");
    Serial.println(eth1.localIP());
    Serial.print("W5500 Max Sockets: ");
    Serial.println(eth1.getMaxSockets());
  } else {
    Serial.println("Failed!");
  }
  
  // Initialize second instance (W5100)
  Serial.print("Initializing W5100 on CS pin 9... ");
  if (eth2.begin(mac2, ip2)) {
    Serial.println("Success!");
    Serial.print("W5100 Hardware Status: ");
    Serial.println(eth2.hardwareStatus());
    Serial.print("W5100 IP Address: ");
    Serial.println(eth2.localIP());
    Serial.print("W5100 Max Sockets: ");
    Serial.println(eth2.getMaxSockets());
  } else {
    Serial.println("Failed!");
  }
  
  Serial.println("\nDemo will monitor link status of both instances...");
  Serial.println("Note: If you only have one shield, one instance will show as failed.");
}

void loop() {
  static unsigned long lastCheck = 0;
  
  // Check link status every 5 seconds
  if (millis() - lastCheck > 5000) {
    Serial.println("\n=== Link Status Check ===");
    
    // Check W5500 instance
    Serial.print("W5500 (CS 10): ");
    if (eth1.linkActive()) {
      Serial.print("Link UP - IP: ");
      Serial.print(eth1.localIP());
      Serial.print(" Gateway: ");
      Serial.println(eth1.gatewayIP());
    } else {
      Serial.println("Link DOWN or not connected");
    }
    
    // Check W5100 instance  
    Serial.print("W5100 (CS 9):  ");
    if (eth2.linkActive()) {
      Serial.print("Link UP - IP: ");
      Serial.print(eth2.localIP());
      Serial.print(" Gateway: ");
      Serial.println(eth2.gatewayIP());
    } else {
      Serial.println("Link DOWN or not connected");
    }
    
    // Show socket states for each instance
    Serial.println("\n--- Socket States ---");
    Serial.print("W5500 Sockets: ");
    for (uint8_t i = 0; i < eth1.getMaxSockets(); i++) {
      Serial.print(eth1.getSocketState(i));
      Serial.print(" ");
    }
    Serial.println();
    
    Serial.print("W5100 Sockets: ");
    for (uint8_t i = 0; i < eth2.getMaxSockets(); i++) {
      Serial.print(eth2.getSocketState(i));
      Serial.print(" ");
    }
    Serial.println();
    
    lastCheck = millis();
  }
  
  // You could add client/server code here that uses different instances
  // For example:
  // EthernetClient client1(&eth1);  // Client using W5500
  // EthernetClient client2(&eth2);  // Client using W5100
  
  delay(100);
}

/* 
 Expected Output:
 
 Ethernet3 Multi-Instance Demo
 =============================
 Initializing W5500 on CS pin 10... Success!
 W5500 Hardware Status: 3
 W5500 IP Address: 192.168.1.177
 W5500 Max Sockets: 8
 Initializing W5100 on CS pin 9... Success!
 W5100 Hardware Status: 1  
 W5100 IP Address: 192.168.1.178
 W5100 Max Sockets: 4
 
 Demo will monitor link status of both instances...
 
 === Link Status Check ===
 W5500 (CS 10): Link UP - IP: 192.168.1.177 Gateway: 192.168.1.1
 W5100 (CS 9):  Link UP - IP: 192.168.1.178 Gateway: 192.168.1.1
 
 --- Socket States ---
 W5500 Sockets: 0 0 0 0 0 0 0 0 
 W5100 Sockets: 0 0 0 0 
 */
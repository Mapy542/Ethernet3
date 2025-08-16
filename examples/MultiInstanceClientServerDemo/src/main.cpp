/*
 * MultiInstanceClientServerDemo.ino
 * 
 * This example demonstrates Phase 3 & 4 enhancements:
 * - Multi-instance EthernetClient and EthernetServer
 * - Platform-specific optimizations (ESP32, STM32, Arduino)
 * - Comprehensive multi-instance architecture
 * 
 * Hardware:
 * - Two Ethernet chips: W5500 on pin 10, W5100 on pin 9
 * - Or use single chip with multiple logical instances
 */

#include <SPI.h>
#include <Ethernet3.h>
#include <EthernetUdp.h>

// Platform-specific platform selection
#ifdef ESP32
#include "hal/ESP32Platform.h"
ESP32Platform platform;
#elif defined(STM32F1) || defined(STM32F4)
#include "hal/STM32Platform.h"
STM32Platform platform;
#else
#include "hal/ArduinoPlatform.h"
ArduinoPlatform platform;
#endif

// Network configuration
byte mac1[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte mac2[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEC };
IPAddress ip1(192, 168, 1, 177);
IPAddress ip2(192, 168, 1, 178);

// Create multiple Ethernet instances with unified platform
Ethernet3Class eth1(CHIP_TYPE_W5500, 10, &platform);  // W5500 on CS pin 10  
Ethernet3Class eth2(CHIP_TYPE_W5100, 9, &platform);   // W5100 on CS pin 9

// Create multi-instance clients and servers
EthernetClient client1(&eth1);      // Client using eth1
EthernetClient client2(&eth2);      // Client using eth2
EthernetServer server1(80, &eth1);  // Web server on eth1, port 80
EthernetServer server2(81, &eth2);  // Web server on eth2, port 81

// Multi-instance UDP and DHCP
EthernetUDP udp1(&eth1);
EthernetUDP udp2(&eth2);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  
  Serial.println("Multi-Instance Client/Server Demo");
  Serial.println("=================================");
  
  // Display platform information
  Serial.print("Platform: ");
  Serial.println(hal.getPlatformName());
  Serial.print("Hardware Acceleration: ");
  Serial.println(hal.hasHardwareAcceleration() ? "Yes" : "No");
  Serial.print("DMA Support: ");
  Serial.println(hal.supportsDMA() ? "Yes" : "No");
  Serial.print("Real-time Support: ");
  Serial.println(hal.supportsRealTime() ? "Yes" : "No");
  
  // Initialize Ethernet instances
  Serial.println("\nInitializing Ethernet instances...");
  
  Serial.print("eth1 (W5500): ");
  if (eth1.begin(mac1, ip1)) {
    Serial.print("OK - ");
    Serial.print(eth1.localIP());
    Serial.print(" (");
    Serial.print(eth1.getMaxSockets());
    Serial.println(" sockets)");
    
    if (eth1.linkActive()) {
      Serial.println("  Link active!");
    } else {
      Serial.println("  No link detected");
    }
  } else {
    Serial.println("Failed!");
  }
  
  Serial.print("eth2 (W5100): ");
  if (eth2.begin(mac2, ip2)) {
    Serial.print("OK - ");
    Serial.print(eth2.localIP());
    Serial.print(" (");
    Serial.print(eth2.getMaxSockets());
    Serial.println(" sockets)");
    
    if (eth2.linkActive()) {
      Serial.println("  Link active!");
    } else {
      Serial.println("  No link detected");
    }
  } else {
    Serial.println("Failed!");
  }
  
  // Start servers
  Serial.println("\nStarting servers...");
  server1.begin();
  Serial.println("Server1 (eth1) started on port 80");
  server2.begin();
  Serial.println("Server2 (eth2) started on port 81");
  
  // Start UDP services
  Serial.println("\nStarting UDP services...");
  if (udp1.begin(8888)) {
    Serial.println("UDP1 (eth1) started on port 8888");
  }
  if (udp2.begin(8889)) {
    Serial.println("UDP2 (eth2) started on port 8889");
  }
  
  Serial.println("\nDemo ready!");
  Serial.println("Connect to:");
  Serial.print("- Web server 1: http://");
  Serial.print(eth1.localIP());
  Serial.println(":80");
  Serial.print("- Web server 2: http://");
  Serial.print(eth2.localIP());
  Serial.println(":81");
  Serial.print("- UDP service 1: ");
  Serial.print(eth1.localIP());
  Serial.println(":8888");
  Serial.print("- UDP service 2: ");
  Serial.print(eth2.localIP());
  Serial.println(":8889");
}

void loop() {
  // Handle web server requests
  handleWebServer(server1, &eth1, "Server1 (W5500)");
  handleWebServer(server2, &eth2, "Server2 (W5100)");
  
  // Handle UDP packets
  handleUDP(udp1, &eth1, "UDP1 (W5500)");
  handleUDP(udp2, &eth2, "UDP2 (W5100)");
  
  // Demonstrate client connections periodically
  static unsigned long lastClientDemo = 0;
  if (millis() - lastClientDemo > 30000) {  // Every 30 seconds
    demonstrateClientConnections();
    lastClientDemo = millis();
  }
  
  // Platform-specific yielding
  hal.yield();
  delay(10);
}

void handleWebServer(EthernetServer &server, Ethernet3Class* eth, const char* serverName) {
  EthernetClient client = server.available();
  if (client) {
    Serial.print(serverName);
    Serial.println(" - New client connected");
    
    // Read HTTP request
    String request = "";
    while (client.connected() && client.available()) {
      char c = client.read();
      request += c;
      if (request.endsWith("\r\n\r\n")) break;
    }
    
    // Send HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    
    // HTML content
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head><title>Ethernet3 Multi-Instance Demo</title></head>");
    client.println("<body>");
    client.print("<h1>");
    client.print(serverName);
    client.println("</h1>");
    client.print("<p>Platform: ");
    client.print(hal.getPlatformName());
    client.println("</p>");
    client.print("<p>Server IP: ");
    client.print(eth->localIP());
    client.println("</p>");
    client.print("<p>Available Sockets: ");
    client.print(eth->getMaxSockets());
    client.println("</p>");
    client.print("<p>Link Status: ");
    client.print(eth->linkActive() ? "Active" : "Inactive");
    client.println("</p>");
    client.print("<p>Uptime: ");
    client.print(millis() / 1000);
    client.println(" seconds</p>");
    client.println("</body>");
    client.println("</html>");
    
    delay(10);
    client.stop();
    Serial.print(serverName);
    Serial.println(" - Client disconnected");
  }
}

void handleUDP(EthernetUDP &udp, Ethernet3Class* eth, const char* udpName) {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print(udpName);
    Serial.print(" - Received packet size ");
    Serial.print(packetSize);
    Serial.print(" from ");
    Serial.print(udp.remoteIP());
    Serial.print(":");
    Serial.println(udp.remotePort());
    
    // Read and echo packet
    char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    
    // Echo response
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.print("Echo from ");
    udp.print(udpName);
    udp.print(": ");
    udp.write(packetBuffer, packetSize);
    udp.endPacket();
  }
}

void demonstrateClientConnections() {
  Serial.println("\n--- Client Connection Demo ---");
  
  // Try connecting to Google DNS
  IPAddress google(8, 8, 8, 8);
  
  Serial.print("Client1 (eth1) connecting to Google DNS... ");
  if (client1.connect(google, 53)) {
    Serial.println("Connected!");
    client1.stop();
  } else {
    Serial.println("Failed");
  }
  
  Serial.print("Client2 (eth2) connecting to Google DNS... ");
  if (client2.connect(google, 53)) {
    Serial.println("Connected!");
    client2.stop();
  } else {
    Serial.println("Failed");
  }
  
  Serial.println("--- End Client Demo ---\n");
}
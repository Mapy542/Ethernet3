/*
  BackwardCompatibilityDemo - Demonstrates backward compatibility modes in Ethernet3

  This example shows how to use Ethernet3 in different compatibility modes:
  1. Default mode (backward compatible with Ethernet/Ethernet2)
  2. Modern mode (explicit instances, no global singleton)
  
  To switch between modes, comment/uncomment the #define below.
  
  Hardware:
  - Arduino board (Uno, Nano, Mega, etc.)
  - W5500 Ethernet module on CS pin 10
  - Optional: W5100 module on CS pin 9 for multi-instance demo
  
  Created: 2024
  by: Ethernet3 Library
*/

// Uncomment this line to use modern mode (no backward compatibility)
// #define ETHERNET3_NO_BACKWARDS_COMPATIBILITY

#include <Ethernet3.h>

// Network configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

#ifdef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
// Modern mode - explicit instances required
Ethernet3Class eth(CHIP_TYPE_W5500, 10);
EthernetServer server(80, &eth);
EthernetClient client(&eth);
#else
// Backward compatible mode - uses global instances
EthernetServer server(80);
EthernetClient client;
#endif

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial console
  
  Serial.println("Ethernet3 Backward Compatibility Demo");
  
#ifdef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
  Serial.println("Mode: Modern (no backward compatibility)");
  Serial.println("- Explicit Ethernet instance required");
  Serial.println("- No global singletons instantiated");
  Serial.println("- Lower memory usage");
  
  // Initialize explicit instance
  if (eth.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    return;
  }
  
  Serial.print("IP address: ");
  Serial.println(eth.localIP());
  
  // Start server on explicit instance
  server.begin();
  Serial.println("Modern server started on port 80");
  
#else
  Serial.println("Mode: Backward Compatible (default)");
  Serial.println("- Global Ethernet instance available");
  Serial.println("- Compatible with Ethernet/Ethernet2 code");
  Serial.println("- Legacy constructors available");
  
  // Initialize global instance (legacy style)
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    return;
  }
  
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  
  // Start server using global instance
  server.begin();
  Serial.println("Compatible server started on port 80");
  
  // Show that global instance is available
  Serial.print("Hardware status: ");
  Serial.println(Ethernet.hardwareStatus());
  Serial.print("Link status: ");
  Serial.println(Ethernet.linkStatus());
#endif

  Serial.println("");
  Serial.println("Demo features:");
  Serial.println("- Basic HTTP server responding to requests");
  Serial.println("- Link status monitoring");
  Serial.println("- Connection handling");
  Serial.println("");
  Serial.println("Open http://" + 
#ifdef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
                 String(eth.localIP()[0]) + "." + eth.localIP()[1] + "." + 
                 eth.localIP()[2] + "." + eth.localIP()[3]
#else
                 String(Ethernet.localIP()[0]) + "." + Ethernet.localIP()[1] + "." + 
                 Ethernet.localIP()[2] + "." + Ethernet.localIP()[3]
#endif
                 + " in browser");
}

void loop() {
  // Check for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    
    // Read the HTTP request
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          // End of HTTP request, send response
          if (currentLine.length() == 0) {
            // Send HTTP response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            
            // Send HTML page
            client.println("<!DOCTYPE html>");
            client.println("<html><head><title>Ethernet3 Demo</title></head>");
            client.println("<body>");
            client.println("<h1>Ethernet3 Backward Compatibility Demo</h1>");
            
#ifdef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
            client.println("<h2>Modern Mode</h2>");
            client.println("<p>Using explicit Ethernet3Class instance</p>");
            client.print("<p>IP: "); client.print(eth.localIP()); client.println("</p>");
            client.print("<p>Hardware: "); client.print(eth.hardwareStatus()); client.println("</p>");
            client.print("<p>Link Active: "); client.print(eth.linkActive() ? "Yes" : "No"); client.println("</p>");
#else
            client.println("<h2>Backward Compatible Mode</h2>");
            client.println("<p>Using global Ethernet instance</p>");
            client.print("<p>IP: "); client.print(Ethernet.localIP()); client.println("</p>");
            client.print("<p>Hardware: "); client.print(Ethernet.hardwareStatus()); client.println("</p>");
            client.print("<p>Link Active: "); client.print(Ethernet.linkActive() ? "Yes" : "No"); client.println("</p>");
#endif
            
            client.println("<h3>Features Demonstrated:</h3>");
            client.println("<ul>");
            client.println("<li>HTTP server handling</li>");
            client.println("<li>Link status monitoring</li>");
            client.println("<li>Backward compatibility options</li>");
            client.println("</ul>");
            
            client.println("<p>Compile with ETHERNET3_NO_BACKWARDS_COMPATIBILITY defined to see modern mode.</p>");
            client.println("</body></html>");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    
    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
  
  // Monitor link status every 5 seconds
  static unsigned long lastLinkCheck = 0;
  if (millis() - lastLinkCheck > 5000) {
    lastLinkCheck = millis();
    
#ifdef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
    bool linkActive = eth.linkActive();
#else
    bool linkActive = Ethernet.linkActive();
#endif
    
    Serial.print("Link status: ");
    Serial.println(linkActive ? "ACTIVE" : "INACTIVE");
    
    if (!linkActive) {
      Serial.println("Check Ethernet cable connection");
    }
  }
}
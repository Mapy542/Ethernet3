/*
  Backwards Compatibility Test

  This sketch demonstrates the backwards compatibility mode where
  global instances are used and you don't need to pass ethernet and chip
  objects to every class constructor.

  To enable backwards compatibility, define ETHERNET_BACKWARDS_COMPATIBILITY
  before including Ethernet3.h

  Circuit:
  * Ethernet shield attached to pins 10, 11, 12, 13

  created August 2025
  by GitHub Copilot
 */

// Enable backwards compatibility mode
#define ETHERNET_BACKWARDS_COMPATIBILITY

#include <Ethernet3.h>
#include <SPI.h>

// Enter a MAC address for your controller below.
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};

// Initialize classes using the old (simple) way - no need to pass chip or ethernet instances
EthernetClient client;  // No parameters needed!
EthernetServer server(80);  // Only port needed!

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("Testing Ethernet3 Backwards Compatibility Mode");
  
  // start the Ethernet connection using the global Ethernet object:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for (;;);
  }
  
  // start the server:
  server.begin();
  
  // print your local IP address:
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    
    // send a simple HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<h1>Ethernet3 Backwards Compatibility Test</h1>");
    client.println("<p>If you can see this page, backwards compatibility is working!</p>");
    client.println("</html>");
    
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
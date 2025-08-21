/*
 * HTTP Client Example
 * 
 * This example demonstrates how to use the HTTPClient class
 * to make HTTP requests to a web server.
 * 
 * Circuit:
 * * Ethernet shield attached to pins 10, 11, 12, 13
 * 
 * Created: 2024
 * 
 */

#include <Ethernet3.h>
#include <SPI.h>
#include <HTTP.h>

// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 1);

// Init the desired ethernet chip
W5500 chip(10);  // 10 is the CS pin for the W5500 chip

EthernetClass Ethernet(&chip);

// Initialize HTTP client
HTTPClient httpClient(&Ethernet, &chip);

unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.linkActive() == 0) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");
}

void loop() {
  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpGet();
  }

  delay(1000);
}

// this method makes an HTTP GET request:
void httpGet() {
  // close any connection before send a new request.
  httpClient.disconnect();

  Serial.println("\nStarting HTTP GET request...");
  
  // Make a complete GET request with URL
  HTTPResponse response = httpClient.request("GET", "http://httpbin.org/get");
  
  // Print response details
  Serial.print("Status Code: ");
  Serial.println(response.getStatusCode());
  Serial.print("Status Message: ");
  Serial.println(response.getStatusMessage());
  
  Serial.println("Headers:");
  for (int i = 0; i < response.getHeaderCount(); i++) {
    // This is a simple way to print headers - in real implementation
    // you might want to add a method to iterate through headers
    Serial.print("  Header ");
    Serial.print(i);
    Serial.println(" present");
  }
  
  Serial.println("Body:");
  Serial.println(response.getBody());
  
  // note the time that the connection was made:
  lastConnectionTime = millis();
}

// Alternative method for manual connection and request
void httpGetManual() {
  httpClient.disconnect();
  
  Serial.println("\nStarting manual HTTP GET request...");
  
  // Connect to server manually
  if (httpClient.connect("httpbin.org", 80)) {
    Serial.println("Connected to server");
    
    // Make GET request
    HTTPResponse response = httpClient.GET("/get");
    
    Serial.print("Status Code: ");
    Serial.println(response.getStatusCode());
    Serial.println("Body:");
    Serial.println(response.getBody());
    
    httpClient.disconnect();
  } else {
    Serial.println("Connection failed");
  }
  
  lastConnectionTime = millis();
}
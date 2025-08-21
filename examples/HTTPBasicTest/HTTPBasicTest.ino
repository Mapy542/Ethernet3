/*
 * HTTP Basic Test
 * 
 * A simple compilation test for the HTTP library components.
 * This example creates HTTP objects without actually using the network
 * to verify that the classes compile correctly.
 */

#include <Ethernet3.h>
#include <SPI.h>
#include <HTTP.h>

// Basic setup for compilation test
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

W5500 chip(10);
EthernetClass Ethernet(&chip);

void setup() {
  Serial.begin(9600);
  
  // Test HTTPRequest
  HTTPRequest request = HTTPRequest::GET("/test");
  request.addHeader("Host", "example.com");
  String requestStr = request.toString();
  
  // Test HTTPResponse  
  HTTPResponse response = HTTPResponse::OK("Hello World");
  response.setHeader("Content-Type", "text/plain");
  String responseStr = response.toString();
  
  // Test HTTPClient
  HTTPClient client(&Ethernet, &chip);
  client.setUserAgent("TestClient/1.0");
  
  // Test HTTPServer
  HTTPServer server(&Ethernet, &chip, 80);
  server.onGET("/", testHandler);
  
  Serial.println("HTTP library basic test completed successfully!");
}

void loop() {
  // Nothing to do in loop for this test
}

HTTPResponse testHandler(const HTTPRequest& request) {
  return HTTPServer::sendPlain("Test response");
}
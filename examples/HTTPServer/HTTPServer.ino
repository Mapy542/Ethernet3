/*
 * HTTP Server Example
 * 
 * This example demonstrates how to use the HTTPServer class
 * to create a web server with routing support.
 * 
 * Circuit:
 * * Ethernet shield attached to pins 10, 11, 12, 13
 * * Analog inputs attached to pins A0 through A5 (optional)
 * 
 * Created: 2024
 * 
 */

#include <Ethernet3.h>
#include <SPI.h>
#include <HTTP.h>

// Enter a MAC address and IP address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// Init the desired ethernet chip
W5500 chip(10);  // 10 is the CS pin for the W5500 chip

EthernetClass Ethernet(&chip);

// Initialize HTTP server on port 80
HTTPServer server(&Ethernet, &chip, 80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  
  // Set up routes
  server.onGET("/", handleRoot);
  server.onGET("/sensors", handleSensors);
  server.onGET("/api/status", handleAPIStatus);
  server.onPOST("/api/data", handleAPIData);
  server.onNotFound(handleNotFound);
  
  server.begin();
  
  Serial.print("HTTP server is at ");
  Serial.println(Ethernet.localIP());
  Serial.println("Available routes:");
  Serial.println("  GET  /");
  Serial.println("  GET  /sensors");
  Serial.println("  GET  /api/status");
  Serial.println("  POST /api/data");
}

void loop() {
  server.handleClient();
}

// Route handlers
HTTPResponse handleRoot(const HTTPRequest& request) {
  String html = "<!DOCTYPE html><html><head><title>Arduino HTTP Server</title></head>";
  html += "<body><h1>Welcome to Arduino HTTP Server!</h1>";
  html += "<p>This server is powered by the Ethernet3 HTTP library.</p>";
  html += "<ul>";
  html += "<li><a href=\"/sensors\">View Sensor Data</a></li>";
  html += "<li><a href=\"/api/status\">API Status (JSON)</a></li>";
  html += "</ul>";
  html += "</body></html>";
  
  return HTTPServer::sendHTML(html);
}

HTTPResponse handleSensors(const HTTPRequest& request) {
  String html = "<!DOCTYPE html><html><head><title>Sensor Data</title>";
  html += "<meta http-equiv=\"refresh\" content=\"5\"></head>";
  html += "<body><h1>Sensor Readings</h1>";
  html += "<table border=\"1\"><tr><th>Pin</th><th>Value</th></tr>";
  
  // Read analog sensors
  for (int i = 0; i < 6; i++) {
    int sensorValue = analogRead(i);
    html += "<tr><td>A" + String(i) + "</td><td>" + String(sensorValue) + "</td></tr>";
  }
  
  html += "</table>";
  html += "<p>Page refreshes every 5 seconds</p>";
  html += "<a href=\"/\">Back to Home</a>";
  html += "</body></html>";
  
  return HTTPServer::sendHTML(html);
}

HTTPResponse handleAPIStatus(const HTTPRequest& request) {
  String json = "{";
  json += "\"status\":\"ok\",";
  json += "\"uptime\":" + String(millis()) + ",";
  json += "\"free_memory\":0,";
  json += "\"ip\":\"192.168.1.177\"";
  json += "}";
  
  return HTTPServer::sendJSON(json);
}

HTTPResponse handleAPIData(const HTTPRequest& request) {
  // Handle POST request - echo back the received data
  String json = "{";
  json += "\"received\":\"" + request.getBody() + "\",";
  json += "\"method\":\"" + request.getMethod() + "\",";
  json += "\"path\":\"" + request.getPath() + "\",";
  json += "\"timestamp\":" + String(millis());
  json += "}";
  
  return HTTPServer::sendJSON(json);
}

HTTPResponse handleNotFound(const HTTPRequest& request) {
  String html = "<!DOCTYPE html><html><head><title>404 Not Found</title></head>";
  html += "<body><h1>404 - Page Not Found</h1>";
  html += "<p>The requested path <code>" + request.getPath() + "</code> was not found.</p>";
  html += "<p>Method: " + request.getMethod() + "</p>";
  html += "<a href=\"/\">Go to Home</a>";
  html += "</body></html>";
  
  HTTPResponse response = HTTPResponse::NotFound(html);
  response.setHeader("Content-Type", "text/html");
  return response;
}

// Helper function to get approximate uptime in seconds
unsigned long getUptimeSeconds() {
  return millis() / 1000;
}
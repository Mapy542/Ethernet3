/*
  Ethernet3 Comprehensive Feature Demo
  
  This example demonstrates all major features of the Ethernet3 library:
  - Multi-instance architecture with W5100 and W5500
  - UDP multicast group communication
  - Link status monitoring
  - Platform-specific optimizations
  - Independent client/server operations
  
  Hardware Required:
  - Arduino Uno/Mega/ESP32/STM32
  - Two Ethernet modules (W5100 and W5500)
  - Ethernet cables and network connection
  
  Circuit:
  W5500 Module:
  - VCC to 3.3V/5V
  - GND to GND  
  - MOSI to pin 11 (or MOSI)
  - MISO to pin 12 (or MISO)
  - SCK to pin 13 (or SCK)
  - CS to pin 10
  
  W5100 Module:
  - VCC to 3.3V/5V
  - GND to GND
  - MOSI to pin 11 (or MOSI)  
  - MISO to pin 12 (or MISO)
  - SCK to pin 13 (or SCK)
  - CS to pin 9
  
  This example ties into the documentation:
  - Getting Started: Basic setup and initialization
  - Multi-Instance: Using multiple Ethernet interfaces
  - UDP Multicast: Group communication patterns
  - Platform Support: Automatic optimization detection
  
  Created for Ethernet3 library
  by Ethernet3 Contributors
*/

#include <Ethernet3.h>

// ===== NETWORK CONFIGURATION =====

// Create two Ethernet instances with different chips
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // W5500 on CS pin 10
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);   // W5100 on CS pin 9

// MAC addresses (must be unique)
byte mac1[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01};  // W5500 interface
byte mac2[] = {0xCA, 0xFE, 0xBA, 0xBE, 0xFA, 0x02};  // W5100 interface

// IP configurations for different network segments
IPAddress ip1(192, 168, 1, 177);    // W5500 network
IPAddress ip2(10, 0, 0, 177);       // W5100 network

// Multicast groups for different purposes
IPAddress sensorGroup(239, 255, 1, 1);    // Sensor data sharing
IPAddress controlGroup(239, 255, 1, 2);   // Control commands
IPAddress statusGroup(239, 255, 1, 3);    // Status updates

// ===== MULTI-INSTANCE NETWORK OBJECTS =====

// Instance-specific clients and servers
EthernetClient webClient1(&eth1);         // Web client via W5500
EthernetClient webClient2(&eth2);         // Web client via W5100
EthernetServer webServer1(80, &eth1);     // Web server on W5500
EthernetServer webServer2(8080, &eth2);   // Web server on W5100

// UDP instances for multicast communication
EthernetUDP multicastUDP1(&eth1);         // Multicast via W5500  
EthernetUDP multicastUDP2(&eth2);         // Multicast via W5100
EthernetUDP unicastUDP1(&eth1);           // Unicast via W5500
EthernetUDP unicastUDP2(&eth2);           // Unicast via W5100

// ===== APPLICATION STATE =====

unsigned long lastSensorReading = 0;
unsigned long lastStatusUpdate = 0;
unsigned long lastLinkCheck = 0;
unsigned long lastWebRequest = 0;

bool eth1LinkWasActive = false;
bool eth2LinkWasActive = false;

int requestCounter = 0;
float simulatedTemperature = 20.0;

// ===== SETUP FUNCTION =====

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    ; // Wait for serial port to connect (Leonardo/Micro)
  }
  
  Serial.println("\n=== Ethernet3 Comprehensive Feature Demo ===");
  Serial.println("Demonstrating multi-instance, multicast, and platform features");
  
  // Display platform information
  displayPlatformInfo();
  
  // Initialize first Ethernet interface (W5500)
  Serial.println("\nInitializing W5500 interface...");
  if (eth1.begin(mac1, ip1)) {
    Serial.println("✓ W5500 initialized successfully");
    Serial.print("  IP address: ");
    Serial.println(eth1.localIP());
    Serial.print("  Sockets available: ");
    Serial.println("8 (W5500)");
  } else {
    Serial.println("✗ W5500 initialization failed");
  }
  
  // Initialize second Ethernet interface (W5100)
  Serial.println("\nInitializing W5100 interface...");
  if (eth2.begin(mac2, ip2)) {
    Serial.println("✓ W5100 initialized successfully");
    Serial.print("  IP address: ");
    Serial.println(eth2.localIP());
    Serial.print("  Sockets available: ");
    Serial.println("4 (W5100)");
  } else {
    Serial.println("✗ W5100 initialization failed");
  }
  
  // Initialize web servers
  webServer1.begin();
  webServer2.begin();
  Serial.println("\n✓ Web servers started:");
  Serial.print("  Server 1 (W5500): http://");
  Serial.print(eth1.localIP());
  Serial.println(":80");
  Serial.print("  Server 2 (W5100): http://");
  Serial.print(eth2.localIP());
  Serial.println(":8080");
  
  // Initialize multicast communication
  Serial.println("\nInitializing multicast communication...");
  
  // Join sensor data group on both interfaces
  if (multicastUDP1.beginMulticast(sensorGroup, 8001)) {
    Serial.println("✓ W5500 joined sensor multicast group");
  }
  
  if (multicastUDP2.beginMulticast(controlGroup, 8002)) {
    Serial.println("✓ W5100 joined control multicast group");  
  }
  
  // Initialize unicast UDP for status reporting
  unicastUDP1.begin(9001);
  unicastUDP2.begin(9002);
  Serial.println("✓ Unicast UDP initialized");
  
  // Display initial link status
  checkLinkStatus(true);
  
  Serial.println("\n=== Demo Started ===");
  printCommands();
}

// ===== MAIN LOOP =====

void loop() {
  // Check for physical link changes
  if (millis() - lastLinkCheck > 5000) {
    checkLinkStatus(false);
    lastLinkCheck = millis();
  }
  
  // Handle multicast communication
  handleMulticastCommunication();
  
  // Handle web server requests
  handleWebServers();
  
  // Send periodic sensor data via multicast
  if (millis() - lastSensorReading > 15000) {
    sendSensorData();
    lastSensorReading = millis();
  }
  
  // Send periodic status updates
  if (millis() - lastStatusUpdate > 30000) {
    sendStatusUpdate();
    lastStatusUpdate = millis();
  }
  
  // Handle serial commands
  handleSerialCommands();
  
  // Small delay to prevent overwhelming the network
  delay(100);
}

// ===== PLATFORM INFORMATION =====

void displayPlatformInfo() {
  Serial.println("\nPlatform Information:");
  
#ifdef ESP32
  Serial.println("  Platform: ESP32");
  Serial.println("  Features: FreeRTOS, 16MHz SPI, dual-core support");
  Serial.println("  HAL: ESP32HAL with task optimization");
  Serial.println("  SPI Bus: ESP32SPIBus with DMA support");
#elif defined(STM32F4)
  Serial.println("  Platform: STM32F4");
  Serial.println("  Features: DMA transfers, 12MHz SPI, hardware acceleration");
  Serial.println("  HAL: STM32HAL with fast GPIO");
  Serial.println("  SPI Bus: STM32SPIBus with DMA");
#elif defined(STM32F1)
  Serial.println("  Platform: STM32F1");
  Serial.println("  Features: Hardware optimization, fast SPI");
  Serial.println("  HAL: STM32HAL");
  Serial.println("  SPI Bus: STM32SPIBus");
#elif defined(ARDUINO_ARCH_AVR)
  Serial.println("  Platform: Arduino AVR (Uno/Mega/etc.)");
  Serial.println("  Features: Conservative optimizations, full compatibility");
  Serial.println("  HAL: ArduinoHAL with yield support");
  Serial.println("  SPI Bus: ArduinoSPIBus");
#else
  Serial.println("  Platform: Generic Arduino");
  Serial.println("  Features: Standard compatibility mode");
  Serial.println("  HAL: ArduinoHAL");
  Serial.println("  SPI Bus: ArduinoSPIBus");
#endif
}

// ===== LINK STATUS MONITORING =====

void checkLinkStatus(bool forceDisplay) {
  bool eth1Active = eth1.linkActive();
  bool eth2Active = eth2.linkActive();
  
  // Check for link state changes
  if (eth1Active != eth1LinkWasActive || forceDisplay) {
    Serial.print("W5500 Link: ");
    Serial.println(eth1Active ? "UP ✓" : "DOWN ✗");
    eth1LinkWasActive = eth1Active;
  }
  
  if (eth2Active != eth2LinkWasActive || forceDisplay) {
    Serial.print("W5100 Link: ");
    Serial.println(eth2Active ? "UP ✓" : "DOWN ✗");
    eth2LinkWasActive = eth2Active;
  }
}

// ===== MULTICAST COMMUNICATION =====

void handleMulticastCommunication() {
  // Handle sensor data multicast on W5500
  int packetSize1 = multicastUDP1.parsePacket();
  if (packetSize1) {
    Serial.print("Sensor multicast received (");
    Serial.print(packetSize1);
    Serial.print(" bytes) from ");
    Serial.println(multicastUDP1.remoteIP());
    
    // Read and display packet
    String data = "";
    while (multicastUDP1.available()) {
      data += (char)multicastUDP1.read();
    }
    Serial.println("  Data: " + data);
  }
  
  // Handle control multicast on W5100
  int packetSize2 = multicastUDP2.parsePacket();
  if (packetSize2) {
    Serial.print("Control multicast received (");
    Serial.print(packetSize2);
    Serial.print(" bytes) from ");
    Serial.println(multicastUDP2.remoteIP());
    
    String command = "";
    while (multicastUDP2.available()) {
      command += (char)multicastUDP2.read();
    }
    Serial.println("  Command: " + command);
    
    // Process commands
    processControlCommand(command);
  }
}

void sendSensorData() {
  if (!eth1.linkActive()) return;
  
  // Simulate sensor reading
  simulatedTemperature += random(-10, 11) / 10.0;  // Random variation
  if (simulatedTemperature < 15.0) simulatedTemperature = 15.0;
  if (simulatedTemperature > 35.0) simulatedTemperature = 35.0;
  
  // Create JSON sensor data
  String sensorData = "{";
  sensorData += "\"device\":\"Demo_Sensor_001\",";
  sensorData += "\"type\":\"temperature\",";
  sensorData += "\"value\":" + String(simulatedTemperature, 1) + ",";
  sensorData += "\"unit\":\"C\",";
  sensorData += "\"timestamp\":" + String(millis()) + ",";
  sensorData += "\"interface\":\"W5500\",";
  sensorData += "\"ip\":\"" + eth1.localIP().toString() + "\"";
  sensorData += "}";
  
  // Send via multicast
  multicastUDP1.beginPacket(sensorGroup, 8001);
  multicastUDP1.print(sensorData);
  multicastUDP1.endPacket();
  
  Serial.println("Sensor data transmitted: " + String(simulatedTemperature, 1) + "°C");
}

void sendStatusUpdate() {
  if (!eth2.linkActive()) return;
  
  // Create status message
  String status = "STATUS:Demo_Device:";
  status += String(millis());
  status += ":ONLINE:W5100:";
  status += eth2.localIP().toString();
  
  // Send to status group via unicast (to demonstrate mixed communication)
  IPAddress statusCollector(239, 255, 1, 3);
  unicastUDP2.beginPacket(statusCollector, 8003);
  unicastUDP2.print(status);
  unicastUDP2.endPacket();
  
  Serial.println("Status update sent via W5100");
}

void processControlCommand(const String& command) {
  if (command == "RESET_TEMP") {
    simulatedTemperature = 20.0;
    Serial.println("  → Temperature sensor reset");
  } else if (command == "STATUS_REQUEST") {
    // Send immediate status response
    sendStatusUpdate();
    Serial.println("  → Status response sent");
  } else if (command.startsWith("SET_TEMP:")) {
    float newTemp = command.substring(9).toFloat();
    if (newTemp >= 0 && newTemp <= 50) {
      simulatedTemperature = newTemp;
      Serial.println("  → Temperature set to " + String(newTemp, 1) + "°C");
    }
  } else {
    Serial.println("  → Unknown command: " + command);
  }
}

// ===== WEB SERVER HANDLING =====

void handleWebServers() {
  // Handle W5500 web server (port 80)
  EthernetClient client1 = webServer1.available();
  if (client1) {
    handleWebClient(client1, "W5500", 1);
  }
  
  // Handle W5100 web server (port 8080)
  EthernetClient client2 = webServer2.available();
  if (client2) {
    handleWebClient(client2, "W5100", 2);
  }
}

void handleWebClient(EthernetClient& client, const char* interface, int serverNum) {
  Serial.print("Web client connected to ");
  Serial.print(interface);
  Serial.println(" server");
  
  // Read HTTP request
  String request = "";
  bool requestComplete = false;
  
  while (client.connected() && !requestComplete) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      request += line + "\n";
      
      if (line == "\r") {
        requestComplete = true;
      }
    }
  }
  
  // Extract requested path
  String path = "/";
  int pathStart = request.indexOf(' ') + 1;
  int pathEnd = request.indexOf(' ', pathStart);
  if (pathStart > 0 && pathEnd > pathStart) {
    path = request.substring(pathStart, pathEnd);
  }
  
  // Generate response based on path
  if (path == "/") {
    sendWebHomePage(client, interface, serverNum);
  } else if (path == "/status") {
    sendWebStatusPage(client, interface, serverNum);
  } else if (path == "/sensor") {
    sendWebSensorPage(client, interface, serverNum);
  } else {
    sendWeb404Page(client, interface);
  }
  
  delay(1);
  client.stop();
  Serial.println("Web client disconnected");
}

void sendWebHomePage(EthernetClient& client, const char* interface, int serverNum) {
  // Send HTTP headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  // Send HTML page
  client.println("<!DOCTYPE html>");
  client.println("<html><head>");
  client.println("<title>Ethernet3 Demo - " + String(interface) + "</title>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; margin: 40px; }");
  client.println("h1 { color: #333; }");
  client.println(".info { background: #f0f0f0; padding: 15px; margin: 10px 0; }");
  client.println(".status { background: #e8f5e8; padding: 10px; margin: 10px 0; }");
  client.println("a { color: #0066cc; text-decoration: none; margin-right: 20px; }");
  client.println("</style>");
  client.println("</head><body>");
  
  client.println("<h1>Ethernet3 Multi-Instance Demo</h1>");
  client.println("<div class='info'>");
  client.println("<h2>Server Information</h2>");
  client.println("<p><strong>Interface:</strong> " + String(interface) + "</p>");
  client.println("<p><strong>Server:</strong> #" + String(serverNum) + "</p>");
  
  if (serverNum == 1) {
    client.println("<p><strong>IP Address:</strong> " + eth1.localIP().toString() + "</p>");
    client.println("<p><strong>Chip Type:</strong> W5500 (8 sockets)</p>");
  } else {
    client.println("<p><strong>IP Address:</strong> " + eth2.localIP().toString() + "</p>");
    client.println("<p><strong>Chip Type:</strong> W5100 (4 sockets)</p>");
  }
  
  client.println("<p><strong>Uptime:</strong> " + String(millis() / 1000) + " seconds</p>");
  client.println("</div>");
  
  client.println("<div class='status'>");
  client.println("<h2>Link Status</h2>");
  client.print("<p>W5500 Link: ");
  client.println(eth1.linkActive() ? "<span style='color: green;'>UP ✓</span></p>" : "<span style='color: red;'>DOWN ✗</span></p>");
  client.print("<p>W5100 Link: ");
  client.println(eth2.linkActive() ? "<span style='color: green;'>UP ✓</span></p>" : "<span style='color: red;'>DOWN ✗</span></p>");
  client.println("</div>");
  
  client.println("<h2>Navigation</h2>");
  client.println("<p>");
  client.println("<a href='/'>Home</a>");
  client.println("<a href='/status'>System Status</a>");
  client.println("<a href='/sensor'>Sensor Data</a>");
  client.println("</p>");
  
  client.println("<h2>Ethernet3 Features Demonstrated</h2>");
  client.println("<ul>");
  client.println("<li>Multi-instance architecture (W5100 + W5500)</li>");
  client.println("<li>Independent web servers on different chips</li>");
  client.println("<li>UDP multicast communication</li>");
  client.println("<li>Real-time link status monitoring</li>");
  client.println("<li>Platform-specific optimizations</li>");
  client.println("</ul>");
  
  client.println("</body></html>");
}

void sendWebStatusPage(EthernetClient& client, const char* interface, int serverNum) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();
  
  client.println("{");
  client.println("  \"interface\": \"" + String(interface) + "\",");
  client.println("  \"server\": " + String(serverNum) + ",");
  
  if (serverNum == 1) {
    client.println("  \"ip\": \"" + eth1.localIP().toString() + "\",");
    client.println("  \"link_active\": " + String(eth1.linkActive() ? "true" : "false") + ",");
  } else {
    client.println("  \"ip\": \"" + eth2.localIP().toString() + "\",");
    client.println("  \"link_active\": " + String(eth2.linkActive() ? "true" : "false") + ",");
  }
  
  client.println("  \"uptime_ms\": " + String(millis()) + ",");
  client.println("  \"temperature\": " + String(simulatedTemperature, 1) + ",");
  client.println("  \"requests_served\": " + String(++requestCounter));
  client.println("}");
}

void sendWebSensorPage(EthernetClient& client, const char* interface, int serverNum) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  client.println("<!DOCTYPE html>");
  client.println("<html><head>");
  client.println("<title>Sensor Data - " + String(interface) + "</title>");
  client.println("<meta http-equiv='refresh' content='5'>");  // Auto-refresh every 5 seconds
  client.println("</head><body>");
  client.println("<h1>Live Sensor Data</h1>");
  client.println("<p><strong>Interface:</strong> " + String(interface) + "</p>");
  client.println("<p><strong>Current Temperature:</strong> " + String(simulatedTemperature, 1) + "°C</p>");
  client.println("<p><strong>Last Reading:</strong> " + String((millis() - lastSensorReading) / 1000) + " seconds ago</p>");
  client.println("<p><em>Page auto-refreshes every 5 seconds</em></p>");
  client.println("<p><a href='/'>← Back to Home</a></p>");
  client.println("</body></html>");
}

void sendWeb404Page(EthernetClient& client, const char* interface) {
  client.println("HTTP/1.1 404 Not Found");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  client.println("<html><body>");
  client.println("<h1>404 - Page Not Found</h1>");
  client.println("<p>Interface: " + String(interface) + "</p>");
  client.println("<p><a href='/'>Go to Home Page</a></p>");
  client.println("</body></html>");
}

// ===== SERIAL COMMAND INTERFACE =====

void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command == "help") {
      printCommands();
    } else if (command == "status") {
      printSystemStatus();
    } else if (command == "links") {
      checkLinkStatus(true);
    } else if (command == "sensor") {
      sendSensorData();
    } else if (command == "multicast") {
      testMulticast();
    } else if (command.startsWith("temp ")) {
      float newTemp = command.substring(5).toFloat();
      if (newTemp >= 0 && newTemp <= 50) {
        simulatedTemperature = newTemp;
        Serial.println("Temperature set to " + String(newTemp, 1) + "°C");
      } else {
        Serial.println("Invalid temperature (0-50°C)");
      }
    } else if (command != "") {
      Serial.println("Unknown command: " + command);
      Serial.println("Type 'help' for available commands");
    }
  }
}

void printCommands() {
  Serial.println("\n=== Available Commands ===");
  Serial.println("help       - Show this help message");
  Serial.println("status     - Show system status");
  Serial.println("links      - Check link status");
  Serial.println("sensor     - Send sensor data");
  Serial.println("multicast  - Test multicast transmission");
  Serial.println("temp <val> - Set temperature (0-50°C)");
  Serial.println("");
}

void printSystemStatus() {
  Serial.println("\n=== System Status ===");
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
  Serial.println("");
  
  Serial.println("W5500 Interface:");
  Serial.println("  IP: " + eth1.localIP().toString());
  Serial.println("  Link: " + String(eth1.linkActive() ? "UP" : "DOWN"));
  Serial.println("  Sockets: 8 available");
  Serial.println("");
  
  Serial.println("W5100 Interface:");
  Serial.println("  IP: " + eth2.localIP().toString());
  Serial.println("  Link: " + String(eth2.linkActive() ? "UP" : "DOWN"));
  Serial.println("  Sockets: 4 available");
  Serial.println("");
  
  Serial.println("Application Data:");
  Serial.println("  Temperature: " + String(simulatedTemperature, 1) + "°C");
  Serial.println("  Web requests served: " + String(requestCounter));
  Serial.println("  Last sensor reading: " + String((millis() - lastSensorReading) / 1000) + "s ago");
  Serial.println("");
  
  Serial.println("Multicast Groups:");
  Serial.println("  Sensor group: " + sensorGroup.toString() + ":8001 (W5500)");
  Serial.println("  Control group: " + controlGroup.toString() + ":8002 (W5100)");
  Serial.println("");
}

void testMulticast() {
  Serial.println("\nTesting multicast transmission...");
  
  // Send test message to sensor group
  if (eth1.linkActive()) {
    multicastUDP1.beginPacket(sensorGroup, 8001);
    multicastUDP1.print("TEST:Multicast message from W5500 at " + String(millis()));
    multicastUDP1.endPacket();
    Serial.println("Test message sent to sensor group via W5500");
  }
  
  // Send test command to control group
  if (eth2.linkActive()) {
    multicastUDP2.beginPacket(controlGroup, 8002);
    multicastUDP2.print("STATUS_REQUEST");
    multicastUDP2.endPacket();
    Serial.println("Test command sent to control group via W5100");
  }
  
  Serial.println("Check serial output for received messages...");
}
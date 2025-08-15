# Getting Started with Ethernet3

This guide will help you install and start using the Ethernet3 library with your Arduino, ESP32, or STM32 project.

## Installation

### PlatformIO (Recommended)

1. **Install via Library Manager**:
   ```bash
   pio lib install "Ethernet3"
   ```

2. **Add to platformio.ini**:
   ```ini
   [env:myboard]
   platform = espressif32  ; or atmelavr, ststm32, etc.
   board = esp32dev
   framework = arduino
   lib_deps = 
       Ethernet3
   ```

3. **Include in your code**:
   ```cpp
   #include <Ethernet3.h>
   ```

### Arduino IDE

1. **Download Release**:
   - Go to [GitHub Releases](https://github.com/Mapy542/Ethernet3/releases)
   - Download the latest `Ethernet3-x.x.x.zip`

2. **Install Library**:
   - Arduino IDE → Sketch → Include Library → Add .ZIP Library
   - Select the downloaded zip file
   - Restart Arduino IDE

3. **Verify Installation**:
   - File → Examples → Ethernet3 → WebClient
   - Compile to verify installation

### Manual Installation

1. **Clone Repository**:
   ```bash
   git clone https://github.com/Mapy542/Ethernet3.git
   ```

2. **Copy to Libraries**:
   ```bash
   cp -r Ethernet3 ~/Arduino/libraries/
   ```

## Hardware Setup

### Supported Chips

#### W5500 (Recommended)
- **Features**: 8 sockets, hardware TCP/IP stack, SPI interface
- **Performance**: Up to 16MHz SPI, full duplex communication
- **Use Cases**: High-performance applications, multiple connections

#### W5100 
- **Features**: 4 sockets, hardware TCP/IP stack, SPI interface  
- **Performance**: Up to 12MHz SPI, proven stability
- **Use Cases**: Legacy projects, simple applications

### Wiring

#### Arduino Uno/Nano + W5500

```
W5500 Module    Arduino Uno
VCC        →    3.3V or 5V
GND        →    GND
MOSI       →    Pin 11 (MOSI)
MISO       →    Pin 12 (MISO)  
SCK        →    Pin 13 (SCK)
CS         →    Pin 10 (or custom)
RST        →    Pin 9 (optional)
```

#### ESP32 + W5500

```
W5500 Module    ESP32
VCC        →    3.3V
GND        →    GND
MOSI       →    GPIO 23 (MOSI)
MISO       →    GPIO 19 (MISO)
SCK        →    GPIO 18 (SCK)
CS         →    GPIO 5 (or custom)
RST        →    GPIO 4 (optional)
```

#### STM32 + W5500

```
W5500 Module    STM32 (Blue Pill)
VCC        →    3.3V
GND        →    GND
MOSI       →    PA7 (SPI1_MOSI)
MISO       →    PA6 (SPI1_MISO)
SCK        →    PA5 (SPI1_SCK)
CS         →    PA4 (or custom)
RST        →    PA3 (optional)
```

## Basic Usage

### Single Instance (Simple)

```cpp
#include <Ethernet3.h>

// Network configuration
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  
  // Initialize Ethernet with default W5500 on pin 10
  Ethernet.begin(mac, ip, gateway, subnet);
  
  // Check connection
  if (Ethernet.linkActive()) {
    Serial.println("Ethernet connected successfully");
    Serial.print("IP address: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("Failed to connect Ethernet");
  }
}

void loop() {
  // Your application code
}
```

### DHCP Configuration

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Serial.begin(115200);
  
  // Start with DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Try static configuration as fallback
    IPAddress ip(192, 168, 1, 177);
    Ethernet.begin(mac, ip);
  }
  
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
}
```

### Custom Chip and Pin Configuration

```cpp
#include <Ethernet3.h>

// Create custom Ethernet instance
Ethernet3Class myEthernet(CHIP_TYPE_W5500, 8);  // W5500 on pin 8

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

void setup() {
  Serial.begin(115200);
  
  // Initialize custom instance
  myEthernet.begin(mac, ip);
  
  if (myEthernet.linkActive()) {
    Serial.println("Custom Ethernet connected");
    Serial.print("IP: ");
    Serial.println(myEthernet.localIP());
  }
}
```

## Platform-Specific Configuration

### ESP32 Optimization

```cpp
#include <Ethernet3.h>

void setup() {
  // ESP32 will automatically use optimized HAL and SPI bus
  // Features enabled:
  // - 16MHz SPI speed
  // - FreeRTOS task optimization  
  // - Dual-core support
  // - Critical sections for thread safety
  
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  Ethernet.begin(mac);  // DHCP
  
  Serial.println("ESP32 optimized Ethernet initialized");
}
```

### STM32 Optimization

```cpp
#include <Ethernet3.h>

void setup() {
  // STM32 will automatically use:
  // - 12MHz SPI speed
  // - DMA transfers where available
  // - Hardware acceleration
  // - Fast GPIO operations
  
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  Ethernet.begin(mac);
  
  Serial.println("STM32 optimized Ethernet initialized");
}
```

## Basic Web Client Example

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
char server[] = "www.google.com";

EthernetClient client;

void setup() {
  Serial.begin(115200);
  
  // Start Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    while(1);
  }
  
  delay(1000);  // Give network time to initialize
  
  Serial.println("Connecting to server...");
  
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
    
    // Make HTTP request
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Connection failed");
  }
}

void loop() {
  // Read response
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  
  // Close connection when done
  if (!client.connected()) {
    Serial.println("\nDisconnecting from server");
    client.stop();
    
    // Don't do anything more
    while(1);
  }
}
```

## Basic Web Server Example

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

EthernetServer server(80);

void setup() {
  Serial.begin(115200);
  
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("Server started at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  EthernetClient client = server.available();
  
  if (client) {
    Serial.println("New client connected");
    
    // Read HTTP request
    String request = "";
    while (client.connected() && client.available()) {
      String line = client.readStringUntil('\n');
      request += line + "\n";
      if (line == "\r") break;  // End of headers
    }
    
    // Send HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    
    // Send HTML page
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head><title>Ethernet3 Server</title></head>");
    client.println("<body>");
    client.println("<h1>Hello from Ethernet3!</h1>");
    client.print("<p>Uptime: ");
    client.print(millis());
    client.println(" ms</p>");
    client.println("</body>");
    client.println("</html>");
    
    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}
```

## Troubleshooting

### Common Issues

**"Failed to configure Ethernet using DHCP"**
- Check physical connections
- Verify cable is plugged in
- Try static IP configuration
- Check if DHCP server is available

**Compilation Errors**
- Ensure correct board selected in Arduino IDE
- Verify SPI library is available
- Check for conflicting libraries

**No Network Activity**
- Verify chip select pin configuration
- Check SPI wiring (MOSI, MISO, SCK)
- Ensure proper power supply (3.3V for most modules)
- Test with different cables

### Debugging Tips

```cpp
// Enable verbose output
void setup() {
  Serial.begin(115200);
  
  // Check link before initialization
  Serial.print("Link status before init: ");
  Serial.println(Ethernet.linkActive());
  
  // Initialize and check again
  Ethernet.begin(mac, ip);
  
  Serial.print("Link status after init: ");
  Serial.println(Ethernet.linkActive());
  
  Serial.print("Local IP: ");
  Serial.println(Ethernet.localIP());
}
```

## Next Steps

- Explore [Multi-Instance Architecture](multi-instance.md) for advanced applications
- Learn about [UDP Multicast](udp-multicast.md) for group communication
- Check [Examples](examples.md) for more complete applications
- Review [API Reference](api-reference.md) for detailed documentation
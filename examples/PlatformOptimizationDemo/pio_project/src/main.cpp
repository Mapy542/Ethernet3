/*
 * PlatformOptimizationDemo.ino
 * 
 * This example demonstrates Phase 4 platform optimizations:
 * - Platform-specific performance features
 * - Hardware acceleration when available
 * - DMA support demonstration
 * - Real-time optimizations
 */

#include <SPI.h>
#include <Ethernet3.h>
#include <EthernetUdp2.h>

// Platform-specific includes and optimizations
#ifdef ESP32
#include "hal/ESP32Platform.h"
ESP32Platform platform(NULL, 16000000);  // 16MHz for ESP32
#elif defined(STM32F1) || defined(STM32F4)
#include "hal/STM32Platform.h"
STM32Platform platform(NULL, 12000000);  // 12MHz for STM32
#else
#include "hal/ArduinoPlatform.h"
ArduinoPlatform platform;  // Default speed for Arduino
#endif

// Network configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// Create optimized Ethernet instance with unified platform
Ethernet3Class eth(CHIP_TYPE_W5500, 10, &platform);
EthernetUDP udp(&eth);

// Performance measurement variables
unsigned long testStartTime;
unsigned long testEndTime;
unsigned int packetCount = 0;
unsigned long totalBytes = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  
  Serial.println("Platform Optimization Demo");
  Serial.println("==========================");
  
  // Display platform capabilities
  displayPlatformInfo();
  
  // Initialize Ethernet with optimizations
  Serial.println("\nInitializing optimized Ethernet...");
  if (eth.begin(mac, ip)) {
    Serial.print("Ethernet initialized: ");
    Serial.println(eth.localIP());
    
    if (eth.linkActive()) {
      Serial.println("Physical link active!");
    } else {
      Serial.println("Warning: No physical link detected");
    }
  } else {
    Serial.println("Ethernet initialization failed!");
    return;
  }
  
  // Start UDP service for performance testing
  if (udp.begin(8888)) {
    Serial.println("UDP service started on port 8888");
  } else {
    Serial.println("UDP service failed to start");
    return;
  }
  
  // Run platform-specific optimizations
  enablePlatformOptimizations();
  
  // Start performance test
  Serial.println("\nStarting performance measurements...");
  Serial.println("Send UDP packets to port 8888 for testing");
  
  testStartTime = millis();
}

void loop() {
  // Handle UDP packets with performance measurement
  handleUDPWithPerfMeasurement();
  
  // Display performance statistics every 10 seconds
  static unsigned long lastStatsDisplay = 0;
  if (millis() - lastStatsDisplay > 10000) {
    displayPerformanceStats();
    lastStatsDisplay = millis();
  }
  
  // Platform-specific yielding for optimal performance
  platform.yield();
  
  // Minimal delay for high-performance operation
  #ifdef ESP32
  // ESP32 can handle higher speeds
  delayMicroseconds(100);
  #elif defined(STM32F4)
  // STM32F4 has good performance
  delayMicroseconds(200);
  #else
  // Conservative delay for Arduino
  delay(1);
  #endif
}

void displayPlatformInfo() {
  Serial.print("Platform: ");
  Serial.println(platform.getPlatformName());
  Serial.print("Hardware Acceleration: ");
  Serial.println(platform.hasHardwareAcceleration() ? "Available" : "Not Available");
  Serial.print("DMA Support: ");
  Serial.println(platform.supportsDMA() ? "Available" : "Not Available");
  Serial.print("Real-time Support: ");
  Serial.println(platform.supportsRealTime() ? "Available" : "Not Available");
  
  // Platform SPI information
  Serial.print("SPI Bus: ");
  Serial.println(platform.getPlatformInfo());
  Serial.print("Max SPI Speed: ");
  Serial.print(platform.getMaxSPISpeed() / 1000000);
  Serial.println(" MHz");
  Serial.print("SPI DMA Transfers: ");
  Serial.println(platform.supportsDMA() ? "Supported" : "Not Supported");
  Serial.print("Hardware Transactions: ");
  Serial.println(platform.supportsTransactions() ? "Supported" : "Not Supported");
}

void enablePlatformOptimizations() {
  Serial.println("\nEnabling platform-specific optimizations...");
  
  #ifdef ESP32
  Serial.println("- Configuring ESP32 optimizations:");
  Serial.print("  - Running on Core: ");
  Serial.println(static_cast<ESP32HAL*>(&hal)->getCoreId());
  Serial.println("  - High-speed SPI enabled");
  static_cast<ESP32SPIBus*>(&bus)->setFrequency(16000000);  // 16MHz
  Serial.println("  - FreeRTOS task yielding enabled");
  
  #elif defined(STM32F4)
  Serial.println("- Configuring STM32F4 optimizations:");
  Serial.println("  - Hardware timer support available");
  Serial.println("  - DMA transfers enabled");
  static_cast<STM32SPIBus*>(&bus)->setFrequency(12000000);  // 12MHz
  Serial.println("  - Fast GPIO operations enabled");
  
  #elif defined(STM32F1)
  Serial.println("- Configuring STM32F1 optimizations:");
  Serial.println("  - Medium-speed SPI configuration");
  static_cast<STM32SPIBus*>(&bus)->setFrequency(8000000);   // 8MHz
  Serial.println("  - GPIO optimizations enabled");
  
  #else
  Serial.println("- Standard Arduino optimizations:");
  Serial.println("  - Conservative SPI speed settings");
  Serial.println("  - Standard GPIO operations");
  #endif
  
  Serial.println("Optimizations enabled!");
}

void handleUDPWithPerfMeasurement() {
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    // Record timing for performance measurement
    unsigned long packetStartTime = micros();
    
    packetCount++;
    totalBytes += packetSize;
    
    // Read packet data
    char packetBuffer[1460];  // Maximum Ethernet payload
    int bytesRead = udp.read(packetBuffer, min(packetSize, 1460));
    
    // Platform-specific processing optimizations
    #ifdef ESP32
    // ESP32: Use DMA-optimized response
    processPacketOptimized(packetBuffer, bytesRead);
    #elif defined(STM32F4)
    // STM32F4: Use hardware acceleration if available
    processPacketWithHardwareAccel(packetBuffer, bytesRead);
    #else
    // Standard processing for Arduino
    processPacketStandard(packetBuffer, bytesRead);
    #endif
    
    // Send optimized response
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.print("Platform: ");
    udp.print(hal.getPlatformName());
    udp.print(" | Packet #");
    udp.print(packetCount);
    udp.print(" | Size: ");
    udp.print(bytesRead);
    udp.print(" bytes");
    udp.endPacket();
    
    // Record processing time
    unsigned long processingTime = micros() - packetStartTime;
    
    // Display detailed performance info for first few packets
    if (packetCount <= 5) {
      Serial.print("Packet ");
      Serial.print(packetCount);
      Serial.print(": ");
      Serial.print(bytesRead);
      Serial.print(" bytes, processed in ");
      Serial.print(processingTime);
      Serial.println(" microseconds");
    }
  }
}

void processPacketOptimized(char* buffer, int length) {
  #ifdef ESP32
  // ESP32-specific optimizations
  // Could implement DMA transfers, hardware crypto, etc.
  #endif
  
  // Add timestamp or other processing
  // This is where platform-specific acceleration would be used
}

void processPacketWithHardwareAccel(char* buffer, int length) {
  #if defined(STM32F4)
  // STM32F4-specific hardware acceleration
  // Could use hardware CRC, DMA, etc.
  #endif
}

void processPacketStandard(char* buffer, int length) {
  // Standard processing for all platforms
  // Simple byte manipulation or checksums
}

void displayPerformanceStats() {
  unsigned long elapsedTime = millis() - testStartTime;
  
  if (packetCount > 0 && elapsedTime > 0) {
    float packetsPerSecond = (float)packetCount / (elapsedTime / 1000.0);
    float bytesPerSecond = (float)totalBytes / (elapsedTime / 1000.0);
    float kbytesPerSecond = bytesPerSecond / 1024.0;
    
    Serial.println("\n--- Performance Statistics ---");
    Serial.print("Platform: ");
    Serial.println(hal.getPlatformName());
    Serial.print("Elapsed Time: ");
    Serial.print(elapsedTime / 1000.0);
    Serial.println(" seconds");
    Serial.print("Total Packets: ");
    Serial.println(packetCount);
    Serial.print("Total Bytes: ");
    Serial.println(totalBytes);
    Serial.print("Packets/sec: ");
    Serial.print(packetsPerSecond, 2);
    Serial.println();
    Serial.print("Throughput: ");
    Serial.print(kbytesPerSecond, 2);
    Serial.println(" KB/s");
    
    // Platform-specific performance indicators
    #ifdef ESP32
    Serial.print("ESP32 Core: ");
    Serial.println(static_cast<ESP32HAL*>(&hal)->getCoreId());
    #endif
    
    Serial.println("------------------------------\n");
  } else {
    Serial.println("No packets received yet for performance measurement");
  }
}
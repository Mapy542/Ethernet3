# Platform Support

Ethernet3 provides comprehensive platform support with automatic optimizations for different microcontroller architectures.

## Supported Platforms

### Arduino AVR (ATmega328P, ATmega2560)

**Boards:** Arduino Uno, Nano, Mini, Mega, Leonardo, Micro

**Features:**
- Conservative optimizations for maximum compatibility
- Full backward compatibility with Ethernet2
- Yield support for cooperative multitasking
- Memory-optimized implementations

**SPI Speed:** Up to 8MHz (hardware limitation)

**Configuration:**
```cpp
#include <Ethernet3.h>
// Automatically detects Arduino AVR platform
// Uses ArduinoHAL and ArduinoSPIBus
```

**PlatformIO Configuration:**
```ini
[env:arduino_uno]
platform = atmelavr
board = uno
framework = arduino
lib_deps = Ethernet3
```

### ESP32

**Boards:** ESP32 Dev Module, ESP32-WROVER, ESP32-S2, ESP32-C3

**Features:**
- 16MHz SPI speed (2x faster than Arduino)
- FreeRTOS task integration and thread safety
- Dual-core support with proper task distribution
- Critical sections for SPI protection
- Automatic task yielding in long operations
- Hardware timer usage for precise timing

**Optimizations:**
- DMA support for large transfers
- Non-blocking operations where possible
- FreeRTOS mutex protection
- Interrupt-safe operations

**Configuration:**
```cpp
#include <Ethernet3.h>
// Automatically detects ESP32 platform
// Uses ESP32HAL and ESP32SPIBus with 16MHz SPI
```

**PlatformIO Configuration:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = Ethernet3
build_flags = -DESP32
```

**ESP32-Specific Features:**
```cpp
void setup() {
  // ESP32 features automatically enabled:
  // - 16MHz SPI speed
  // - FreeRTOS integration
  // - Critical sections
  // - Task yielding
  
  Ethernet.begin(mac);
  
  // Create FreeRTOS task for networking
  xTaskCreate(networkTask, "Network", 4096, NULL, 1, NULL);
}

void networkTask(void* parameter) {
  while (true) {
    // Network operations with automatic yielding
    EthernetClient client;
    client.connect("server.com", 80);
    // Task yielding happens automatically
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
```

### STM32

**Supported Series:** STM32F1, STM32F4, STM32F7, STM32L4

**Boards:** Blue Pill (F103C8), Nucleo F401RE, Nucleo F746ZG, Discovery boards

**Features:**
- 12MHz SPI speed on most series
- DMA transfers for high-speed communication
- Hardware acceleration features
- Fast GPIO operations using hardware registers
- Hardware timer integration
- Interrupt-driven operations

**Optimizations:**
- DMA circular buffers for continuous transfers
- Hardware CRC calculation where available
- Power management integration
- Real-time performance enhancements

**Configuration:**
```cpp
#include <Ethernet3.h>
// Automatically detects STM32 platform
// Uses STM32HAL and STM32SPIBus with DMA
```

**PlatformIO Configuration:**
```ini
[env:nucleo_f401re]
platform = ststm32
board = nucleo_f401re
framework = arduino
lib_deps = Ethernet3
build_flags = -DSTM32F4
```

**STM32-Specific Features:**
```cpp
void setup() {
  // STM32 features automatically enabled:
  // - 12MHz SPI with DMA
  // - Hardware acceleration
  // - Fast GPIO operations
  // - Hardware timers
  
  Ethernet.begin(mac);
  
  // DMA transfers happen automatically for large packets
}
```

### ESP8266

**Boards:** NodeMCU, Wemos D1, ESP8266 Dev Board

**Features:**
- 10MHz SPI speed
- Yield support for WiFi coexistence
- Memory-optimized for limited RAM
- Software timer integration

**Configuration:**
```cpp
#include <Ethernet3.h>
// Uses ESP8266-optimized implementations
```

**PlatformIO Configuration:**
```ini
[env:esp8266]
platform = espressif8266
board = nodemcuv2
framework = arduino
lib_deps = Ethernet3
build_flags = -DESP8266
```

### Raspberry Pi Pico (RP2040)

**Features:**
- Dual-core ARM Cortex-M0+
- High-speed SPI capability
- DMA support
- Hardware timer integration

**Configuration:**
```ini
[env:pico]
platform = raspberrypi
board = pico
framework = arduino
lib_deps = Ethernet3
build_flags = -DPICO
```

### Teensy

**Supported:** Teensy 3.x, Teensy 4.x series

**Features:**
- High-speed SPI (up to 30MHz on Teensy 4.x)
- DMA transfers
- Hardware acceleration
- Real-time performance

**Configuration:**
```ini
[env:teensy40]
platform = teensy
board = teensy40
framework = arduino
lib_deps = Ethernet3
build_flags = -DTEENSY4
```

## Platform Detection

Ethernet3 automatically detects your platform and applies appropriate optimizations:

```cpp
#include <Ethernet3.h>

void setup() {
  Serial.begin(115200);
  
  // Platform information is automatically configured
  Serial.println("Platform Information:");
  
#ifdef ESP32
  Serial.println("ESP32 detected - 16MHz SPI, FreeRTOS enabled");
#elif defined(STM32F4)
  Serial.println("STM32F4 detected - 12MHz SPI, DMA enabled");
#elif defined(STM32F1)
  Serial.println("STM32F1 detected - Hardware optimizations enabled");
#elif defined(ESP8266)
  Serial.println("ESP8266 detected - Yield support enabled");
#elif defined(ARDUINO_ARCH_AVR)
  Serial.println("Arduino AVR detected - Conservative optimizations");
#elif defined(TEENSY)
  Serial.println("Teensy detected - High-speed SPI enabled");
#else
  Serial.println("Generic platform - Standard compatibility mode");
#endif
  
  Ethernet.begin(mac);
}
```

## Performance Comparison

| Platform | SPI Speed | Special Features | Throughput | Notes |
|----------|-----------|------------------|------------|-------|
| Arduino AVR | 8MHz | Yield support | ~5 Mbps | Conservative, stable |
| ESP32 | 16MHz | FreeRTOS, DMA | ~20 Mbps | Dual-core, thread-safe |
| STM32F4 | 12MHz | DMA, Hardware accel | ~15 Mbps | Real-time capable |
| STM32F1 | 8MHz | Fast GPIO | ~8 Mbps | Good for control apps |
| ESP8266 | 10MHz | Yield support | ~8 Mbps | WiFi coexistence |
| Teensy 4.x | 30MHz | High-speed DMA | ~35 Mbps | Maximum performance |
| RP2040 | 16MHz | Dual-core | ~18 Mbps | Good price/performance |

## Hardware Abstraction Layer (HAL)

Ethernet3 uses a Hardware Abstraction Layer to provide consistent APIs across platforms:

### ArduinoHAL
```cpp
class ArduinoHAL {
public:
  void delay(uint32_t ms);
  void yield();
  uint32_t millis();
  // Conservative implementations for maximum compatibility
};
```

### ESP32HAL
```cpp
class ESP32HAL {
public:
  void delay(uint32_t ms);
  void yield();                    // FreeRTOS task yield
  uint32_t millis();
  void enterCriticalSection();     // Interrupt protection
  void exitCriticalSection();
  void createTask(TaskFunction_t task, const char* name, uint32_t stackSize);
};
```

### STM32HAL
```cpp
class STM32HAL {
public:
  void delay(uint32_t ms);
  void yield();
  uint32_t millis();
  void configureDMA(uint32_t channel, uint32_t mode);
  void enableHardwareAcceleration();
  void fastGPIOWrite(uint32_t pin, bool value);
};
```

## SPI Bus Implementations

### ArduinoSPIBus
```cpp
class ArduinoSPIBus {
  uint32_t maxSpeed = 8000000;     // 8MHz max for stability
  void beginTransaction();
  void endTransaction();
  uint8_t transfer(uint8_t data);
  void transfer(uint8_t* buffer, size_t length);
};
```

### ESP32SPIBus  
```cpp
class ESP32SPIBus {
  uint32_t maxSpeed = 16000000;    // 16MHz for performance
  bool dmaEnabled = true;
  void beginTransaction();
  void endTransaction();
  uint8_t transfer(uint8_t data);
  void transferDMA(uint8_t* buffer, size_t length);  // DMA for large transfers
};
```

### STM32SPIBus
```cpp
class STM32SPIBus {
  uint32_t maxSpeed = 12000000;    // 12MHz with DMA
  bool dmaEnabled = true;
  void beginTransaction();
  void endTransaction();
  uint8_t transfer(uint8_t data);
  void transferDMA(uint8_t* buffer, size_t length);  // Hardware DMA
  void configureDMACircular();     // Continuous transfers
};
```

## Platform-Specific Optimizations

### ESP32 Optimizations

**FreeRTOS Integration:**
```cpp
// Automatic task yielding in long operations
void longOperation() {
  for (int i = 0; i < 1000; i++) {
    // Process data
    processPacket(i);
    
    // Automatic yield every 10 iterations
    if (i % 10 == 0) {
      taskYIELD();  // Let other tasks run
    }
  }
}

// Critical section protection for SPI
void spiOperation() {
  taskENTER_CRITICAL(&spiMutex);
  // SPI operation protected from interrupts
  SPI.transfer(data);
  taskEXIT_CRITICAL(&spiMutex);
}
```

**Dual-Core Utilization:**
```cpp
void setup() {
  // Network operations can use both cores
  Ethernet.begin(mac);
  
  // Core 0: Network processing
  xTaskCreatePinnedToCore(networkTask, "Network", 4096, NULL, 1, NULL, 0);
  
  // Core 1: Application logic  
  xTaskCreatePinnedToCore(appTask, "App", 4096, NULL, 1, NULL, 1);
}
```

### STM32 Optimizations

**DMA Transfers:**
```cpp
// Large packet transfers use DMA automatically
void sendLargePacket(uint8_t* data, size_t length) {
  if (length > 64) {
    // Use DMA for large transfers
    configureDMA(data, length);
    startDMATransfer();
    waitForDMAComplete();
  } else {
    // Use regular SPI for small transfers
    SPI.transfer(data, length);
  }
}
```

**Hardware Acceleration:**
```cpp
// CRC calculation using hardware
uint32_t calculateCRC(uint8_t* data, size_t length) {
#ifdef STM32_CRC_HARDWARE
  return HAL_CRC_Calculate(&hcrc, (uint32_t*)data, length);
#else
  return softwareCRC(data, length);
#endif
}
```

### Arduino AVR Optimizations

**Memory Efficiency:**
```cpp
// Minimize RAM usage on constrained platforms
void conserveMemory() {
  // Use PROGMEM for constants
  static const char response[] PROGMEM = "HTTP/1.1 200 OK\r\n";
  
  // Reuse buffers
  static uint8_t buffer[256];
  
  // Minimize dynamic allocation
  // Prefer stack allocation for small objects
}
```

**Yield Support:**
```cpp
void longTask() {
  for (int i = 0; i < 1000; i++) {
    processData(i);
    
    // Yield to other processes
    if (i % 50 == 0) {
      yield();  // Allow other tasks to run
    }
  }
}
```

## Compiler Optimizations

### Build Flags for Performance

**ESP32 Performance:**
```ini
[env:esp32_performance]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = Ethernet3
build_flags = 
    -DESP32
    -O3                          ; Maximum optimization
    -DETHERNET3_HIGH_PERFORMANCE ; Enable performance features
    -DETHERNET3_DMA_ENABLED      ; Enable DMA transfers
```

**STM32 Performance:**
```ini
[env:stm32_performance]
platform = ststm32
board = nucleo_f401re
framework = arduino
lib_deps = Ethernet3
build_flags = 
    -DSTM32F4
    -O3                          ; Maximum optimization
    -DETHERNET3_DMA_ENABLED      ; Enable DMA
    -DETHERNET3_HARDWARE_CRC     ; Use hardware CRC
```

**Arduino Compatibility:**
```ini
[env:arduino_compatible]
platform = atmelavr
board = uno
framework = arduino
lib_deps = Ethernet3
build_flags = 
    -Os                          ; Size optimization
    -DETHERNET3_CONSERVATIVE     ; Conservative features only
```

## Platform-Specific Examples

### ESP32 Multi-Core Example

```cpp
#include <Ethernet3.h>

// Core 0: Network processing
void networkTask(void* parameter) {
  EthernetServer server(80);
  server.begin();
  
  while (true) {
    EthernetClient client = server.available();
    if (client) {
      handleClient(client);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Core 1: Sensor processing  
void sensorTask(void* parameter) {
  while (true) {
    readSensors();
    processData();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Ethernet.begin(mac);
  
  // Start tasks on different cores
  xTaskCreatePinnedToCore(networkTask, "Network", 8192, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(sensorTask, "Sensor", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // Main loop can be empty or handle coordination
  delay(1000);
}
```

### STM32 DMA Example

```cpp
#include <Ethernet3.h>

// Large data buffer
uint8_t dataBuffer[4096];

void setup() {
  Ethernet.begin(mac);
  
  // Configure DMA for high-speed transfers
  Serial.println("STM32 DMA enabled automatically");
}

void sendLargeData() {
  EthernetClient client;
  if (client.connect("server.com", 80)) {
    // DMA transfer happens automatically for large writes
    client.write(dataBuffer, sizeof(dataBuffer));
    client.stop();
  }
}
```

### Arduino Memory-Optimized Example

```cpp
#include <Ethernet3.h>

// Use PROGMEM to save RAM
const char htmlHeader[] PROGMEM = 
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "Connection: close\r\n\r\n";

void setup() {
  Ethernet.begin(mac, ip);  // Static IP to save RAM
}

void handleClient(EthernetClient& client) {
  // Use stack buffer to save RAM
  char buffer[64];
  
  // Copy from PROGMEM
  strcpy_P(buffer, htmlHeader);
  client.print(buffer);
  
  // Yield to other processes
  yield();
}
```

## Troubleshooting Platform Issues

### ESP32 Issues

**Problem:** Random crashes or resets
**Solution:** Check stack sizes for FreeRTOS tasks
```cpp
// Increase stack size if needed
xTaskCreate(networkTask, "Network", 8192, NULL, 1, NULL);  // 8KB stack
```

**Problem:** SPI conflicts with WiFi
**Solution:** Use different SPI pins or disable WiFi
```cpp
// Use HSPI instead of VSPI if WiFi is needed
SPIClass hspi(HSPI);
```

### STM32 Issues

**Problem:** DMA conflicts
**Solution:** Check DMA channel assignments
```cpp
// Verify DMA channels in platformio.ini
build_flags = -DETHERNET3_DMA_CHANNEL=DMA1_Channel2
```

**Problem:** Clock configuration
**Solution:** Ensure proper system clock setup
```cpp
// Check HAL_RCC_ClockConfig in system initialization
```

### Arduino Issues

**Problem:** Out of memory
**Solution:** Reduce buffer sizes and use PROGMEM
```cpp
// Reduce socket buffer sizes
#define ETHERNET3_SOCKET_BUFFER_SIZE 512  // Instead of 2048
```

**Problem:** SPI speed too high
**Solution:** Force lower speed
```cpp
// Override maximum SPI speed
#define ETHERNET3_MAX_SPI_SPEED 4000000  // 4MHz instead of 8MHz
```

## Best Practices

### Cross-Platform Development

1. **Test on Multiple Platforms**: Verify your code works on different architectures
2. **Use Platform Detection**: Adapt behavior based on capabilities
3. **Conservative Defaults**: Start with compatible settings, then optimize
4. **Memory Awareness**: Consider RAM limitations on smaller platforms
5. **Yield Appropriately**: Use yield() in long operations for cooperative multitasking

### Performance Tuning

1. **Profile First**: Measure before optimizing
2. **Platform-Specific Builds**: Use different optimization levels per platform
3. **Buffer Sizing**: Adjust based on available RAM
4. **SPI Speed**: Test maximum stable speeds for your hardware
5. **DMA Usage**: Enable for high-throughput applications

The platform abstraction in Ethernet3 ensures your code works across all supported platforms while automatically applying the best optimizations for each target.
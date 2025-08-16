/**
 * W5500SingletonDemo.ino - Demonstrates conditional W5500 singleton behavior
 * 
 * This example shows how the Ethernet3 library handles the W5500 singleton
 * based on the ETHERNET3_NO_BACKWARDS_COMPATIBILITY preprocessor directive.
 */

#include <Arduino.h>

// Uncomment the line below to enable modern mode (no backward compatibility)
// #define ETHERNET3_NO_BACKWARDS_COMPATIBILITY

#include <Ethernet3.h>

uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("W5500 Singleton Demo - Ethernet3 Library");
    Serial.println("==========================================");
    
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
    Serial.println("Mode: Backward Compatibility (Default)");
    
    // Legacy syntax works
    Serial.print("Starting Ethernet with legacy syntax... ");
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed!");
    } else {
        Serial.println("Success!");
        Serial.print("IP address: ");
        Serial.println(Ethernet.localIP());
    }
    
    // Network classes work with default singleton
    EthernetServer server(80);  // Uses global Ethernet
    EthernetUDP udp;           // Uses global Ethernet
    Serial.println("Network classes created with singleton reference");
    
#else
    Serial.println("Mode: Modern (No Backward Compatibility)");
    
    // Must create explicit instances
    Serial.print("Creating explicit Ethernet instance... ");
    Ethernet3Class eth(CHIP_TYPE_W5500, 10);
    Serial.println("Success!");
    
    Serial.print("Starting Ethernet with explicit instance... ");
    if (eth.begin(mac) == 0) {
        Serial.println("Failed!");
    } else {
        Serial.println("Success!");
        Serial.print("IP address: ");
        Serial.println(eth.localIP());
    }
    
    // Network classes require chip instance
    EthernetServer server(80, &eth);  // Must pass chip instance
    EthernetUDP udp(&eth);           // Must pass chip instance
    Serial.println("Network classes created with explicit chip reference");
#endif

    Serial.println("Demo completed successfully!");
}

void loop() {
    delay(5000);
    
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
    Serial.print("Link active (global): ");
    Serial.println(Ethernet.linkActive() ? "Yes" : "No");
#else
    Serial.println("Link monitoring available through explicit instances");
#endif
}
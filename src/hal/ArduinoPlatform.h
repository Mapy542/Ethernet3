/*
 * ArduinoPlatform.h - Arduino unified platform implementation for Ethernet3 library
 */

#ifndef ARDUINO_PLATFORM_H
#define ARDUINO_PLATFORM_H

#include "EthernetPlatform.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>

/**
 * Arduino-specific unified platform implementation
 */
class ArduinoPlatform : public EthernetPlatform {
private:
    SPISettings settings;
    uint8_t cs_pin;
    
public:
    ArduinoPlatform() : settings(8000000, MSBFIRST, SPI_MODE0), cs_pin(10) {}
    
    // Hardware abstraction implementation
    void delay(uint32_t ms) override {
        ::delay(ms);
    }
    
    uint32_t millis() override {
        return ::millis();
    }
    
    void digitalWrite(uint8_t pin, uint8_t value) override {
        ::digitalWrite(pin, value);
    }
    
    void pinMode(uint8_t pin, uint8_t mode) override {
        ::pinMode(pin, mode);
    }
    
    uint8_t digitalRead(uint8_t pin) override {
        return ::digitalRead(pin);
    }
    
    void yield() override {
        #ifdef ESP8266
        ::yield();  // ESP8266 has built-in yield
        #endif
    }
    
    // SPI communication implementation
    void spiBegin(uint8_t cs_pin) override {
        this->cs_pin = cs_pin;
        pinMode(cs_pin, OUTPUT);
        digitalWrite(cs_pin, HIGH);
        SPI.begin();
    }
    
    void spiEnd() override {
        SPI.end();
    }
    
    uint8_t spiTransfer(uint8_t data) override {
        return SPI.transfer(data);
    }
    
    void spiTransfer(uint8_t* buffer, size_t length) override {
        SPI.transfer(buffer, length);
    }
    
    void spiSetBitOrder(uint8_t order) override {
        // Update settings and apply
        settings = SPISettings(8000000, order, SPI_MODE0);
    }
    
    void spiSetDataMode(uint8_t mode) override {
        // Update settings and apply
        settings = SPISettings(8000000, MSBFIRST, mode);
    }
    
    void spiSetClockDivider(uint8_t rate) override {
        // Convert Arduino clock divider to frequency
        uint32_t freq = 16000000 / rate;  // Assuming 16MHz Arduino
        settings = SPISettings(freq, MSBFIRST, SPI_MODE0);
    }
    
    void spiBeginTransaction() override {
        SPI.beginTransaction(settings);
        digitalWrite(cs_pin, LOW);
    }
    
    void spiEndTransaction() override {
        digitalWrite(cs_pin, HIGH);
        SPI.endTransaction();
    }
    
    // Platform capability implementation
    const char* getPlatformName() override {
        #if defined(__AVR_ATmega328P__)
        return "Arduino Uno (ATmega328P)";
        #elif defined(__AVR_ATmega2560__)
        return "Arduino Mega (ATmega2560)";
        #elif defined(__AVR_ATmega32U4__)
        return "Arduino Leonardo (ATmega32U4)";
        #elif defined(ESP8266)
        return "ESP8266";
        #elif defined(ESP32)
        return "ESP32";
        #elif defined(STM32F1)
        return "STM32F1";
        #elif defined(STM32F4)
        return "STM32F4";
        #else
        return "Arduino Compatible";
        #endif
    }
    
    bool hasHardwareAcceleration() override {
        #if defined(ESP32) || defined(STM32F4)
        return true;  // These platforms have hardware acceleration
        #else
        return false;
        #endif
    }
    
    bool supportsDMA() override {
        #if defined(ESP32) || defined(STM32F1) || defined(STM32F4)
        return true;
        #else
        return false;
        #endif
    }
    
    bool supportsRealTime() override {
        #if defined(ESP32)
        return true;  // ESP32 has FreeRTOS
        #else
        return false;
        #endif
    }
    
    bool supportsTransactions() override {
        return true;  // Arduino SPI library supports transactions
    }
    
    uint32_t getMaxSPISpeed() override {
        #if defined(ESP32)
        return 16000000;  // 16MHz for ESP32
        #elif defined(ESP8266)
        return 10000000;  // 10MHz for ESP8266
        #else
        return 8000000;   // 8MHz for standard Arduino
        #endif
    }
    
    const char* getPlatformInfo() override {
        return "Arduino Platform with SPI";
    }
};

#endif // ARDUINO

#endif // ARDUINO_PLATFORM_H
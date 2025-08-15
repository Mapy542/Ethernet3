/*
 * ArduinoHAL.h - Arduino Hardware Abstraction Layer implementation for Ethernet3 library
 */

#ifndef ARDUINO_HAL_H
#define ARDUINO_HAL_H

#include "EthernetHAL.h"

#ifdef ARDUINO
#include <Arduino.h>

/**
 * Arduino-specific HAL implementation
 */
class ArduinoHAL : public EthernetHAL {
public:
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
};

#endif // ARDUINO

#endif // ARDUINO_HAL_H
/*
 * ArduinoHAL.h - Arduino Hardware Abstraction Layer implementation for Ethernet3 library
 */

#ifndef ARDUINO_HAL_H
#define ARDUINO_HAL_H

#include "../EthernetHAL.h"

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
};

#endif // ARDUINO

#endif // ARDUINO_HAL_H
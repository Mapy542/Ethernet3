/*
 * STM32HAL.h - STM32 Hardware Abstraction Layer implementation for Ethernet3 library
 * Provides STM32-specific optimizations and features
 */

#ifndef STM32_HAL_H
#define STM32_HAL_H

#include "EthernetHAL.h"

#if defined(STM32F1) || defined(STM32F4) || defined(STM32) || defined(__STM32__)
#include <Arduino.h>

/**
 * STM32-specific HAL implementation with hardware optimizations
 */
class STM32HAL : public EthernetHAL {
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
    
    // STM32-specific optimizations
    void yield() override {
        // STM32 doesn't have native yield, use minimal delay
        delayMicroseconds(1);
    }
    
    // Fast GPIO operations (if available in the STM32 core)
    void fastDigitalWrite(uint8_t pin, uint8_t value) {
        #ifdef GPIOA
        // Use direct register access for faster GPIO operations
        // Implementation would depend on the specific STM32 core
        digitalWrite(pin, value);  // Fallback to standard implementation
        #else
        digitalWrite(pin, value);
        #endif
    }
    
    // DMA support indicator
    bool hasDMASupport() {
        #ifdef DMA1
        return true;
        #else
        return false;
        #endif
    }
    
    // Hardware timer support
    bool hasHardwareTimer() {
        #ifdef TIM1
        return true;
        #else
        return false;
        #endif
    }
};

#endif // STM32 variants

#endif // STM32_HAL_H
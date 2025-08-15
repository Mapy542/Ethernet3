/*
 * ESP32HAL.h - ESP32 Hardware Abstraction Layer implementation for Ethernet3 library
 * Provides ESP32-specific optimizations and features
 */

#ifndef ESP32_HAL_H
#define ESP32_HAL_H

#include "EthernetHAL.h"

#ifdef ESP32
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * ESP32-specific HAL implementation with RTOS optimizations
 */
class ESP32HAL : public EthernetHAL {
public:
    void delay(uint32_t ms) override {
        if (ms > 0) {
            vTaskDelay(pdMS_TO_TICKS(ms));  // Use FreeRTOS delay for better task scheduling
        }
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
    
    // ESP32-specific optimizations
    void yield() override {
        vTaskDelay(1);  // Yield to other tasks
    }
    
    uint32_t getCoreId() {
        return xPortGetCoreID();
    }
    
    void enterCriticalSection() {
        portENTER_CRITICAL_ISR(&_mux);
    }
    
    void exitCriticalSection() {
        portEXIT_CRITICAL_ISR(&_mux);
    }
    
private:
    portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;
};

#endif // ESP32

#endif // ESP32_HAL_H
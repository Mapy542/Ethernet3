/*
 * EthernetHAL.h - Hardware Abstraction Layer for Ethernet3 library
 * 
 * This file provides platform-independent hardware abstraction
 * enabling support for multiple platforms while maintaining compatibility.
 */

#ifndef ETHERNET_HAL_H
#define ETHERNET_HAL_H

#include <stdint.h>

/**
 * Abstract base class for hardware abstraction layer.
 * Platform-specific implementations should inherit from this class.
 */
class EthernetHAL {
public:
    virtual ~EthernetHAL() {}
    
    /**
     * Platform-independent delay function
     * @param ms Milliseconds to delay
     */
    virtual void delay(uint32_t ms) = 0;
    
    /**
     * Get current time in milliseconds
     * @return Current time in milliseconds since startup
     */
    virtual uint32_t millis() = 0;
    
    /**
     * Set digital pin output value
     * @param pin Pin number
     * @param value Pin value (HIGH/LOW or 1/0)
     */
    virtual void digitalWrite(uint8_t pin, uint8_t value) = 0;
    
    /**
     * Set pin mode
     * @param pin Pin number  
     * @param mode Pin mode (INPUT/OUTPUT or platform specific constants)
     */
    virtual void pinMode(uint8_t pin, uint8_t mode) = 0;
    
    /**
     * Read digital pin value
     * @param pin Pin number
     * @return Pin value (HIGH/LOW or 1/0)
     */
    virtual uint8_t digitalRead(uint8_t pin) = 0;
};

#endif // ETHERNET_HAL_H
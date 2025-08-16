/*
 * EthernetPlatform.h - Unified platform abstraction for Ethernet3 library
 * 
 * This file provides a unified platform abstraction combining hardware operations
 * and SPI communication into a single interface for simplicity and maintainability.
 */

#ifndef ETHERNET_PLATFORM_H
#define ETHERNET_PLATFORM_H

#include <stdint.h>
#include <stddef.h>

/**
 * Unified platform abstraction class combining hardware and SPI operations.
 * Platform-specific implementations should inherit from this class.
 */
class EthernetPlatform {
public:
    virtual ~EthernetPlatform() {}
    
    // Hardware abstraction methods
    
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
    
    /**
     * Yield execution to other tasks/processes (optional)
     * Default implementation does nothing for platforms without task scheduling
     */
    virtual void yield() {}
    
    // SPI communication methods
    
    /**
     * Initialize the SPI bus with specified chip select pin
     * @param cs_pin The chip select pin number
     */
    virtual void spiBegin(uint8_t cs_pin) = 0;
    
    /**
     * Deinitialize the SPI bus
     */
    virtual void spiEnd() {}
    
    /**
     * Transfer a single byte
     * @param data Byte to send
     * @return Received byte
     */
    virtual uint8_t spiTransfer(uint8_t data) = 0;
    
    /**
     * Transfer multiple bytes
     * @param buffer Buffer containing data to send/receive
     * @param length Number of bytes to transfer
     */
    virtual void spiTransfer(uint8_t* buffer, size_t length) = 0;
    
    /**
     * Set SPI bit order
     * @param order Bit order (platform specific constants)
     */
    virtual void spiSetBitOrder(uint8_t order) = 0;
    
    /**
     * Set SPI data mode
     * @param mode Data mode (platform specific constants)
     */
    virtual void spiSetDataMode(uint8_t mode) = 0;
    
    /**
     * Set SPI clock divider/speed
     * @param rate Clock divider (platform specific constants)
     */
    virtual void spiSetClockDivider(uint8_t rate) = 0;
    
    /**
     * Begin SPI transaction (if supported by platform)
     */
    virtual void spiBeginTransaction() {}
    
    /**
     * End SPI transaction (if supported by platform)
     */
    virtual void spiEndTransaction() {}
    
    // Platform capability methods
    
    /**
     * Get platform identification string
     * @return Platform name/version string
     */
    virtual const char* getPlatformName() { return "Generic"; }
    
    /**
     * Check if platform supports hardware acceleration features
     * @return true if hardware acceleration is available
     */
    virtual bool hasHardwareAcceleration() { return false; }
    
    /**
     * Check if platform supports DMA operations
     * @return true if DMA is available
     */
    virtual bool supportsDMA() { return false; }
    
    /**
     * Check if platform supports real-time operations
     * @return true if real-time features are available  
     */
    virtual bool supportsRealTime() { return false; }
    
    /**
     * Check if the platform supports hardware SPI transaction management
     * @return true if hardware transactions are supported
     */
    virtual bool supportsTransactions() { return false; }
    
    /**
     * Get maximum SPI transfer speed
     * @return Maximum speed in Hz
     */
    virtual uint32_t getMaxSPISpeed() { return 8000000; }  // Default 8MHz
    
    /**
     * Get platform-specific information
     * @return Platform identification string
     */
    virtual const char* getPlatformInfo() { return "Generic Platform with SPI"; }
};

#endif // ETHERNET_PLATFORM_H
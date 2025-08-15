/*
 * EthernetBus.h - Abstract SPI bus interface for Ethernet3 library
 * 
 * This file provides platform-independent SPI bus abstraction
 * enabling support for multiple platforms while maintaining compatibility.
 */

#ifndef ETHERNET_BUS_H
#define ETHERNET_BUS_H

#include <stdint.h>
#include <stddef.h>

/**
 * Abstract base class for SPI bus communication.
 * Platform-specific implementations should inherit from this class.
 */
class EthernetBus {
public:
    virtual ~EthernetBus() {}
    
    /**
     * Initialize the SPI bus with specified chip select pin
     * @param cs_pin The chip select pin number
     */
    virtual void begin(uint8_t cs_pin) = 0;
    
    /**
     * Deinitialize the SPI bus
     */
    virtual void end() {}
    
    /**
     * Transfer a single byte
     * @param data Byte to send
     * @return Received byte
     */
    virtual uint8_t transfer(uint8_t data) = 0;
    
    /**
     * Transfer multiple bytes
     * @param buffer Buffer containing data to send/receive
     * @param length Number of bytes to transfer
     */
    virtual void transfer(uint8_t* buffer, size_t length) = 0;
    
    /**
     * Set SPI bit order
     * @param order Bit order (platform specific constants)
     */
    virtual void setBitOrder(uint8_t order) = 0;
    
    /**
     * Set SPI data mode
     * @param mode Data mode (platform specific constants)
     */
    virtual void setDataMode(uint8_t mode) = 0;
    
    /**
     * Set SPI clock divider/speed
     * @param rate Clock divider (platform specific constants)
     */
    virtual void setClockDivider(uint8_t rate) = 0;
    
    /**
     * Begin SPI transaction (if supported by platform)
     */
    virtual void beginTransaction() {}
    
    /**
     * End SPI transaction (if supported by platform)
     */
    virtual void endTransaction() {}
    
    /**
     * Check if the bus supports DMA transfers
     * @return true if DMA is supported
     */
    virtual bool supportsDMA() { return false; }
    
    /**
     * Check if the bus supports hardware transaction management
     * @return true if hardware transactions are supported
     */
    virtual bool supportsTransactions() { return false; }
    
    /**
     * Get maximum transfer speed
     * @return Maximum speed in Hz
     */
    virtual uint32_t getMaxSpeed() { return 8000000; }  // Default 8MHz
    
    /**
     * Get platform-specific bus information
     * @return Bus identification string
     */
    virtual const char* getBusInfo() { return "Generic SPI"; }
};

#endif // ETHERNET_BUS_H
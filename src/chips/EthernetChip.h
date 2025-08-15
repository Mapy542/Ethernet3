/*
 * EthernetChip.h - Abstract Ethernet chip interface for Ethernet3 library
 * 
 * This file provides a common interface for different Ethernet chips
 * enabling support for W5100, W5500 and future chip variants.
 */

#ifndef ETHERNET_CHIP_H
#define ETHERNET_CHIP_H

#include <stdint.h>
#include "../bus/EthernetBus.h"
#include "../hal/EthernetHAL.h"

// Chip type constants
#define CHIP_TYPE_W5100 0x51
#define CHIP_TYPE_W5500 0x55

/**
 * Abstract base class for Ethernet chip implementations.
 * Specific chip classes (W5100, W5500) should inherit from this class.
 */
class EthernetChip {
protected:
    EthernetBus* bus;     ///< SPI bus interface
    EthernetHAL* hal;     ///< Hardware abstraction layer
    uint8_t cs_pin;       ///< Chip select pin

public:
    /**
     * Constructor
     * @param bus_interface Pointer to SPI bus implementation
     * @param hal_interface Pointer to HAL implementation  
     * @param chip_select_pin Chip select pin number
     */
    EthernetChip(EthernetBus* bus_interface, EthernetHAL* hal_interface, uint8_t chip_select_pin)
        : bus(bus_interface), hal(hal_interface), cs_pin(chip_select_pin) {}
    
    virtual ~EthernetChip() {}
    
    /**
     * Initialize the chip
     * @return true if initialization successful, false otherwise
     */
    virtual bool init() = 0;
    
    /**
     * Check physical link status
     * @return true if link is active, false otherwise
     */
    virtual bool linkActive() = 0;
    
    /**
     * Get chip type identifier
     * @return Chip type constant (CHIP_TYPE_W5100, CHIP_TYPE_W5500, etc.)
     */
    virtual uint8_t getChipType() = 0;
    
    /**
     * Software reset the chip
     */
    virtual void swReset() = 0;
    
    /**
     * Get chip select pin
     * @return Chip select pin number
     */
    uint8_t getCSPin() const { return cs_pin; }
    
    /**
     * Get bus interface
     * @return Pointer to bus interface
     */
    EthernetBus* getBus() const { return bus; }
    
    /**
     * Get HAL interface
     * @return Pointer to HAL interface
     */
    EthernetHAL* getHAL() const { return hal; }
};

#endif // ETHERNET_CHIP_H
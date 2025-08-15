/*
 * W5500Chip.h - W5500 chip implementation for Ethernet3 library
 * 
 * This file provides W5500-specific implementation of the EthernetChip interface
 */

#ifndef W5500_CHIP_H
#define W5500_CHIP_H

#include "../EthernetChip.h"
#include "../utility/w5500.h"

/**
 * W5500 chip implementation
 */
class W5500Chip : public EthernetChip {
private:
    W5500Class* w5500_instance;
    
public:
    /**
     * Constructor
     * @param bus_interface Pointer to SPI bus implementation
     * @param hal_interface Pointer to HAL implementation  
     * @param chip_select_pin Chip select pin number
     */
    W5500Chip(EthernetBus* bus_interface, EthernetHAL* hal_interface, uint8_t chip_select_pin)
        : EthernetChip(bus_interface, hal_interface, chip_select_pin), w5500_instance(nullptr) {}
    
    /**
     * Constructor with existing W5500Class instance (for backward compatibility)
     * @param w5500_inst Pointer to existing W5500Class instance
     * @param bus_interface Pointer to SPI bus implementation
     * @param hal_interface Pointer to HAL implementation  
     * @param chip_select_pin Chip select pin number
     */
    W5500Chip(W5500Class* w5500_inst, EthernetBus* bus_interface, EthernetHAL* hal_interface, uint8_t chip_select_pin)
        : EthernetChip(bus_interface, hal_interface, chip_select_pin), w5500_instance(w5500_inst) {}
    
    bool init() override {
        if (w5500_instance) {
            w5500_instance->init(cs_pin);
            return true;
        }
        return false;
    }
    
    bool linkActive() override {
        if (w5500_instance) {
            // Check physical link status via PHY configuration register  
            // PHYCFGR bit 0 (LNK) indicates link status: 1 = link up, 0 = link down
            uint8_t phy_cfg = w5500_instance->getPHYCFGR();
            return (phy_cfg & 0x01) != 0;
        }
        return false;
    }
    
    uint8_t getChipType() override {
        return CHIP_TYPE_W5500;
    }
    
    void swReset() override {
        if (w5500_instance) {
            w5500_instance->swReset();
        }
    }
    
    /**
     * Get the underlying W5500Class instance
     * @return Pointer to W5500Class instance  
     */
    W5500Class* getW5500() const {
        return w5500_instance;
    }
    
    /**
     * Set the W5500Class instance (for backward compatibility)
     * @param w5500_inst Pointer to W5500Class instance
     */
    void setW5500(W5500Class* w5500_inst) {
        w5500_instance = w5500_inst;
    }
};

#endif // W5500_CHIP_H
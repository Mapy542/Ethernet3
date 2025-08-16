/*
 * W5100Chip.h - W5100 chip implementation for Ethernet3 library
 *
 * This file provides W5100-specific implementation of the EthernetChip interface
 */

#ifndef W5100_CHIP_H
#define W5100_CHIP_H

#include "EthernetChip.h"
#include "utility/w5100.h"

/**
 * W5100 chip implementation
 */
class W5100Chip : public EthernetChip {
   private:
    W5100Class* w5100_instance;

   public:
    /**
     * Constructor
     * @param platform_interface Pointer to unified platform implementation
     * @param chip_select_pin Chip select pin number
     */
    W5100Chip(EthernetPlatform* platform_interface, uint8_t chip_select_pin)
        : EthernetChip(platform_interface, chip_select_pin), w5100_instance(nullptr) {}

    /**
     * Constructor with existing W5100Class instance (for backward compatibility)
     * @param w5100_inst Pointer to existing W5100Class instance
     * @param platform_interface Pointer to unified platform implementation
     * @param chip_select_pin Chip select pin number
     */
    W5100Chip(W5100Class* w5100_inst, EthernetPlatform* platform_interface, uint8_t chip_select_pin)
        : EthernetChip(platform_interface, chip_select_pin), w5100_instance(w5100_inst) {}

    bool init() override {
        if (w5100_instance) {
            w5100_instance->init(cs_pin);
            return true;
        }
        return false;
    }

    bool linkActive() override {
        if (w5100_instance) {
            return w5100_instance->linkActive();
        }
        return false;
    }

    uint8_t getChipType() override { return CHIP_TYPE_W5100; }

    void swReset() override {
        if (w5100_instance) {
            w5100_instance->swReset();
        }
    }

    /**
     * Get the underlying W5100Class instance
     * @return Pointer to W5100Class instance
     */
    W5100Class* getW5100() const { return w5100_instance; }

    /**
     * Set the W5100Class instance (for backward compatibility)
     * @param w5100_inst Pointer to W5100Class instance
     */
    void setW5100(W5100Class* w5100_inst) { w5100_instance = w5100_inst; }

    /**
     * Get maximum number of sockets supported by W5100
     * @return Maximum socket count (4 for W5100)
     */
    uint8_t getMaxSockets() const { return 4; }
};

#endif  // W5100_CHIP_H
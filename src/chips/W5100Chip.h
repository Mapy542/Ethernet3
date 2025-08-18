/*
 * W5100Chip.h - W5100 chip implementation for Ethernet3 library
 *
 * This file provides W5100-specific implementation of the EthernetChip interface
 */

#ifndef W5100_CHIP_H
#define W5100_CHIP_H

#include <Arduino.h>
#include <SPI.h>

#include "EthernetChip.h"
#include "utility/wiznet_registers.h"

/**
 * W5100 chip implementation
 */
class W5100Chip : public EthernetChip {
   private:
    static SPISettings spi_settings;

    inline void select() { digitalWrite(cs_pin, LOW); }
    inline void deselect() { digitalWrite(cs_pin, HIGH); }

    // Primitive read/write (W5100 bus protocol differs from W5500: command byte first)
    uint8_t readReg(uint16_t addr);
    void writeReg(uint16_t addr, uint8_t data);
    void readReg(uint16_t addr, uint8_t* buf, uint16_t len);
    void writeReg(uint16_t addr, const uint8_t* buf, uint16_t len);
    uint16_t readReg16(uint16_t addr);
    void writeReg16(uint16_t addr, uint16_t value);

    // Socket register base calculation
    uint16_t sockReg(uint8_t s, uint16_t offset) {
        return (uint16_t)(W5100_S0_MR + (s * W5100_SOCK_REG_SIZE) + offset);
    }

    // Buffer helpers
    uint16_t txMemBase(uint8_t s) { return (uint16_t)(W5100_TX_MEM_BASE + (s * 0x0800)); }
    uint16_t rxMemBase(uint8_t s) { return (uint16_t)(W5100_RX_MEM_BASE + (s * 0x0800)); }
    uint16_t bufMask() const { return 0x07FF; }  // 2KB buffer mask

   public:
    explicit W5100Chip(EthernetPlatform* platform_interface, uint8_t chip_select_pin)
        : EthernetChip(platform_interface, chip_select_pin) {}

    // EthernetChip interface
    bool init() override;
    bool linkActive() override;
    uint8_t getChipType() override { return CHIP_TYPE_W5100; }
    void swReset() override;

    void setGatewayIp(const uint8_t* addr) override;
    void getGatewayIp(uint8_t* addr) override;
    void setSubnetMask(const uint8_t* addr) override;
    void getSubnetMask(uint8_t* addr) override;
    void setMACAddress(const uint8_t* addr) override;
    void getMACAddress(uint8_t* addr) override;
    void setIPAddress(const uint8_t* addr) override;
    void getIPAddress(uint8_t* addr) override;
    void setRetransmissionTime(uint16_t timeout) override;
    void setRetransmissionCount(uint8_t retry) override;

    uint8_t maxSockets() const override { return W5100_MAX_SOCK_NUM; }
    void execSocketCommand(uint8_t socket, uint8_t cmd) override;
    uint8_t readSocketStatus(uint8_t socket) override;
    uint8_t readSocketInterrupt(uint8_t socket) override;
    void writeSocketInterrupt(uint8_t socket, uint8_t flags) override;
    uint16_t getTXFreeSize(uint8_t socket) override;
    uint16_t getRXReceivedSize(uint8_t socket) override;
    void readSocketData(uint8_t socket, uint16_t srcPtr, uint8_t* dest, uint16_t len,
                        bool peek = false) override;
    void writeSocketData(uint8_t socket, const uint8_t* data, uint16_t len) override;
    void writeSocketDataOffset(uint8_t socket, uint16_t offset, const uint8_t* data,
                               uint16_t len) override;
    void advanceRX(uint8_t socket, uint16_t len) override;
    void advanceTX(uint8_t socket, uint16_t len) override;
    void commitTX(uint8_t socket) override;
    void setSocketMode(uint8_t socket, uint8_t mode) override {
        writeReg(sockReg(socket, WIZ_Sn_MR), mode);
    }
    void setSocketSourcePort(uint8_t socket, uint16_t port) override {
        writeReg(sockReg(socket, WIZ_Sn_PORT), (uint8_t)(port >> 8));
        writeReg(sockReg(socket, WIZ_Sn_PORT) + 1, (uint8_t)(port & 0xFF));
    }
    void setSocketDestination(uint8_t socket, const uint8_t* ip, uint16_t port) override {
        uint16_t dipr = sockReg(socket, WIZ_Sn_DIPR);
        writeReg(dipr + 0, ip[0]);
        writeReg(dipr + 1, ip[1]);
        writeReg(dipr + 2, ip[2]);
        writeReg(dipr + 3, ip[3]);
        uint16_t dport = sockReg(socket, WIZ_Sn_DPORT);
        writeReg(dport, (uint8_t)(port >> 8));
        writeReg(dport + 1, (uint8_t)(port & 0xFF));
    }
    uint16_t getSocketRXReadPointer(uint8_t socket) override {
        return readReg16(sockReg(socket, WIZ_Sn_RX_RD));
    }
    void setSocketRXReadPointer(uint8_t socket, uint16_t value) override {
        writeReg(sockReg(socket, WIZ_Sn_RX_RD), (uint8_t)(value >> 8));
        writeReg(sockReg(socket, WIZ_Sn_RX_RD) + 1, (uint8_t)(value & 0xFF));
    }
    uint16_t getSocketTXWritePointer(uint8_t socket) override {
        return readReg16(sockReg(socket, WIZ_Sn_TX_WR));
    }
    void setSocketTXWritePointer(uint8_t socket, uint16_t value) override {
        writeReg(sockReg(socket, WIZ_Sn_TX_WR), (uint8_t)(value >> 8));
        writeReg(sockReg(socket, WIZ_Sn_TX_WR) + 1, (uint8_t)(value & 0xFF));
    }
};

#endif  // W5100_CHIP_H
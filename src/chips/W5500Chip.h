/*
 * W5500Chip.h - W5500 chip implementation for Ethernet3 library
 *
 * This file provides W5500-specific implementation of the EthernetChip interface
 */

#ifndef W5500_CHIP_H
#define W5500_CHIP_H

#include <Arduino.h>
#include <SPI.h>

#include "EthernetChip.h"
#include "utility/wiznet_registers.h"

/**
 * W5500 chip implementation
 */
class W5500Chip : public EthernetChip {
   private:
    // SPI settings (8MHz default; can be tuned per platform)
    static SPISettings spi_settings;

    // --- Low-level SPI helpers ---
    inline void select() { digitalWrite(cs_pin, LOW); }
    inline void deselect() { digitalWrite(cs_pin, HIGH); }

    static constexpr uint8_t CB_COMMON_READ = 0x00;  // Block select bits + R/W + OM
    static constexpr uint8_t CB_COMMON_WRITE = 0x04;

    // Socket register control byte base (OR with socket<<5)
    static constexpr uint8_t CB_SOCK_READ = 0x08;
    static constexpr uint8_t CB_SOCK_WRITE = 0x0C;
    static constexpr uint8_t CB_SOCK_TXBUF_WRITE = 0x14;  // write to TX buffer
    static constexpr uint8_t CB_SOCK_RXBUF_READ = 0x18;   // read from RX buffer

    // Primitive read/write operations
    uint8_t writeReg(uint16_t addr, uint8_t control, uint8_t data);
    uint16_t writeReg(uint16_t addr, uint8_t control, const uint8_t* buf, uint16_t len);
    uint8_t readReg(uint16_t addr, uint8_t control);
    uint16_t readReg(uint16_t addr, uint8_t control, uint8_t* buf, uint16_t len);

    // Socket register helpers
    uint8_t readSn(uint8_t s, uint16_t off) {
        return readReg(off, (uint8_t)(CB_SOCK_READ | (s << 5)));
    }
    void writeSn(uint8_t s, uint16_t off, uint8_t data) {
        writeReg(off, (uint8_t)(CB_SOCK_WRITE | (s << 5)), data);
    }
    uint16_t readSn(uint8_t s, uint16_t off, uint8_t* buf, uint16_t len) {
        return readReg(off, (uint8_t)(CB_SOCK_READ | (s << 5)), buf, len);
    }
    uint16_t writeSn(uint8_t s, uint16_t off, const uint8_t* buf, uint16_t len) {
        return writeReg(off, (uint8_t)(CB_SOCK_WRITE | (s << 5)), buf, len);
    }

    // Buffer access
    void writeSocketBuffer(uint8_t s, uint16_t ptr, const uint8_t* data, uint16_t len) {
        writeReg(ptr, (uint8_t)(CB_SOCK_TXBUF_WRITE | (s << 5)), data, len);
    }
    void readSocketBuffer(uint8_t s, uint16_t ptr, uint8_t* data, uint16_t len) {
        readReg(ptr, (uint8_t)(CB_SOCK_RXBUF_READ | (s << 5)), data, len);
    }

   public:
    explicit W5500Chip(EthernetPlatform* platform_interface, uint8_t chip_select_pin)
        : EthernetChip(platform_interface, chip_select_pin) {}

    // ---- EthernetChip interface implementation ----
    bool init() override;
    bool linkActive() override;
    uint8_t getChipType() override { return CHIP_TYPE_W5500; }
    void swReset() override;

    // Network configuration
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

    void setPHYCFGR(uint8_t val) override;
    uint8_t getPHYCFGR() override;

    // Sockets
    uint8_t maxSockets() const override { return W5500_MAX_SOCK_NUM; }
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
    void setSocketMode(uint8_t socket, uint8_t mode) override { writeSn(socket, WIZ_Sn_MR, mode); }
    void setSocketSourcePort(uint8_t socket, uint16_t port) override {
        writeSn(socket, WIZ_Sn_PORT, (uint8_t)(port >> 8));
        writeSn(socket, WIZ_Sn_PORT + 1, (uint8_t)(port & 0xFF));
    }
    void setSocketDestination(uint8_t socket, const uint8_t* ip, uint16_t port) override {
        writeSn(socket, WIZ_Sn_DIPR + 0, ip[0]);
        writeSn(socket, WIZ_Sn_DIPR + 1, ip[1]);
        writeSn(socket, WIZ_Sn_DIPR + 2, ip[2]);
        writeSn(socket, WIZ_Sn_DIPR + 3, ip[3]);
        writeSn(socket, WIZ_Sn_DPORT, (uint8_t)(port >> 8));
        writeSn(socket, WIZ_Sn_DPORT + 1, (uint8_t)(port & 0xFF));
    }
    uint16_t getSocketRXReadPointer(uint8_t socket) override {
        uint8_t hi = readSn(socket, WIZ_Sn_RX_RD);
        uint8_t lo = readSn(socket, WIZ_Sn_RX_RD + 1);
        return (uint16_t(hi) << 8) | lo;
    }
    void setSocketRXReadPointer(uint8_t socket, uint16_t value) override {
        writeSn(socket, WIZ_Sn_RX_RD, (uint8_t)(value >> 8));
        writeSn(socket, WIZ_Sn_RX_RD + 1, (uint8_t)(value & 0xFF));
    }
    uint16_t getSocketTXWritePointer(uint8_t socket) override {
        uint8_t hi = readSn(socket, WIZ_Sn_TX_WR);
        uint8_t lo = readSn(socket, WIZ_Sn_TX_WR + 1);
        return (uint16_t(hi) << 8) | lo;
    }
    void setSocketTXWritePointer(uint8_t socket, uint16_t value) override {
        writeSn(socket, WIZ_Sn_TX_WR, (uint8_t)(value >> 8));
        writeSn(socket, WIZ_Sn_TX_WR + 1, (uint8_t)(value & 0xFF));
    }
};

#endif  // W5500_CHIP_H
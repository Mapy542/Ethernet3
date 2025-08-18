/*
 * W5100Chip.cpp - Implementation of W5100Chip for Ethernet3
 */

#include "W5100Chip.h"

SPISettings W5100Chip::spi_settings(14000000, MSBFIRST, SPI_MODE0);

// Low-level primitive access -------------------------------------------------
uint8_t W5100Chip::readReg(uint16_t addr) {
    SPI.beginTransaction(spi_settings);
    select();
    SPI.transfer(0x0F);  // Read opcode
    SPI.transfer((addr >> 8) & 0xFF);
    SPI.transfer(addr & 0xFF);
    uint8_t v = SPI.transfer(0);
    deselect();
    SPI.endTransaction();
    return v;
}

void W5100Chip::writeReg(uint16_t addr, uint8_t data) {
    SPI.beginTransaction(spi_settings);
    select();
    SPI.transfer(0xF0);  // Write opcode
    SPI.transfer((addr >> 8) & 0xFF);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(data);
    deselect();
    SPI.endTransaction();
}

void W5100Chip::readReg(uint16_t addr, uint8_t* buf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) buf[i] = readReg(addr + i);
}

void W5100Chip::writeReg(uint16_t addr, const uint8_t* buf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) writeReg(addr + i, buf[i]);
}

uint16_t W5100Chip::readReg16(uint16_t addr) {
    return ((uint16_t)readReg(addr) << 8) | readReg(addr + 1);
}
void W5100Chip::writeReg16(uint16_t addr, uint16_t value) {
    writeReg(addr, (uint8_t)(value >> 8));
    writeReg(addr + 1, (uint8_t)(value & 0xFF));
}

// Core init / reset ----------------------------------------------------------
bool W5100Chip::init() {
    pinMode(cs_pin, OUTPUT);
    deselect();
    SPI.begin();
    swReset();
    delay(100);  // allow chip settle
    // Allocate 2KB per socket (0x55 means 2KB each for 4 sockets)
    writeReg(W5100_TMSR, 0x55);
    writeReg(W5100_RMSR, 0x55);
    return true;
}

void W5100Chip::swReset() {
    writeReg(WIZ_MR, WIZ_MR_RST);  // WIZ_MR same as W5100_MR address 0x0000
    delay(1);
}

bool W5100Chip::linkActive() {
    // No dedicated PHY reg; simple read/write test to MR
    uint8_t test = 0x5A;
    writeReg(WIZ_MR, test);
    uint8_t rd = readReg(WIZ_MR);
    writeReg(WIZ_MR, 0x00);  // restore
    return rd == test;
}

// Network configuration ------------------------------------------------------
void W5100Chip::setGatewayIp(const uint8_t* addr) { writeReg(WIZ_GAR, addr, 4); }
void W5100Chip::getGatewayIp(uint8_t* addr) { readReg(WIZ_GAR, addr, 4); }
void W5100Chip::setSubnetMask(const uint8_t* addr) { writeReg(WIZ_SUBR, addr, 4); }
void W5100Chip::getSubnetMask(uint8_t* addr) { readReg(WIZ_SUBR, addr, 4); }
void W5100Chip::setMACAddress(const uint8_t* addr) { writeReg(WIZ_SHAR, addr, 6); }
void W5100Chip::getMACAddress(uint8_t* addr) { readReg(WIZ_SHAR, addr, 6); }
void W5100Chip::setIPAddress(const uint8_t* addr) { writeReg(WIZ_SIPR, addr, 4); }
void W5100Chip::getIPAddress(uint8_t* addr) { readReg(WIZ_SIPR, addr, 4); }
void W5100Chip::setRetransmissionTime(uint16_t timeout) { writeReg16(WIZ_RTR, timeout); }
void W5100Chip::setRetransmissionCount(uint8_t retry) { writeReg(WIZ_RCR, retry); }

// Socket helpers -------------------------------------------------------------
void W5100Chip::execSocketCommand(uint8_t socket, uint8_t cmd) {
    writeReg(sockReg(socket, WIZ_Sn_CR), cmd);
    while (readReg(sockReg(socket, WIZ_Sn_CR))) { /* wait */
    }
}

uint8_t W5100Chip::readSocketStatus(uint8_t socket) { return readReg(sockReg(socket, WIZ_Sn_SR)); }
uint8_t W5100Chip::readSocketInterrupt(uint8_t socket) {
    return readReg(sockReg(socket, WIZ_Sn_IR));
}
void W5100Chip::writeSocketInterrupt(uint8_t socket, uint8_t flags) {
    writeReg(sockReg(socket, WIZ_Sn_IR), flags);
}

uint16_t W5100Chip::getTXFreeSize(uint8_t socket) {
    uint16_t v = 0, v1 = 0;
    do {
        v1 = readReg16(sockReg(socket, WIZ_Sn_TX_FSR));
        if (v1 != 0) v = readReg16(sockReg(socket, WIZ_Sn_TX_FSR));
    } while (v != v1);
    return v;
}

uint16_t W5100Chip::getRXReceivedSize(uint8_t socket) {
    uint16_t v = 0, v1 = 0;
    do {
        v1 = readReg16(sockReg(socket, WIZ_Sn_RX_RSR));
        if (v1 != 0) v = readReg16(sockReg(socket, WIZ_Sn_RX_RSR));
    } while (v != v1);
    return v;
}

void W5100Chip::readSocketData(uint8_t socket, uint16_t srcPtr, uint8_t* dest, uint16_t len,
                               bool peek) {
    uint16_t ptr = srcPtr;
    uint16_t base = rxMemBase(socket);
    uint16_t mask = bufMask();
    for (uint16_t i = 0; i < len; i++) dest[i] = readReg(base + (ptr++ & mask));
    if (!peek) {
        uint16_t rd = readReg16(sockReg(socket, WIZ_Sn_RX_RD));
        rd += len;
        writeReg16(sockReg(socket, WIZ_Sn_RX_RD), rd);
    }
}

void W5100Chip::writeSocketData(uint8_t socket, const uint8_t* data, uint16_t len) {
    uint16_t ptr = readReg16(sockReg(socket, WIZ_Sn_TX_WR));
    uint16_t base = txMemBase(socket);
    uint16_t mask = bufMask();
    for (uint16_t i = 0; i < len; i++) writeReg(base + (ptr++ & mask), data[i]);
    writeReg16(sockReg(socket, WIZ_Sn_TX_WR), ptr);
}

void W5100Chip::writeSocketDataOffset(uint8_t socket, uint16_t offset, const uint8_t* data,
                                      uint16_t len) {
    uint16_t ptr = readReg16(sockReg(socket, WIZ_Sn_TX_WR));
    uint16_t base = txMemBase(socket);
    uint16_t mask = bufMask();
    ptr += offset;
    for (uint16_t i = 0; i < len; i++) writeReg(base + (ptr++ & mask), data[i]);
    // Caller will advance via advanceTX
}

void W5100Chip::advanceRX(uint8_t socket, uint16_t len) {
    uint16_t rd = readReg16(sockReg(socket, WIZ_Sn_RX_RD));
    rd += len;
    writeReg16(sockReg(socket, WIZ_Sn_RX_RD), rd);
}

void W5100Chip::advanceTX(uint8_t socket, uint16_t len) {
    uint16_t wr = readReg16(sockReg(socket, WIZ_Sn_TX_WR));
    wr += len;
    writeReg16(sockReg(socket, WIZ_Sn_TX_WR), wr);
}

void W5100Chip::commitTX(uint8_t socket) { execSocketCommand(socket, Sock_SEND); }

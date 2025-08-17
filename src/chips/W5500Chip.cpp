/*
 * W5500Chip.cpp - W5500 chip implementation for Ethernet3 library
 *
 * This file provides W5500-specific implementation with integrated functionality
 * formerly provided by utility/w5500.h/cpp
 */

#include "W5500Chip.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>

// SPI settings for W5500 (8MHz, MSBFIRST, MODE0)
SPISettings W5500Chip::spi_settings(8000000, MSBFIRST, SPI_MODE0);

// Constructor
W5500Chip::W5500Chip(EthernetPlatform* platform_interface, uint8_t chip_select_pin)
    : EthernetChip(platform_interface, chip_select_pin) {
}

// ==== Core EthernetChip Interface ====

bool W5500Chip::init() {
    if (!platform) return false;
    
    // Initialize platform
    platform->init();
    
    // Configure CS pin
    platform->pinMode(cs_pin, OUTPUT);
    resetSS();
    
    // Initialize SPI
    SPI.begin();
    
    // Reset chip
    swReset();
    platform->delay(1000);
    
    // Configure socket buffer sizes (2KB each for TX/RX)
    for (int i = 0; i < W5500_MAX_SOCK_NUM; i++) {
        uint8_t cntl_byte = (0x0C + (i << 5));
        write(0x1E, cntl_byte, 2);  // Sn_RXBUF_SIZE = 2KB
        write(0x1F, cntl_byte, 2);  // Sn_TXBUF_SIZE = 2KB
    }
    
    return true;
}

bool W5500Chip::linkActive() {
    // Check physical link status via PHY configuration register
    // PHYCFGR bit 0 (LNK) indicates link status: 1 = link up, 0 = link down
    uint8_t phy_cfg = getPHYCFGR();
    return (phy_cfg & 0x01) != 0;
}

void W5500Chip::swReset() {
    // Write reset bit to Mode Register
    writeRegister(0x0000, 0x80);
    platform->delay(1);
}

// ==== Low-level SPI Interface ====

void W5500Chip::setSS() {
    platform->digitalWrite(cs_pin, LOW);
}

void W5500Chip::resetSS() {
    platform->digitalWrite(cs_pin, HIGH);
}

uint8_t W5500Chip::read(uint16_t addr, uint8_t cb) {
    SPI.beginTransaction(spi_settings);
    setSS();
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(cb);
    uint8_t data = SPI.transfer(0);
    resetSS();
    SPI.endTransaction();
    return data;
}

uint16_t W5500Chip::read(uint16_t addr, uint8_t cb, uint8_t* buf, uint16_t len) {
    SPI.beginTransaction(spi_settings);
    setSS();
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(cb);
    for (uint16_t i = 0; i < len; i++) {
        buf[i] = SPI.transfer(0);
    }
    resetSS();
    SPI.endTransaction();
    return len;
}

uint8_t W5500Chip::write(uint16_t addr, uint8_t cb, uint8_t data) {
    SPI.beginTransaction(spi_settings);
    setSS();
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(cb);
    SPI.transfer(data);
    resetSS();
    SPI.endTransaction();
    return 1;
}

uint16_t W5500Chip::write(uint16_t addr, uint8_t cb, const uint8_t* buf, uint16_t len) {
    SPI.beginTransaction(spi_settings);
    setSS();
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(cb);
    for (uint16_t i = 0; i < len; i++) {
        SPI.transfer(buf[i]);
    }
    resetSS();
    SPI.endTransaction();
    return len;
}

// ==== Socket Register Access Helpers ====

uint8_t W5500Chip::readSocketReg(uint8_t socket, uint16_t addr) {
    uint8_t cntl_byte = (socket << 5) + 0x08;
    return read(addr, cntl_byte);
}

uint8_t W5500Chip::writeSocketReg(uint8_t socket, uint16_t addr, uint8_t data) {
    uint8_t cntl_byte = (socket << 5) + 0x0C;
    return write(addr, cntl_byte, data);
}

uint16_t W5500Chip::readSocketReg(uint8_t socket, uint16_t addr, uint8_t* buf, uint16_t len) {
    uint8_t cntl_byte = (socket << 5) + 0x08;
    return read(addr, cntl_byte, buf, len);
}

uint16_t W5500Chip::writeSocketReg(uint8_t socket, uint16_t addr, const uint8_t* buf, uint16_t len) {
    uint8_t cntl_byte = (socket << 5) + 0x0C;
    return write(addr, cntl_byte, buf, len);
}

// ==== Common Register Interface ====

uint8_t W5500Chip::readRegister(uint16_t addr) {
    return read(addr, 0x00);  // Common register read
}

void W5500Chip::writeRegister(uint16_t addr, uint8_t data) {
    write(addr, 0x04, data);  // Common register write
}

uint16_t W5500Chip::readRegister(uint16_t addr, uint8_t* buf, uint16_t len) {
    return read(addr, 0x00, buf, len);
}

uint16_t W5500Chip::writeRegister(uint16_t addr, const uint8_t* buf, uint16_t len) {
    return write(addr, 0x04, buf, len);
}

// ==== Socket-specific Register Interface ====

uint8_t W5500Chip::readSocketRegister(uint8_t socket, uint16_t addr) {
    return readSocketReg(socket, addr);
}

void W5500Chip::writeSocketRegister(uint8_t socket, uint16_t addr, uint8_t data) {
    writeSocketReg(socket, addr, data);
}

uint16_t W5500Chip::readSocketRegister(uint8_t socket, uint16_t addr, uint8_t* buf, uint16_t len) {
    return readSocketReg(socket, addr, buf, len);
}

uint16_t W5500Chip::writeSocketRegister(uint8_t socket, uint16_t addr, const uint8_t* buf, uint16_t len) {
    return writeSocketReg(socket, addr, buf, len);
}

// ==== Network Configuration Interface ====

void W5500Chip::setMACAddress(const uint8_t* mac) {
    writeRegister(0x0009, mac, 6);  // SHAR
}

void W5500Chip::getMACAddress(uint8_t* mac) {
    readRegister(0x0009, mac, 6);  // SHAR
}

void W5500Chip::setIPAddress(const uint8_t* ip) {
    writeRegister(0x000F, ip, 4);  // SIPR
}

void W5500Chip::getIPAddress(uint8_t* ip) {
    readRegister(0x000F, ip, 4);  // SIPR
}

void W5500Chip::setGatewayIP(const uint8_t* gateway) {
    writeRegister(0x0001, gateway, 4);  // GAR
}

void W5500Chip::getGatewayIP(uint8_t* gateway) {
    readRegister(0x0001, gateway, 4);  // GAR
}

void W5500Chip::setSubnetMask(const uint8_t* subnet) {
    writeRegister(0x0005, subnet, 4);  // SUBR
}

void W5500Chip::getSubnetMask(uint8_t* subnet) {
    readRegister(0x0005, subnet, 4);  // SUBR
}

// ==== Socket Operations Interface ====

void W5500Chip::executeSocketCommand(uint8_t socket, uint8_t cmd) {
    writeSocketRegister(socket, 0x0001, cmd);  // Sn_CR
}

uint16_t W5500Chip::getTXFreeSize(uint8_t socket) {
    uint16_t val = 0, val1 = 0;
    do {
        val1 = (readSocketReg(socket, 0x0020) << 8) + readSocketReg(socket, 0x0021);  // Sn_TX_FSR
        if (val1 != 0) {
            val = (readSocketReg(socket, 0x0020) << 8) + readSocketReg(socket, 0x0021);
        }
    } while (val != val1);
    return val;
}

uint16_t W5500Chip::getRXReceivedSize(uint8_t socket) {
    uint16_t val = 0, val1 = 0;
    do {
        val1 = (readSocketReg(socket, 0x0026) << 8) + readSocketReg(socket, 0x0027);  // Sn_RX_RSR
        if (val1 != 0) {
            val = (readSocketReg(socket, 0x0026) << 8) + readSocketReg(socket, 0x0027);
        }
    } while (val != val1);
    return val;
}

void W5500Chip::sendDataProcessing(uint8_t socket, const uint8_t* data, uint16_t len) {
    // This is same as having no offset in a call to sendDataProcessingOffset
    sendDataProcessingOffset(socket, 0, data, len);
}

void W5500Chip::receiveDataProcessing(uint8_t socket, uint8_t* data, uint16_t len, bool peek) {
    uint16_t ptr = (readSocketReg(socket, 0x0028) << 8) + readSocketReg(socket, 0x0029);  // Sn_RX_RD
    
    readData(socket, ptr, data, len);
    
    if (!peek) {
        ptr += len;
        writeSocketReg(socket, 0x0028, ptr >> 8);      // Sn_RX_RD high
        writeSocketReg(socket, 0x0029, ptr & 0xFF);    // Sn_RX_RD low
    }
}

// ==== W5500-specific Methods ====

void W5500Chip::setRetransmissionTime(uint16_t timeout) {
    writeRegister(0x0019, timeout >> 8);    // RTR high
    writeRegister(0x001A, timeout & 0xFF);  // RTR low
}

void W5500Chip::setRetransmissionCount(uint8_t retry) {
    writeRegister(0x001B, retry);  // RCR
}

void W5500Chip::setPHYCFGR(uint8_t val) {
    writeRegister(0x002E, val);  // PHYCFGR
}

uint8_t W5500Chip::getPHYCFGR() {
    return readRegister(0x002E);  // PHYCFGR
}

void W5500Chip::readData(uint8_t socket, uint16_t src, uint8_t* dst, uint16_t len) {
    uint8_t cntl_byte = (0x18 + (socket << 5));  // Socket RX buffer read
    read(src, cntl_byte, dst, len);
}

void W5500Chip::sendDataProcessingOffset(uint8_t socket, uint16_t data_offset, const uint8_t* data, uint16_t len) {
    uint16_t ptr = (readSocketReg(socket, 0x0024) << 8) + readSocketReg(socket, 0x0025);  // Sn_TX_WR
    uint8_t cntl_byte = (0x14 + (socket << 5));  // Socket TX buffer write
    
    ptr += data_offset;
    write(ptr, cntl_byte, data, len);
    ptr += len;
    
    writeSocketReg(socket, 0x0024, ptr >> 8);      // Sn_TX_WR high
    writeSocketReg(socket, 0x0025, ptr & 0xFF);    // Sn_TX_WR low
}

#endif  // ARDUINO
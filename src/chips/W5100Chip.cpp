/*
 * W5100Chip.cpp - W5100 chip implementation for Ethernet3 library
 *
 * This file provides W5100-specific implementation with integrated functionality
 * formerly provided by utility/w5100.h/cpp
 */

#include "W5100Chip.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>

// SPI settings for W5100 (14MHz, MSBFIRST, MODE0)
SPISettings W5100Chip::spi_settings(14000000, MSBFIRST, SPI_MODE0);

// Constructor
W5100Chip::W5100Chip(EthernetPlatform* platform_interface, uint8_t chip_select_pin)
    : EthernetChip(platform_interface, chip_select_pin) {
}

// ==== Core EthernetChip Interface ====

bool W5100Chip::init() {
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
    
    // Configure memory allocation for sockets (2KB each for TX/RX)
    // W5100 has 8KB total TX memory and 8KB total RX memory
    // Split equally among 4 sockets = 2KB each
    writeRegister(W5100_TMSR, 0x55);  // 2KB for each socket TX
    writeRegister(W5100_RMSR, 0x55);  // 2KB for each socket RX
    
    return true;
}

bool W5100Chip::linkActive() {
    // W5100 doesn't have a dedicated PHY status register
    // Check if we can read/write to a known register as a basic test
    uint8_t test_val = 0xA5;
    writeRegister(W5100_MR, test_val);
    uint8_t read_val = readRegister(W5100_MR);
    
    // Reset to normal mode
    writeRegister(W5100_MR, 0x00);
    
    // If we can read back what we wrote, assume link is active
    return (read_val == test_val);
}

void W5100Chip::swReset() {
    writeRegister(W5100_MR, W5100_MR_RST);
    platform->delay(1);
}

// ==== Low-level SPI Interface ====

void W5100Chip::setSS() {
    platform->digitalWrite(cs_pin, LOW);
}

void W5100Chip::resetSS() {
    platform->digitalWrite(cs_pin, HIGH);
}

// ==== Common Register Interface ====

uint8_t W5100Chip::readRegister(uint16_t addr) {
    SPI.beginTransaction(spi_settings);
    setSS();
    
    SPI.transfer(0x0F);  // Read command
    SPI.transfer((addr >> 8) & 0xFF);
    SPI.transfer(addr & 0xFF);
    uint8_t data = SPI.transfer(0);
    
    resetSS();
    SPI.endTransaction();
    return data;
}

void W5100Chip::writeRegister(uint16_t addr, uint8_t data) {
    SPI.beginTransaction(spi_settings);
    setSS();
    
    SPI.transfer(0xF0);  // Write command
    SPI.transfer((addr >> 8) & 0xFF);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(data);
    
    resetSS();
    SPI.endTransaction();
}

uint16_t W5100Chip::readRegister(uint16_t addr, uint8_t* buf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        buf[i] = readRegister(addr + i);
    }
    return len;
}

uint16_t W5100Chip::writeRegister(uint16_t addr, const uint8_t* buf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        writeRegister(addr + i, buf[i]);
    }
    return len;
}

// ==== Socket-specific Register Interface ====

uint8_t W5100Chip::readSocketRegister(uint8_t socket, uint16_t addr) {
    // Calculate socket register address
    uint16_t socket_addr = W5100_S0_MR + (socket * W5100_SOCK_REG_SIZE) + addr;
    return readRegister(socket_addr);
}

void W5100Chip::writeSocketRegister(uint8_t socket, uint16_t addr, uint8_t data) {
    // Calculate socket register address
    uint16_t socket_addr = W5100_S0_MR + (socket * W5100_SOCK_REG_SIZE) + addr;
    writeRegister(socket_addr, data);
}

uint16_t W5100Chip::readSocketRegister(uint8_t socket, uint16_t addr, uint8_t* buf, uint16_t len) {
    uint16_t socket_addr = W5100_S0_MR + (socket * W5100_SOCK_REG_SIZE) + addr;
    return readRegister(socket_addr, buf, len);
}

uint16_t W5100Chip::writeSocketRegister(uint8_t socket, uint16_t addr, const uint8_t* buf, uint16_t len) {
    uint16_t socket_addr = W5100_S0_MR + (socket * W5100_SOCK_REG_SIZE) + addr;
    return writeRegister(socket_addr, buf, len);
}

// ==== Network Configuration Interface ====

void W5100Chip::setMACAddress(const uint8_t* mac) {
    writeRegister(W5100_SHAR, mac, 6);
}

void W5100Chip::getMACAddress(uint8_t* mac) {
    readRegister(W5100_SHAR, mac, 6);
}

void W5100Chip::setIPAddress(const uint8_t* ip) {
    writeRegister(W5100_SIPR, ip, 4);
}

void W5100Chip::getIPAddress(uint8_t* ip) {
    readRegister(W5100_SIPR, ip, 4);
}

void W5100Chip::setGatewayIP(const uint8_t* gateway) {
    writeRegister(W5100_GAR, gateway, 4);
}

void W5100Chip::getGatewayIP(uint8_t* gateway) {
    readRegister(W5100_GAR, gateway, 4);
}

void W5100Chip::setSubnetMask(const uint8_t* subnet) {
    writeRegister(W5100_SUBR, subnet, 4);
}

void W5100Chip::getSubnetMask(uint8_t* subnet) {
    readRegister(W5100_SUBR, subnet, 4);
}

// ==== Socket Operations Interface ====

void W5100Chip::executeSocketCommand(uint8_t socket, uint8_t cmd) {
    writeSocketRegister(socket, 0x0001, cmd);  // Sn_CR offset from socket base
}

uint16_t W5100Chip::getTXFreeSize(uint8_t socket) {
    uint16_t offset = (W5100_S0_TX_FSR - W5100_S0_MR);  // Offset from socket base
    uint16_t val = (readSocketRegister(socket, offset) << 8) + 
                   readSocketRegister(socket, offset + 1);
    return val;
}

uint16_t W5100Chip::getRXReceivedSize(uint8_t socket) {
    uint16_t offset = (W5100_S0_RX_RSR - W5100_S0_MR);  // Offset from socket base
    uint16_t val = (readSocketRegister(socket, offset) << 8) + 
                   readSocketRegister(socket, offset + 1);
    return val;
}

void W5100Chip::sendDataProcessing(uint8_t socket, const uint8_t* data, uint16_t len) {
    uint16_t offset = (W5100_S0_TX_WR - W5100_S0_MR);  // Offset from socket base
    
    // Get current write pointer
    uint16_t ptr = (readSocketRegister(socket, offset) << 8) + 
                   readSocketRegister(socket, offset + 1);
    
    // Write data to TX buffer
    writeData(socket, ptr, data, len);
    
    // Update write pointer
    ptr += len;
    writeSocketRegister(socket, offset, ptr >> 8);      // TX_WR high
    writeSocketRegister(socket, offset + 1, ptr & 0xFF); // TX_WR low
}

void W5100Chip::receiveDataProcessing(uint8_t socket, uint8_t* data, uint16_t len, bool peek) {
    uint16_t offset = (W5100_S0_RX_RD - W5100_S0_MR);  // Offset from socket base
    
    // Get current read pointer
    uint16_t ptr = (readSocketRegister(socket, offset) << 8) + 
                   readSocketRegister(socket, offset + 1);
    
    // Read data from RX buffer
    readData(socket, ptr, data, len);
    
    if (!peek) {
        // Update read pointer
        ptr += len;
        writeSocketRegister(socket, offset, ptr >> 8);      // RX_RD high
        writeSocketRegister(socket, offset + 1, ptr & 0xFF); // RX_RD low
    }
}

// ==== W5100-specific Methods ====

void W5100Chip::setRetransmissionTime(uint16_t timeout) {
    writeRegister(W5100_RTR, timeout >> 8);    // RTR high
    writeRegister(W5100_RTR + 1, timeout & 0xFF);  // RTR low
}

void W5100Chip::setRetransmissionCount(uint8_t retry) {
    writeRegister(W5100_RCR, retry);
}

void W5100Chip::readData(uint8_t socket, uint16_t src, uint8_t* dst, uint16_t len) {
    // Calculate physical address in RX memory
    // W5100 has 2KB per socket for RX buffer
    uint16_t base_addr = W5100_RX_MEM_BASE + (socket * RSIZE);
    uint16_t mask = RSIZE - 1;  // 2KB - 1 = 0x7FF
    
    for (uint16_t i = 0; i < len; i++) {
        uint16_t addr = base_addr + ((src + i) & mask);
        dst[i] = readRegister(addr);
    }
}

void W5100Chip::writeData(uint8_t socket, uint16_t dst, const uint8_t* src, uint16_t len) {
    // Calculate physical address in TX memory
    // W5100 has 2KB per socket for TX buffer
    uint16_t base_addr = W5100_TX_MEM_BASE + (socket * SSIZE);
    uint16_t mask = SSIZE - 1;  // 2KB - 1 = 0x7FF
    
    for (uint16_t i = 0; i < len; i++) {
        uint16_t addr = base_addr + ((dst + i) & mask);
        writeRegister(addr, src[i]);
    }
}

#endif  // ARDUINO
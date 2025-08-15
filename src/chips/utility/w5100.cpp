/*
 * w5100.cpp - W5100 Ethernet chip driver implementation for Ethernet3 library
 */

#include "w5100.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>

// Global instance
W5100Class w5100;

// Static member initialization
uint8_t W5100Class::chip_cs_pin = 10;

// SPI settings for W5100
SPISettings w5100_spi_settings(14000000, MSBFIRST, SPI_MODE0);

void W5100Class::setSS(void) {
    digitalWrite(chip_cs_pin, LOW);
}

void W5100Class::resetSS(void) {
    digitalWrite(chip_cs_pin, HIGH);
}

void W5100Class::init(uint8_t cs_pin) {
    chip_cs_pin = cs_pin;
    pinMode(cs_pin, OUTPUT);
    resetSS();
    
    SPI.begin();
    
    // Reset the chip
    swReset();
    delay(1000);
    
    // Configure memory allocation for sockets (2KB each for TX/RX)
    // W5100 has 8KB total TX memory and 8KB total RX memory
    // Split equally among 4 sockets = 2KB each
    write(W5100_TMSR, 0x55);  // 2KB for each socket TX
    write(W5100_RMSR, 0x55);  // 2KB for each socket RX
}

void W5100Class::swReset() {
    write(W5100_MR, W5100_MR_RST);
    delay(1);
}

bool W5100Class::linkActive() {
    // W5100 doesn't have a dedicated PHY status register
    // Check if we can read/write to a known register as a basic test
    uint8_t test_val = 0xA5;
    write(W5100_MR, test_val);
    uint8_t read_val = read(W5100_MR);
    
    // Reset to normal mode
    write(W5100_MR, 0x00);
    
    // If we can read back what we wrote, assume link is active
    return (read_val == test_val);
}

uint8_t W5100Class::read(uint16_t addr) {
    SPI.beginTransaction(w5100_spi_settings);
    setSS();
    
    SPI.transfer(0x0F);  // Read command
    SPI.transfer((addr >> 8) & 0xFF);
    SPI.transfer(addr & 0xFF);
    uint8_t data = SPI.transfer(0);
    
    resetSS();
    SPI.endTransaction();
    return data;
}

uint16_t W5100Class::read(uint16_t addr, uint8_t *buf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        buf[i] = read(addr + i);
    }
    return len;
}

void W5100Class::write(uint16_t addr, uint8_t data) {
    SPI.beginTransaction(w5100_spi_settings);
    setSS();
    
    SPI.transfer(0xF0);  // Write command
    SPI.transfer((addr >> 8) & 0xFF);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(data);
    
    resetSS();
    SPI.endTransaction();
}

uint16_t W5100Class::write(uint16_t addr, const uint8_t *buf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        write(addr + i, buf[i]);
    }
    return len;
}

uint16_t W5100Class::read16(uint16_t addr) {
    uint16_t val = read(addr) << 8;
    val |= read(addr + 1);
    return val;
}

void W5100Class::write16(uint16_t addr, uint16_t data) {
    write(addr, (data >> 8) & 0xFF);
    write(addr + 1, data & 0xFF);
}

// Common register access
void W5100Class::setGatewayIp(uint8_t *addr) {
    write(W5100_GAR, addr, 4);
}

void W5100Class::getGatewayIp(uint8_t *addr) {
    read(W5100_GAR, addr, 4);
}

void W5100Class::setSubnetMask(uint8_t *addr) {
    write(W5100_SUBR, addr, 4);
}

void W5100Class::getSubnetMask(uint8_t *addr) {
    read(W5100_SUBR, addr, 4);
}

void W5100Class::setMACAddress(uint8_t *addr) {
    write(W5100_SHAR, addr, 6);
}

void W5100Class::getMACAddress(uint8_t *addr) {
    read(W5100_SHAR, addr, 6);
}

void W5100Class::setIPAddress(uint8_t *addr) {
    write(W5100_SIPR, addr, 4);
}

void W5100Class::getIPAddress(uint8_t *addr) {
    read(W5100_SIPR, addr, 4);
}

void W5100Class::setRetransmissionTime(uint16_t timeout) {
    write16(W5100_RTR, timeout);
}

void W5100Class::setRetransmissionCount(uint8_t retry) {
    write(W5100_RCR, retry);
}

// Socket register access helpers
uint16_t W5100Class::getSocketRegAddr(SOCKET s, uint16_t reg_offset) {
    return W5100_S0_MR + (s * W5100_SOCK_REG_SIZE) + reg_offset;
}

// Socket-specific functions
uint8_t W5100Class::readSnMR(SOCKET s) {
    return read(getSocketRegAddr(s, 0));  // MR is at offset 0
}

void W5100Class::writeSnMR(SOCKET s, uint8_t data) {
    write(getSocketRegAddr(s, 0), data);
}

uint8_t W5100Class::readSnSR(SOCKET s) {
    return read(getSocketRegAddr(s, 3));  // SR is at offset 3
}

uint8_t W5100Class::readSnIR(SOCKET s) {
    return read(getSocketRegAddr(s, 2));  // IR is at offset 2
}

void W5100Class::writeSnIR(SOCKET s, uint8_t data) {
    write(getSocketRegAddr(s, 2), data);
}

uint16_t W5100Class::readSnPORT(SOCKET s) {
    return read16(getSocketRegAddr(s, 4));  // PORT is at offset 4
}

void W5100Class::writeSnPORT(SOCKET s, uint16_t port) {
    write16(getSocketRegAddr(s, 4), port);
}

void W5100Class::writeSnCR(SOCKET s, uint8_t cmd) {
    write(getSocketRegAddr(s, 1), cmd);  // CR is at offset 1
}

uint16_t W5100Class::readSnTX_FSR(SOCKET s) {
    return read16(getSocketRegAddr(s, 0x20));  // TX_FSR offset
}

uint16_t W5100Class::readSnRX_RSR(SOCKET s) {
    return read16(getSocketRegAddr(s, 0x26));  // RX_RSR offset
}

uint16_t W5100Class::readSnTX_WR(SOCKET s) {
    return read16(getSocketRegAddr(s, 0x24));  // TX_WR offset
}

void W5100Class::writeSnTX_WR(SOCKET s, uint16_t ptr) {
    write16(getSocketRegAddr(s, 0x24), ptr);
}

uint16_t W5100Class::readSnRX_RD(SOCKET s) {
    return read16(getSocketRegAddr(s, 0x28));  // RX_RD offset
}

void W5100Class::writeSnRX_RD(SOCKET s, uint16_t ptr) {
    write16(getSocketRegAddr(s, 0x28), ptr);
}

// Buffer address calculations
uint16_t W5100Class::getTXBufferAddr(SOCKET s) {
    return W5100_TX_MEM_BASE + (s * 0x0800);  // 2KB per socket
}

uint16_t W5100Class::getRXBufferAddr(SOCKET s) {
    return W5100_RX_MEM_BASE + (s * 0x0800);  // 2KB per socket
}

uint16_t W5100Class::getBufferMask(SOCKET s) {
    return 0x07FF;  // 2KB - 1
}

// Memory management functions
void W5100Class::read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len) {
    uint16_t ptr = src;
    uint16_t rx_base = getRXBufferAddr(s);
    uint16_t mask = getBufferMask(s);
    
    for (uint16_t i = 0; i < len; i++) {
        dst[i] = read(rx_base + (ptr & mask));
        ptr++;
    }
}

void W5100Class::send_data_processing(SOCKET s, const uint8_t *data, uint16_t len) {
    uint16_t ptr = readSnTX_WR(s);
    uint16_t tx_base = getTXBufferAddr(s);
    uint16_t mask = getBufferMask(s);
    
    for (uint16_t i = 0; i < len; i++) {
        write(tx_base + (ptr & mask), data[i]);
        ptr++;
    }
    
    writeSnTX_WR(s, ptr);
}

void W5100Class::recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek) {
    uint16_t ptr = readSnRX_RD(s);
    read_data(s, ptr, data, len);
    
    if (!peek) {
        writeSnRX_RD(s, ptr + len);
    }
}

void W5100Class::execCmdSn(SOCKET s, uint8_t cmd) {
    writeSnCR(s, cmd);
    while (read(getSocketRegAddr(s, 1))) {
        ; // Wait for command to complete
    }
}

uint16_t W5100Class::getTXFreeSize(SOCKET s) {
    uint16_t val = 0, val1 = 0;
    do {
        val1 = readSnTX_FSR(s);
        if (val1 != 0) {
            val = readSnTX_FSR(s);
        }
    } while (val != val1);
    return val;
}

uint16_t W5100Class::getRXReceivedSize(SOCKET s) {
    uint16_t val = 0, val1 = 0;
    do {
        val1 = readSnRX_RSR(s);
        if (val1 != 0) {
            val = readSnRX_RSR(s);
        }
    } while (val != val1);
    return val;
}

#endif // ARDUINO
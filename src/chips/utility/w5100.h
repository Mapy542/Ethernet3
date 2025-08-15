/*
 * w5100.h - W5100 Ethernet chip driver for Ethernet3 library
 * 
 * This file provides W5100-specific functionality similar to w5500.h
 */

#ifndef W5100_H
#define W5100_H

#include <stdint.h>
#include "utility/w5100_registers.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#endif

typedef uint8_t SOCKET;

/**
 * W5100 chip driver class
 * Provides low-level register access and chip-specific functionality
 */
class W5100Class {
private:
    static uint8_t chip_cs_pin;
    
    // Low-level register access
    static void setSS(void);
    static void resetSS(void);
    
public:
    /**
     * Initialize W5100 chip
     * @param cs_pin Chip select pin number
     */
    void init(uint8_t cs_pin = 10);
    
    /**
     * Software reset
     */
    void swReset();
    
    /**
     * Check if link is active by reading MR register
     * Note: W5100 doesn't have a dedicated PHY status register like W5500
     * This is a basic implementation that checks if the chip responds
     */
    bool linkActive();
    
    // Register access functions
    static uint8_t read(uint16_t addr);
    static uint16_t read(uint16_t addr, uint8_t *buf, uint16_t len);
    static void write(uint16_t addr, uint8_t data);
    static uint16_t write(uint16_t addr, const uint8_t *buf, uint16_t len);
    
    // 16-bit register access
    static uint16_t read16(uint16_t addr);
    static void write16(uint16_t addr, uint16_t data);
    
    // Common register access methods
    void setGatewayIp(uint8_t *addr);
    void getGatewayIp(uint8_t *addr);
    
    void setSubnetMask(uint8_t *addr);
    void getSubnetMask(uint8_t *addr);
    
    void setMACAddress(uint8_t *addr);
    void getMACAddress(uint8_t *addr);
    
    void setIPAddress(uint8_t *addr);
    void getIPAddress(uint8_t *addr);
    
    void setRetransmissionTime(uint16_t timeout);
    void setRetransmissionCount(uint8_t retry);
    
    // Socket-specific functions
    uint8_t readSnMR(SOCKET s);
    void writeSnMR(SOCKET s, uint8_t data);
    
    uint8_t readSnSR(SOCKET s);
    uint8_t readSnIR(SOCKET s);
    void writeSnIR(SOCKET s, uint8_t data);
    
    uint16_t readSnPORT(SOCKET s);
    void writeSnPORT(SOCKET s, uint16_t port);
    
    void writeSnCR(SOCKET s, uint8_t cmd);
    
    uint16_t readSnTX_FSR(SOCKET s);
    uint16_t readSnRX_RSR(SOCKET s);
    
    uint16_t readSnTX_WR(SOCKET s);
    void writeSnTX_WR(SOCKET s, uint16_t ptr);
    
    uint16_t readSnRX_RD(SOCKET s);
    void writeSnRX_RD(SOCKET s, uint16_t ptr);
    
    // Memory access functions for socket buffers
    void read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len);
    void send_data_processing(SOCKET s, const uint8_t *data, uint16_t len);
    void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek = 0);
    
    // Command execution
    void execCmdSn(SOCKET s, uint8_t cmd);
    
    // Memory size functions  
    uint16_t getTXFreeSize(SOCKET s);
    uint16_t getRXReceivedSize(SOCKET s);
    
private:
    // Calculate socket register address
    static uint16_t getSocketRegAddr(SOCKET s, uint16_t reg_offset);
    
    // Calculate socket buffer addresses
    static uint16_t getTXBufferAddr(SOCKET s);
    static uint16_t getRXBufferAddr(SOCKET s);
    static uint16_t getBufferMask(SOCKET s);
};

// Global instance for backward compatibility
extern W5100Class w5100;

#endif // W5100_H
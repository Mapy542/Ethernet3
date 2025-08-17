/*
 * W5500Chip.h - W5500 chip implementation for Ethernet3 library
 *
 * This file provides W5500-specific implementation of the EthernetChip interface
 * with integrated register access and networking functionality.
 */

#ifndef W5500_CHIP_H
#define W5500_CHIP_H

#include "EthernetChip.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#endif

// W5500 Constants
#define W5500_MAX_SOCK_NUM 8

// Socket Mode Register (Sn_MR) values
#define W5500_SnMR_CLOSE  0x00  // Closed
#define W5500_SnMR_TCP    0x01  // TCP mode
#define W5500_SnMR_UDP    0x02  // UDP mode  
#define W5500_SnMR_IPRAW  0x03  // IP Raw mode
#define W5500_SnMR_MACRAW 0x04  // MAC Raw mode
#define W5500_SnMR_PPPOE  0x05  // PPPoE mode
#define W5500_SnMR_ND     0x20  // No Delayed ACK
#define W5500_SnMR_MULTI  0x80  // Multicast

// Socket Command Register (Sn_CR)
#define W5500_SnCR_OPEN      0x01  // Open socket
#define W5500_SnCR_LISTEN    0x02  // Listen
#define W5500_SnCR_CONNECT   0x04  // Connect
#define W5500_SnCR_DISCON    0x08  // Disconnect
#define W5500_SnCR_CLOSE     0x10  // Close
#define W5500_SnCR_SEND      0x20  // Send
#define W5500_SnCR_SEND_MAC  0x21  // Send MAC
#define W5500_SnCR_SEND_KEEP 0x22  // Send Keep
#define W5500_SnCR_RECV      0x40  // Receive

// Socket Status Register (Sn_SR) values
#define W5500_SnSR_CLOSED      0x00
#define W5500_SnSR_INIT        0x13
#define W5500_SnSR_LISTEN      0x14
#define W5500_SnSR_SYNSENT     0x15
#define W5500_SnSR_SYNRECV     0x16
#define W5500_SnSR_ESTABLISHED 0x17
#define W5500_SnSR_FIN_WAIT    0x18
#define W5500_SnSR_CLOSING     0x1A
#define W5500_SnSR_TIME_WAIT   0x1B
#define W5500_SnSR_CLOSE_WAIT  0x1C
#define W5500_SnSR_LAST_ACK    0x1D
#define W5500_SnSR_UDP         0x22
#define W5500_SnSR_IPRAW       0x32
#define W5500_SnSR_MACRAW      0x42
#define W5500_SnSR_PPPOE       0x5F

// Socket Interrupt Register (Sn_IR)
#define W5500_SnIR_SEND_OK 0x10
#define W5500_SnIR_TIMEOUT 0x08
#define W5500_SnIR_RECV    0x04
#define W5500_SnIR_DISCON  0x02
#define W5500_SnIR_CON     0x01

/**
 * W5500 chip implementation with integrated functionality
 */
class W5500Chip : public EthernetChip {
   private:
    // SPI settings for W5500
    static SPISettings spi_settings;

    // Low-level SPI access
    void setSS();
    void resetSS();
    
    // W5500-specific register access
    uint8_t read(uint16_t addr, uint8_t cb);
    uint16_t read(uint16_t addr, uint8_t cb, uint8_t* buf, uint16_t len);
    uint8_t write(uint16_t addr, uint8_t cb, uint8_t data);
    uint16_t write(uint16_t addr, uint8_t cb, const uint8_t* buf, uint16_t len);

    // Socket register access helpers
    uint8_t readSocketReg(uint8_t socket, uint16_t addr);
    uint8_t writeSocketReg(uint8_t socket, uint16_t addr, uint8_t data);
    uint16_t readSocketReg(uint8_t socket, uint16_t addr, uint8_t* buf, uint16_t len);
    uint16_t writeSocketReg(uint8_t socket, uint16_t addr, const uint8_t* buf, uint16_t len);

   public:
    /**
     * Constructor
     * @param platform_interface Pointer to unified platform implementation
     * @param chip_select_pin Chip select pin number
     */
    W5500Chip(EthernetPlatform* platform_interface, uint8_t chip_select_pin);

    virtual ~W5500Chip() {}

    // ==== Core EthernetChip Interface ====
    bool init() override;
    bool linkActive() override;
    uint8_t getChipType() override { return CHIP_TYPE_W5500; }
    void swReset() override;
    uint8_t getMaxSockets() override { return W5500_MAX_SOCK_NUM; }

    // ==== Common Register Interface ====
    uint8_t readRegister(uint16_t addr) override;
    void writeRegister(uint16_t addr, uint8_t data) override;
    uint16_t readRegister(uint16_t addr, uint8_t* buf, uint16_t len) override;
    uint16_t writeRegister(uint16_t addr, const uint8_t* buf, uint16_t len) override;

    // ==== Socket-specific Register Interface ====
    uint8_t readSocketRegister(uint8_t socket, uint16_t addr) override;
    void writeSocketRegister(uint8_t socket, uint16_t addr, uint8_t data) override;
    uint16_t readSocketRegister(uint8_t socket, uint16_t addr, uint8_t* buf, uint16_t len) override;
    uint16_t writeSocketRegister(uint8_t socket, uint16_t addr, const uint8_t* buf, uint16_t len) override;

    // ==== Network Configuration Interface ====
    void setMACAddress(const uint8_t* mac) override;
    void getMACAddress(uint8_t* mac) override;
    void setIPAddress(const uint8_t* ip) override;
    void getIPAddress(uint8_t* ip) override;
    void setGatewayIP(const uint8_t* gateway) override;
    void getGatewayIP(uint8_t* gateway) override;
    void setSubnetMask(const uint8_t* subnet) override;
    void getSubnetMask(uint8_t* subnet) override;

    // ==== Socket Operations Interface ====
    void executeSocketCommand(uint8_t socket, uint8_t cmd) override;
    uint16_t getTXFreeSize(uint8_t socket) override;
    uint16_t getRXReceivedSize(uint8_t socket) override;
    void sendDataProcessing(uint8_t socket, const uint8_t* data, uint16_t len) override;
    void receiveDataProcessing(uint8_t socket, uint8_t* data, uint16_t len, bool peek = false) override;

    // ==== W5500-specific Methods ====
    
    /**
     * Set retransmission timeout
     * @param timeout Timeout value
     */
    void setRetransmissionTime(uint16_t timeout);

    /**
     * Set retransmission count
     * @param retry Retry count
     */
    void setRetransmissionCount(uint8_t retry);

    /**
     * Set PHY configuration register
     * @param val Configuration value
     */
    void setPHYCFGR(uint8_t val);

    /**
     * Get PHY configuration register
     * @return Configuration value
     */
    uint8_t getPHYCFGR();

    /**
     * Read data from socket buffer
     * @param socket Socket number
     * @param src Source address in buffer
     * @param dst Destination buffer
     * @param len Length to read
     */
    void readData(uint8_t socket, uint16_t src, uint8_t* dst, uint16_t len);

    /**
     * Send data processing with offset
     * @param socket Socket number  
     * @param data_offset Data offset
     * @param data Data buffer
     * @param len Data length
     */
    void sendDataProcessingOffset(uint8_t socket, uint16_t data_offset, const uint8_t* data, uint16_t len);

    // Buffer sizes
    static const uint16_t SSIZE = 2048;  // Max Tx buffer size per socket
    static const uint16_t RSIZE = 2048;  // Max Rx buffer size per socket
};

#endif  // W5500_CHIP_H
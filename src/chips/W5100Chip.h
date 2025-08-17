/*
 * W5100Chip.h - W5100 chip implementation for Ethernet3 library
 *
 * This file provides W5100-specific implementation of the EthernetChip interface
 * with integrated register access and networking functionality.
 */

#ifndef W5100_CHIP_H
#define W5100_CHIP_H

#include "EthernetChip.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#endif

// W5100 Constants
#define W5100_MAX_SOCK_NUM 4

// W5100 Common Registers
#define W5100_MR        0x0000    // Mode Register
#define W5100_GAR       0x0001    // Gateway Address (4 bytes)
#define W5100_SUBR      0x0005    // Subnet Mask (4 bytes)  
#define W5100_SHAR      0x0009    // Source Hardware Address (6 bytes)
#define W5100_SIPR      0x000F    // Source IP Address (4 bytes)
#define W5100_IR        0x0015    // Interrupt Register
#define W5100_IMR       0x0016    // Interrupt Mask Register  
#define W5100_RTR       0x0017    // Retry Time Register (2 bytes)
#define W5100_RCR       0x0019    // Retry Count Register
#define W5100_RMSR      0x001A    // RX Memory Size Register
#define W5100_TMSR      0x001B    // TX Memory Size Register

// W5100 Socket Registers (per socket)
#define W5100_S0_MR     0x0400    // Socket 0 Mode Register
#define W5100_S0_CR     0x0401    // Socket 0 Command Register
#define W5100_S0_IR     0x0402    // Socket 0 Interrupt Register
#define W5100_S0_SR     0x0403    // Socket 0 Status Register
#define W5100_S0_PORT   0x0404    // Socket 0 Source Port (2 bytes)
#define W5100_S0_DHAR   0x0406    // Socket 0 Destination Hardware Address (6 bytes)
#define W5100_S0_DIPR   0x040C    // Socket 0 Destination IP Address (4 bytes)
#define W5100_S0_DPORT  0x0410    // Socket 0 Destination Port (2 bytes)
#define W5100_S0_MSSR   0x0412    // Socket 0 Maximum Segment Size (2 bytes)
#define W5100_S0_PROTO  0x0414    // Socket 0 Protocol in IP Raw mode
#define W5100_S0_TOS    0x0415    // Socket 0 Type of Service
#define W5100_S0_TTL    0x0416    // Socket 0 Time to Live
#define W5100_S0_TX_FSR 0x0420    // Socket 0 TX Free Size (2 bytes)
#define W5100_S0_TX_RD  0x0422    // Socket 0 TX Read Pointer (2 bytes)
#define W5100_S0_TX_WR  0x0424    // Socket 0 TX Write Pointer (2 bytes)
#define W5100_S0_RX_RSR 0x0426    // Socket 0 RX Received Size (2 bytes)
#define W5100_S0_RX_RD  0x0428    // Socket 0 RX Read Pointer (2 bytes)

// Socket register offsets for other sockets
#define W5100_SOCK_REG_SIZE 0x0100  // Size between socket register blocks

// Mode Register (MR) bits
#define W5100_MR_RST    0x80    // Software Reset
#define W5100_MR_PB     0x10    // Ping Block
#define W5100_MR_PPPOE  0x08    // PPPoE Mode
#define W5100_MR_LB     0x04    // Loopback
#define W5100_MR_AI     0x02    // Address Auto-Increment
#define W5100_MR_IND    0x01    // Indirect Mode

// Socket Mode Register (Sn_MR) bits
#define W5100_SnMR_CLOSE  0x00  // Closed
#define W5100_SnMR_TCP    0x01  // TCP mode
#define W5100_SnMR_UDP    0x02  // UDP mode  
#define W5100_SnMR_IPRAW  0x03  // IP Raw mode
#define W5100_SnMR_MACRAW 0x04  // MAC Raw mode (Socket 0 only)
#define W5100_SnMR_PPPOE  0x05  // PPPoE mode (Socket 0 only)
#define W5100_SnMR_ND     0x20  // No Delayed ACK
#define W5100_SnMR_MULTI  0x80  // Multicast (for UDP)

// Socket Command Register (Sn_CR)
#define W5100_SnCR_OPEN      0x01  // Open socket
#define W5100_SnCR_LISTEN    0x02  // Listen (TCP)
#define W5100_SnCR_CONNECT   0x04  // Connect (TCP)
#define W5100_SnCR_DISCON    0x08  // Disconnect
#define W5100_SnCR_CLOSE     0x10  // Close socket
#define W5100_SnCR_SEND      0x20  // Send data
#define W5100_SnCR_SEND_MAC  0x21  // Send data (MAC Raw)
#define W5100_SnCR_SEND_KEEP 0x22  // Send keep alive
#define W5100_SnCR_RECV      0x40  // Receive data

// Socket Status Register (Sn_SR) values
#define W5100_SnSR_CLOSED      0x00  // Socket closed
#define W5100_SnSR_INIT        0x13  // Socket initialized (TCP)
#define W5100_SnSR_LISTEN      0x14  // Socket listening (TCP)
#define W5100_SnSR_SYNSENT     0x15  // SYN sent (TCP)
#define W5100_SnSR_SYNRECV     0x16  // SYN received (TCP)
#define W5100_SnSR_ESTABLISHED 0x17  // Connection established (TCP)
#define W5100_SnSR_FIN_WAIT    0x18  // FIN_WAIT (TCP)
#define W5100_SnSR_CLOSING     0x1A  // Closing (TCP)
#define W5100_SnSR_TIME_WAIT   0x1B  // TIME_WAIT (TCP)
#define W5100_SnSR_CLOSE_WAIT  0x1C  // CLOSE_WAIT (TCP)
#define W5100_SnSR_LAST_ACK    0x1D  // LAST_ACK (TCP)
#define W5100_SnSR_UDP         0x22  // UDP mode
#define W5100_SnSR_IPRAW       0x32  // IP Raw mode
#define W5100_SnSR_MACRAW      0x42  // MAC Raw mode
#define W5100_SnSR_PPPOE       0x5F  // PPPoE mode

// W5100 Memory mapping
#define W5100_TX_MEM_BASE   0x4000  // TX memory base
#define W5100_RX_MEM_BASE   0x6000  // RX memory base  
#define W5100_TX_MEM_SIZE   0x2000  // 8KB TX memory
#define W5100_RX_MEM_SIZE   0x2000  // 8KB RX memory

/**
 * W5100 chip implementation with integrated functionality
 */
class W5100Chip : public EthernetChip {
   private:
    // SPI settings for W5100
    static SPISettings spi_settings;

    // Low-level SPI access
    void setSS();
    void resetSS();

   public:
    /**
     * Constructor
     * @param platform_interface Pointer to unified platform implementation
     * @param chip_select_pin Chip select pin number
     */
    W5100Chip(EthernetPlatform* platform_interface, uint8_t chip_select_pin);

    virtual ~W5100Chip() {}

    // ==== Core EthernetChip Interface ====
    bool init() override;
    bool linkActive() override;
    uint8_t getChipType() override { return CHIP_TYPE_W5100; }
    void swReset() override;
    uint8_t getMaxSockets() override { return W5100_MAX_SOCK_NUM; }

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

    // ==== W5100-specific Methods ====
    
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
     * Read data from socket buffer
     * @param socket Socket number
     * @param src Source address in buffer
     * @param dst Destination buffer
     * @param len Length to read
     */
    void readData(uint8_t socket, uint16_t src, uint8_t* dst, uint16_t len);

    /**
     * Write data to socket buffer
     * @param socket Socket number
     * @param dst Destination address in buffer
     * @param src Source buffer
     * @param len Length to write
     */
    void writeData(uint8_t socket, uint16_t dst, const uint8_t* src, uint16_t len);

    // Buffer sizes
    static const uint16_t SSIZE = 2048;  // Max Tx buffer size per socket (2KB each)
    static const uint16_t RSIZE = 2048;  // Max Rx buffer size per socket (2KB each)
};

#endif  // W5100_CHIP_H
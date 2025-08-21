/*
 * EthernetChip.h - Abstract Ethernet chip interface for Ethernet3 library
 *
 * This file provides a common interface for different Ethernet chips
 * enabling support for W5100, W5500 and future chip variants.
 */

#ifndef ETHERNET_CHIP_H
#define ETHERNET_CHIP_H

#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>

#include "utility/wiznet_registers.h"

// Register Interface Macros

#define __SOCKET_REGISTER8(name, address)                                               \
    inline void write##name(uint8_t _s, uint8_t _data) { writeSn(_s, address, _data); } \
    inline uint8_t read##name(uint8_t _s) { return readSn(_s, address); }

#define __SOCKET_REGISTER16(name, address)          \
    void write##name(uint8_t _s, uint16_t _data) {  \
        writeSn(_s, address, _data >> 8);           \
        writeSn(_s, address + 1, _data & 0xFF);     \
    }                                               \
    uint16_t read##name(uint8_t _s) {               \
        uint16_t res = readSn(_s, address);         \
        res = (res << 8) + readSn(_s, address + 1); \
        return res;                                 \
    }

#define __SOCKET_REGISTER_N(name, address, size)                                                   \
    uint16_t write##name(uint8_t _s, uint8_t* _buff) { return writeSn(_s, address, _buff, size); } \
    uint16_t read##name(uint8_t _s, uint8_t* _buff) { return readSn(_s, address, _buff, size); }

// Socket Register Interface Macros DEF Only

#define __SOCKET_REGISTER8_DEF(name)                                \
    virtual inline void write##name(uint8_t _s, uint8_t _data) = 0; \
    virtual inline uint8_t read##name(uint8_t _s) = 0;

#define __SOCKET_REGISTER16_DEF(name)                         \
    virtual void write##name(uint8_t _s, uint16_t _data) = 0; \
    virtual uint16_t read##name(uint8_t _s) = 0;

#define __SOCKET_REGISTER_N_DEF(name)                             \
    virtual uint16_t write##name(uint8_t _s, uint8_t* _buff) = 0; \
    virtual uint16_t read##name(uint8_t _s, uint8_t* _buff) = 0;

// GP Register Interface Macros (w5500 specific??)

#define __GP_REGISTER8(name, address)                                       \
    inline void write##name(uint8_t _data) { write(address, 0x04, _data); } \
    inline uint8_t read##name() { return read(address, 0x00); }
#define __GP_REGISTER16(name, address)              \
    void write##name(uint16_t _data) {              \
        write(address, 0x04, _data >> 8);           \
        write(address + 1, 0x04, _data & 0xFF);     \
    }                                               \
    uint16_t read##name() {                         \
        uint16_t res = read(address, 0x00);         \
        res = (res << 8) + read(address + 1, 0x00); \
        return res;                                 \
    }
#define __GP_REGISTER_N(name, address, size)                                           \
    uint16_t write##name(uint8_t* _buff) { return write(address, 0x04, _buff, size); } \
    uint16_t read##name(uint8_t* _buff) { return read(address, 0x00, _buff, size); }

// GP Register Interface Macro Def ONLY

#define __GP_REGISTER8_DEF(name)                        \
    virtual inline void write##name(uint8_t _data) = 0; \
    virtual inline uint8_t read##name() = 0;

#define __GP_REGISTER16_DEF(name)                 \
    virtual void write##name(uint16_t _data) = 0; \
    virtual uint16_t read##name() = 0;

#define __GP_REGISTER_N_DEF(name)                     \
    virtual uint16_t write##name(uint8_t* _buff) = 0; \
    virtual uint16_t read##name(uint8_t* _buff) = 0;

class EthernetChip {
   protected:
    uint8_t _cs_pin;

    inline void initSS() { pinMode(_cs_pin, OUTPUT); }
    inline void setSS() { digitalWrite(_cs_pin, LOW); }
    inline void resetSS() { digitalWrite(_cs_pin, HIGH); }

   public:
    const uint16_t SSIZE = 0;  // REDEFINE in derived classes
    const uint16_t RSIZE = 0;

    EthernetChip(uint8_t cs_pin) : _cs_pin(cs_pin) {}

    /**
     * Initialize the chip
     * @return true if initialization successful, false otherwise
     */
    virtual bool init() = 0;

    /**
     * Check physical link status
     * @return true if link is active, false otherwise
     */
    virtual bool linkActive() = 0;

    /**
     * Get chip type identifier
     * @return Chip type constant (CHIP_TYPE_W5100, CHIP_TYPE_W5500, etc.)
     */
    virtual uint8_t getChipType() = 0;

    /**
     * Software reset the chip
     */
    virtual void swReset() = 0;

    /**
     * Get chip select pin
     * @return Chip select pin number
     */
    uint8_t getCSPin() const { return _cs_pin; }

    // ---------------------------------------------------------------------
    // Common network configuration accessors (must be implemented)
    // ---------------------------------------------------------------------
    /** Set gateway IPv4 address (4 bytes) */
    virtual void setGatewayIp(uint8_t* addr) = 0;
    /** Get gateway IPv4 address (4 bytes) */
    virtual void getGatewayIp(uint8_t* addr) = 0;
    /** Set subnet mask (4 bytes) */
    virtual void setSubnetMask(uint8_t* addr) = 0;
    /** Get subnet mask (4 bytes) */
    virtual void getSubnetMask(uint8_t* addr) = 0;
    /** Set MAC address (6 bytes) */
    virtual void setMACAddress(uint8_t* addr) = 0;
    /** Get MAC address (6 bytes) */
    virtual void getMACAddress(uint8_t* addr) = 0;
    /** Set IPv4 address (4 bytes) */
    virtual void setIPAddress(uint8_t* addr) = 0;
    /** Get IPv4 address (4 bytes) */
    virtual void getIPAddress(uint8_t* addr) = 0;
    /** Set retransmission timeout (ms units per chip spec) */
    virtual void setRetransmissionTime(uint16_t timeout) = 0;
    /** Set retransmission retry count */
    virtual void setRetransmissionCount(uint8_t retry) = 0;

    /**
     * @brief	This function is being used for copy the data form Receive buffer of the chip to
     * application buffer.
     *
     * It calculate the actual physical address where one has to read
     * the data from Receive buffer. Here also take care of the condition while it exceed
     * the Rx memory uper-bound of socket.
     */
    virtual void read_data(SOCKET s, volatile uint16_t src, volatile uint8_t* dst,
                           uint16_t len) = 0;

    /**
     * @brief	 This function is being called by send() and sendto() function also.
     *
     * This function read the Tx write pointer register and after copy the data in buffer update the
     * Tx write pointer register. User should read upper byte first and lower byte later to get
     * proper value.
     */
    virtual void send_data_processing(SOCKET s, const uint8_t* data, uint16_t len) = 0;
    /**
     * @brief A copy of send_data_processing that uses the provided ptr for the
     *        write offset.  Only needed for the "streaming" UDP API, where
     *        a single UDP packet is built up over a number of calls to
     *        send_data_processing_ptr, because TX_WR doesn't seem to get updated
     *        correctly in those scenarios
     * @param ptr value to use in place of TX_WR.  If 0, then the value is read
     *        in from TX_WR
     * @return New value for ptr, to be used in the next call
     */
    // FIXME Update documentation
    virtual void send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t* data,
                                             uint16_t len) = 0;

    /**
     * @brief	This function is being called by recv() also.
     *
     * This function read the Rx read pointer register
     * and after copy the data from receive buffer update the Rx write pointer register.
     * User should read upper byte first and lower byte later to get proper value.
     */
    virtual void recv_data_processing(SOCKET s, uint8_t* data, uint16_t len, uint8_t peek = 0) = 0;

    // ---------------------------------------------------------------------
    // PHY / Link configuration
    // ---------------------------------------------------------------------
    /** Set raw PHY configuration register (only on chips supporting it). Default no-op. */
    virtual void setPHYCFGR(uint8_t val) = 0;
    /** Get raw PHY configuration register (return 0 if unsupported). */
    virtual uint8_t getPHYCFGR() = 0;

    // ---------------------------------------------------------------------
    // Socket level register / buffer helpers
    // These abstract minimal functionality required by higher layers.
    // ---------------------------------------------------------------------
    /** Maximum simultaneous sockets supported by the chip. */
    virtual uint8_t maxSockets() = 0;

    __SOCKET_REGISTER8_DEF(SnMR)       // Mode
    __SOCKET_REGISTER8_DEF(SnCR)       // Command
    __SOCKET_REGISTER8_DEF(SnIR)       // Interrupt
    __SOCKET_REGISTER8_DEF(SnSR)       // Status
    __SOCKET_REGISTER16_DEF(SnPORT)    // Source Port
    __SOCKET_REGISTER_N_DEF(SnDHAR)    // Destination Hardw Addr
    __SOCKET_REGISTER_N_DEF(SnDIPR)    // Destination IP Addr
    __SOCKET_REGISTER16_DEF(SnDPORT)   // Destination Port
    __SOCKET_REGISTER16_DEF(SnMSSR)    // Max Segment Size
    __SOCKET_REGISTER8_DEF(SnPROTO)    // Protocol in IP RAW Mode
    __SOCKET_REGISTER8_DEF(SnTOS)      // IP TOS
    __SOCKET_REGISTER8_DEF(SnTTL)      // IP TTL
    __SOCKET_REGISTER16_DEF(SnTX_FSR)  // TX Free Size
    __SOCKET_REGISTER16_DEF(SnTX_RD)   // TX Read Pointer
    __SOCKET_REGISTER16_DEF(SnTX_WR)   // TX Write Pointer
    __SOCKET_REGISTER16_DEF(SnRX_RSR)  // RX Free Size
    __SOCKET_REGISTER16_DEF(SnRX_RD)   // RX Read Pointer
    __SOCKET_REGISTER16_DEF(SnRX_WR)   // RX Write Pointer (supported?)

    virtual uint16_t getTXFreeSize(uint8_t sock) = 0;
    virtual uint16_t getRXReceivedSize(uint8_t sock) = 0;

    virtual uint8_t readSn(uint8_t _s, uint16_t _addr) = 0;
    virtual uint8_t writeSn(uint8_t _s, uint16_t _addr, uint8_t _data) = 0;
    virtual uint16_t readSn(uint8_t _s, uint16_t _addr, uint8_t* _buf, uint16_t len) = 0;
    virtual uint16_t writeSn(uint8_t _s, uint16_t _addr, uint8_t* _buf, uint16_t len) = 0;

    virtual uint8_t write(uint16_t _addr, uint8_t _cb, uint8_t _data) = 0;
    virtual uint16_t write(uint16_t _addr, uint8_t _cb, const uint8_t* buf, uint16_t len) = 0;
    virtual uint8_t read(uint16_t _addr, uint8_t _cb) = 0;
    virtual uint16_t read(uint16_t _addr, uint8_t _cb, uint8_t* buf, uint16_t len) = 0;

    virtual void execCmdSn(uint8_t sock, SockCMD _cmd) = 0;

    // GP Register accessors defs
    __GP_REGISTER8_DEF(MR);       // Mode
    __GP_REGISTER_N_DEF(GAR);     // Gateway IP address
    __GP_REGISTER_N_DEF(SUBR);    // Subnet mask address
    __GP_REGISTER_N_DEF(SHAR);    // Source MAC address
    __GP_REGISTER_N_DEF(SIPR);    // Source IP address
    __GP_REGISTER8_DEF(IR);       // Interrupt
    __GP_REGISTER8_DEF(IMR);      // Interrupt Mask
    __GP_REGISTER16_DEF(RTR);     // Timeout address
    __GP_REGISTER8_DEF(RCR);      // Retry count
    __GP_REGISTER_N_DEF(UIPR);    // Unreachable IP address in UDP mode
    __GP_REGISTER16_DEF(UPORT);   // Unreachable Port address in UDP mode
    __GP_REGISTER8_DEF(PHYCFGR);  // PHY Configuration register, default value: 0b 1011 1xxx
};

#endif  // ETHERNET_CHIP_H
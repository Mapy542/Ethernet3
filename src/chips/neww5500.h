#ifndef w55002_h
#define w55002_h

#include "EthernetChip.h"
#include "utility/socket.h"
#include "utility/wiznet_registers.h"

class W5500 : public EthernetChip {
   protected:
    SPISettings wiznet_SPI_settings;

   public:
    const uint16_t SSIZE = 2048;
    const uint16_t RSIZE = 2048;

    W5500(uint8_t cs_pin) : EthernetChip(cs_pin) {
        wiznet_SPI_settings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
    }

    virtual bool init() override;
    virtual bool linkActive() override;
    virtual uint8_t getChipType() override;
    virtual void swReset() override;

    virtual uint8_t readSn(SOCKET _s, uint16_t _addr) override;
    virtual uint8_t writeSn(SOCKET _s, uint16_t _addr, uint8_t _data) override;
    virtual uint16_t readSn(SOCKET _s, uint16_t _addr, uint8_t* _buf, uint16_t len) override;
    virtual uint16_t writeSn(SOCKET _s, uint16_t _addr, uint8_t* _buf, uint16_t len) override;

    virtual uint8_t write(uint16_t _addr, uint8_t _cb, uint8_t _data) override;
    virtual uint16_t write(uint16_t _addr, uint8_t _cb, const uint8_t* buf, uint16_t len) override;
    virtual uint8_t read(uint16_t _addr, uint8_t _cb) override;
    virtual uint16_t read(uint16_t _addr, uint8_t _cb, uint8_t* buf, uint16_t len) override;

    virtual void execCmdSn(SOCKET sock, SockCMD _cmd) override;

    // ---------------------------------------------------------------------
    // Common network configuration accessors (must be implemented)
    // ---------------------------------------------------------------------
    /** Set gateway IPv4 address (4 bytes) */
    virtual void setGatewayIp(uint8_t* addr) override;
    /** Get gateway IPv4 address (4 bytes) */
    virtual void getGatewayIp(uint8_t* addr) override;
    /** Set subnet mask (4 bytes) */
    virtual void setSubnetMask(uint8_t* addr) override;
    /** Get subnet mask (4 bytes) */
    virtual void getSubnetMask(uint8_t* addr) override;
    /** Set MAC address (6 bytes) */
    virtual void setMACAddress(uint8_t* addr) override;
    /** Get MAC address (6 bytes) */
    virtual void getMACAddress(uint8_t* addr) override;
    /** Set IPv4 address (4 bytes) */
    virtual void setIPAddress(uint8_t* addr) override;
    /** Get IPv4 address (4 bytes) */
    virtual void getIPAddress(uint8_t* addr) override;
    /** Set retransmission timeout (ms units per chip spec) */
    virtual void setRetransmissionTime(uint16_t timeout) override;
    /** Set retransmission retry count */
    virtual void setRetransmissionCount(uint8_t retry) override;

    /**
     * @brief	This function is being used for copy the data form Receive buffer of the chip to
     * application buffer.
     *
     * It calculate the actual physical address where one has to read
     * the data from Receive buffer. Here also take care of the condition while it exceed
     * the Rx memory uper-bound of socket.
     */
    virtual void read_data(SOCKET s, volatile uint16_t src, volatile uint8_t* dst,
                           uint16_t len) override;

    /**
     * @brief	 This function is being called by send() and sendto() function also.
     *
     * This function read the Tx write pointer register and after copy the data in buffer update the
     * Tx write pointer register. User should read upper byte first and lower byte later to get
     * proper value.
     */
    virtual void send_data_processing(SOCKET s, const uint8_t* data, uint16_t len) override;
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
                                             uint16_t len) override;

    /**
     * @brief	This function is being called by recv() also.
     *
     * This function read the Rx read pointer register
     * and after copy the data from receive buffer update the Rx write pointer register.
     * User should read upper byte first and lower byte later to get proper value.
     */
    virtual void recv_data_processing(SOCKET s, uint8_t* data, uint16_t len,
                                      uint8_t peek = 0) override;

    // ---------------------------------------------------------------------
    // PHY / Link configuration
    // ---------------------------------------------------------------------
    /** Set PHY configuration register */
    void setPHYCFGR(uint8_t phyconf);
    /** Get PHY configuration register */
    uint8_t getPHYCFGR();

    // ---------------------------------------------------------------------
    // Socket level register / buffer helpers
    // These abstract minimal functionality required by higher layers.
    // ---------------------------------------------------------------------
    /** Maximum simultaneous sockets supported by the chip. */
    virtual uint8_t maxSockets() override { return W5500_MAX_SOCK_NUM; }

    virtual uint16_t getTXFreeSize(SOCKET sock) override;
    virtual uint16_t getRXReceivedSize(SOCKET sock) override;

    virtual uint8_t readSn(SOCKET _s, uint16_t _addr) override;
    virtual uint8_t writeSn(SOCKET _s, uint16_t _addr, uint8_t _data) override;
    virtual uint16_t readSn(SOCKET _s, uint16_t _addr, uint8_t* _buf, uint16_t len) override;
    virtual uint16_t writeSn(SOCKET _s, uint16_t _addr, uint8_t* _buf, uint16_t len) override;

    virtual uint8_t write(uint16_t _addr, uint8_t _cb, uint8_t _data) override;
    virtual uint16_t write(uint16_t _addr, uint8_t _cb, const uint8_t* buf, uint16_t len) override;
    virtual uint8_t read(uint16_t _addr, uint8_t _cb) override;
    virtual uint16_t read(uint16_t _addr, uint8_t _cb, uint8_t* buf, uint16_t len) override;

    virtual void execCmdSn(SOCKET sock, SockCMD _cmd) override;

    __SOCKET_REGISTER8(SnMR, 0x0000)        // Mode
    __SOCKET_REGISTER8(SnCR, 0x0001)        // Command
    __SOCKET_REGISTER8(SnIR, 0x0002)        // Interrupt
    __SOCKET_REGISTER8(SnSR, 0x0003)        // Status
    __SOCKET_REGISTER16(SnPORT, 0x0004)     // Source Port
    __SOCKET_REGISTER_N(SnDHAR, 0x0006, 6)  // Destination Hardw Addr
    __SOCKET_REGISTER_N(SnDIPR, 0x000C, 4)  // Destination IP Addr
    __SOCKET_REGISTER16(SnDPORT, 0x0010)    // Destination Port
    __SOCKET_REGISTER16(SnMSSR, 0x0012)     // Max Segment Size
    __SOCKET_REGISTER8(SnPROTO, 0x0014)     // Protocol in IP RAW Mode
    __SOCKET_REGISTER8(SnTOS, 0x0015)       // IP TOS
    __SOCKET_REGISTER8(SnTTL, 0x0016)       // IP TTL
    __SOCKET_REGISTER16(SnTX_FSR, 0x0020)   // TX Free Size
    __SOCKET_REGISTER16(SnTX_RD, 0x0022)    // TX Read Pointer
    __SOCKET_REGISTER16(SnTX_WR, 0x0024)    // TX Write Pointer
    __SOCKET_REGISTER16(SnRX_RSR, 0x0026)   // RX Free Size
    __SOCKET_REGISTER16(SnRX_RD, 0x0028)    // RX Read Pointer
    __SOCKET_REGISTER16(SnRX_WR, 0x002A)    // RX Write Pointer (supported?)

    __GP_REGISTER8(MR, 0x0000);        // Mode
    __GP_REGISTER_N(GAR, 0x0001, 4);   // Gateway IP address
    __GP_REGISTER_N(SUBR, 0x0005, 4);  // Subnet mask address
    __GP_REGISTER_N(SHAR, 0x0009, 6);  // Source MAC address
    __GP_REGISTER_N(SIPR, 0x000F, 4);  // Source IP address
    __GP_REGISTER8(IR, 0x0015);        // Interrupt
    __GP_REGISTER8(IMR, 0x0016);       // Interrupt Mask
    __GP_REGISTER16(RTR, 0x0019);      // Timeout address
    __GP_REGISTER8(RCR, 0x001B);       // Retry count
    __GP_REGISTER_N(UIPR, 0x0028, 4);  // Unreachable IP address in UDP mode
    __GP_REGISTER16(UPORT, 0x002C);    // Unreachable Port address in UDP mode
    __GP_REGISTER8(PHYCFGR, 0x002E);   // PHY Configuration register, default value: 0b 1011 1xxx
};

#endif  // w55002_h
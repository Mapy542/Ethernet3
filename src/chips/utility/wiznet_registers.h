/*
 * wiznet_registers.h - Unified WIZnet register definitions for Ethernet3 library
 *
 * This file provides a common set of register definitions and constants
 * for W5100 and W5500 Ethernet chips, unifying definitions from
 * various source files for clarity and maintainability.
 */

#ifndef WIZNET_REGISTERS_H
#define WIZNET_REGISTERS_H

#include <stdint.h>
typedef uint8_t SOCKET;

//==============================================================================
//--- Common WIZnet Chip Constants ---------------------------------------------
//==============================================================================

#define W5100_MAX_SOCK_NUM 4
#define W5500_MAX_SOCK_NUM 8
#define MAX_SOCK_NUM \
    W5500_MAX_SOCK_NUM  // Use W5500 max sockets for compatibility  (will use some extra resources
                        // on W5100)

//==============================================================================
//--- Common Register Addresses ------------------------------------------------
//==============================================================================

// Common Registers (accessible in both W5100 and W5500)
#define WIZ_MR 0x0000    // Mode Register
#define WIZ_GAR 0x0001   // Gateway Address (4 bytes)
#define WIZ_SUBR 0x0005  // Subnet Mask (4 bytes)
#define WIZ_SHAR 0x0009  // Source Hardware (MAC) Address (6 bytes)
#define WIZ_SIPR 0x000F  // Source IP Address (4 bytes)
#define WIZ_IR 0x0015    // Interrupt Register
#define WIZ_IMR 0x0016   // Interrupt Mask Register
#define WIZ_RTR 0x0017   // Retry Time Register (2 bytes, W5100)
#define WIZ_RCR 0x0019   // Retry Count Register (W5100)

// W5500 Specific Common Registers
#define W5500_RTR 0x0019      // Timeout Register (W5500)
#define W5500_RCR 0x001B      // Retry Count Register (W5500)
#define W5500_UIPR 0x0028     // Unreachable IP Address
#define W5500_UPORT 0x002C    // Unreachable Port
#define W5500_PHYCFGR 0x002E  // PHY Configuration Register

// W5100 Specific Memory Size Registers (for TX/RX buffer allocation)
#define W5100_RMSR 0x001A  // RX Memory Size Register (W5100 only)
#define W5100_TMSR 0x001B  // TX Memory Size Register (W5100 only)

//==============================================================================
//--- Socket Register Addresses (Offsets from Socket Base) ---------------------
//==============================================================================

#define WIZ_Sn_MR 0x0000      // Socket n Mode Register
#define WIZ_Sn_CR 0x0001      // Socket n Command Register
#define WIZ_Sn_IR 0x0002      // Socket n Interrupt Register
#define WIZ_Sn_SR 0x0003      // Socket n Status Register
#define WIZ_Sn_PORT 0x0004    // Socket n Source Port (2 bytes)
#define WIZ_Sn_DHAR 0x0006    // Socket n Destination Hardware (MAC) Address (6 bytes)
#define WIZ_Sn_DIPR 0x000C    // Socket n Destination IP Address (4 bytes)
#define WIZ_Sn_DPORT 0x0010   // Socket n Destination Port (2 bytes)
#define WIZ_Sn_MSSR 0x0012    // Socket n Maximum Segment Size (2 bytes)
#define WIZ_Sn_PROTO 0x0014   // Socket n Protocol in IP Raw mode
#define WIZ_Sn_TOS 0x0015     // Socket n Type of Service
#define WIZ_Sn_TTL 0x0016     // Socket n Time to Live
#define WIZ_Sn_TX_FSR 0x0020  // Socket n TX Free Size (2 bytes)
#define WIZ_Sn_TX_RD 0x0022   // Socket n TX Read Pointer (2 bytes)
#define WIZ_Sn_TX_WR 0x0024   // Socket n TX Write Pointer (2 bytes)
#define WIZ_Sn_RX_RSR 0x0026  // Socket n RX Received Size (2 bytes)
#define WIZ_Sn_RX_RD 0x0028   // Socket n RX Read Pointer (2 bytes)
#define W5500_SnRX_WR 0x002A  // W5500 Socket n RX Write Pointer

// W5100 uses a fixed address for each socket block
#define W5100_S0_MR 0x0400
#define W5100_SOCK_REG_SIZE 0x0100

//==============================================================================
//--- Register Bit Definitions -------------------------------------------------
//==============================================================================

// Mode Register (MR) bits
#define WIZ_MR_RST 0x80    // Software Reset
#define WIZ_MR_PB 0x10     // Ping Block
#define WIZ_MR_PPPOE 0x08  // PPPoE Mode
#define WIZ_MR_LB 0x04     // Loopback
#define WIZ_MR_AI 0x02     // Address Auto-Increment (W5100)
#define WIZ_MR_IND 0x01    // Indirect Mode (W5100)

// Socket Mode Register (Sn_MR) bits
#define WIZ_SnMR_CLOSE 0x00   // Closed
#define WIZ_SnMR_TCP 0x01     // TCP mode
#define WIZ_SnMR_UDP 0x02     // UDP mode
#define WIZ_SnMR_IPRAW 0x03   // IP Raw mode
#define WIZ_SnMR_MACRAW 0x04  // MAC Raw mode
#define WIZ_SnMR_PPPOE 0x05   // PPPoE mode
#define WIZ_SnMR_ND 0x20      // No Delayed ACK
#define WIZ_SnMR_MULTI 0x80   // Multicast (for UDP)

// Socket Command Register (Sn_CR) commands
enum SockCMD {
    Sock_OPEN = 0x01,
    Sock_LISTEN = 0x02,
    Sock_CONNECT = 0x04,
    Sock_DISCON = 0x08,
    Sock_CLOSE = 0x10,
    Sock_SEND = 0x20,
    Sock_SEND_MAC = 0x21,
    Sock_SEND_KEEP = 0x22,
    Sock_RECV = 0x40
};

// Socket Interrupt Register (Sn_IR) bits
#define WIZ_SnIR_SEND_OK 0x10  // Send OK
#define WIZ_SnIR_TIMEOUT 0x08  // Timeout
#define WIZ_SnIR_RECV 0x04     // Receive
#define WIZ_SnIR_DISCON 0x02   // Disconnect
#define WIZ_SnIR_CON 0x01      // Connect

// Socket Status Register (Sn_SR) values
#define WIZ_SnSR_CLOSED 0x00       // Socket closed
#define WIZ_SnSR_INIT 0x13         // Socket initialized (TCP)
#define WIZ_SnSR_LISTEN 0x14       // Socket listening (TCP)
#define WIZ_SnSR_SYNSENT 0x15      // SYN sent (TCP)
#define WIZ_SnSR_SYNRECV 0x16      // SYN received (TCP)
#define WIZ_SnSR_ESTABLISHED 0x17  // Connection established (TCP)
#define WIZ_SnSR_FIN_WAIT 0x18     // FIN_WAIT (TCP)
#define WIZ_SnSR_CLOSING 0x1A      // Closing (TCP)
#define WIZ_SnSR_TIME_WAIT 0x1B    // TIME_WAIT (TCP)
#define WIZ_SnSR_CLOSE_WAIT 0x1C   // CLOSE_WAIT (TCP)
#define WIZ_SnSR_LAST_ACK 0x1D     // LAST_ACK (TCP)
#define WIZ_SnSR_UDP 0x22          // UDP mode
#define WIZ_SnSR_IPRAW 0x32        // IP Raw mode
#define WIZ_SnSR_MACRAW 0x42       // MAC Raw mode
#define WIZ_SnSR_PPPOE 0x5F        // PPPoE mode

// IP Protocol Numbers
class IPPROTO {
   public:
    static const uint8_t IP = 0;
    static const uint8_t ICMP = 1;
    static const uint8_t IGMP = 2;
    static const uint8_t GGP = 3;
    static const uint8_t TCP = 6;
    static const uint8_t PUP = 12;
    static const uint8_t UDP = 17;
    static const uint8_t IDP = 22;
    static const uint8_t ND = 77;
    static const uint8_t RAW = 255;
};

class SnMR {
   public:
    static const uint8_t CLOSE = 0x00;
    static const uint8_t TCP = 0x01;
    static const uint8_t UDP = 0x02;
    static const uint8_t IPRAW = 0x03;
    static const uint8_t MACRAW = 0x04;
    static const uint8_t PPPOE = 0x05;
    static const uint8_t ND = 0x20;
    static const uint8_t MULTI = 0x80;
};

class SnIR {
   public:
    static const uint8_t SEND_OK = 0x10;
    static const uint8_t TIMEOUT = 0x08;
    static const uint8_t RECV = 0x04;
    static const uint8_t DISCON = 0x02;
    static const uint8_t CON = 0x01;
};

class SnSR {
   public:
    static const uint8_t CLOSED = 0x00;
    static const uint8_t INIT = 0x13;
    static const uint8_t LISTEN = 0x14;
    static const uint8_t SYNSENT = 0x15;
    static const uint8_t SYNRECV = 0x16;
    static const uint8_t ESTABLISHED = 0x17;
    static const uint8_t FIN_WAIT = 0x18;
    static const uint8_t CLOSING = 0x1A;
    static const uint8_t TIME_WAIT = 0x1B;
    static const uint8_t CLOSE_WAIT = 0x1C;
    static const uint8_t LAST_ACK = 0x1D;
    static const uint8_t UDP = 0x22;
    static const uint8_t IPRAW = 0x32;
    static const uint8_t MACRAW = 0x42;
    static const uint8_t PPPOE = 0x5F;
};

// PHY Configuration Register (W5500 only)
class W5500PHYCFGR {
   public:
    static const uint8_t LNK_ON = 0x01;    // Link ON
    static const uint8_t SPD_100 = 0x02;   // Speed 100Mbps
    static const uint8_t DUP_FULL = 0x04;  // Full Duplex
    static const uint8_t AUTO = 0x08;      // Auto Negotiation
    static const uint8_t OPMODE = 0x40;    // Operation Mode
    static const uint8_t RST = 0x80;       // Reset
};

//==============================================================================
//--- W5100 Memory Mapping -----------------------------------------------------
//==============================================================================

#define W5100_TX_MEM_BASE 0x4000  // TX memory base
#define W5100_RX_MEM_BASE 0x6000  // RX memory base
#define W5100_TX_MEM_SIZE 0x2000  // 8KB TX memory
#define W5100_RX_MEM_SIZE 0x2000  // 8KB RX memory

#endif  // WIZNET_REGISTERS_H

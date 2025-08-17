/*
 * w5100_registers.h - W5100 register definitions for Ethernet3 library
 * 
 * Register definitions and constants for W5100 Ethernet chip
 */

#ifndef W5100_REGISTERS_H
#define W5100_REGISTERS_H

#include <stdint.h>

// W5100 has 4 sockets vs W5500's 8
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

// Socket Mode Register (Sn_MR) bits - similar to W5500
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

#endif // W5100_REGISTERS_H
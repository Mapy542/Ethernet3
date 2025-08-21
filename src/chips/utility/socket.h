/*
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 */

#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <Arduino.h>

#include "../EthernetChip.h"
#include "wiznet_registers.h"

typedef uint8_t SOCKET;

extern uint8_t socket(EthernetChip* chip, SOCKET s, uint8_t protocol, uint16_t port,
                      uint8_t flag);              // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close(EthernetChip* chip, SOCKET s);  // Close socket
extern uint8_t connect(EthernetChip* chip, SOCKET s, uint8_t* addr,
                       uint16_t port);  // Establish TCP connection (Active connection)
extern void disconnect(EthernetChip* chip, SOCKET s);  // disconnect the connection
extern uint8_t listen(EthernetChip* chip,
                      SOCKET s);  // Establish TCP connection (Passive connection)
extern uint16_t send(EthernetChip* chip, SOCKET s, const uint8_t* buf,
                     uint16_t len);                                            // Send data (TCP)
extern int16_t recv(EthernetChip* chip, SOCKET s, uint8_t* buf, int16_t len);  // Receive data (TCP)
extern uint16_t peek(EthernetChip* chip, SOCKET s, uint8_t* buf);
extern uint16_t sendto(EthernetChip* chip, SOCKET s, const uint8_t* buf, uint16_t len,
                       uint8_t* addr,
                       uint16_t port);  // Send data (UDP/IP RAW)
extern uint16_t recvfrom(EthernetChip* chip, SOCKET s, uint8_t* buf, uint16_t len, uint8_t* addr,
                         uint16_t* port);         // Receive data (UDP/IP RAW)
extern void flush(EthernetChip* chip, SOCKET s);  // Wait for transmission to complete

extern uint16_t igmpsend(EthernetChip* chip, SOCKET s, const uint8_t* buf, uint16_t len);

// Functions to allow buffered UDP send (i.e. where the UDP datagram is built up over a
// number of calls before being sent
/*
  @brief This function sets up a UDP datagram, the data for which will be provided by one
  or more calls to bufferData and then finally sent with sendUDP.
  @return 1 if the datagram was successfully set up, or 0 if there was an error
*/
extern int startUDP(EthernetChip* chip, SOCKET s, uint8_t* addr, uint16_t port);
/*
  @brief This function copies up to len bytes of data from buf into a UDP datagram to be
  sent later by sendUDP.  Allows datagrams to be built up from a series of bufferData calls.
  @return Number of bytes successfully buffered
*/
uint16_t bufferData(EthernetChip* chip, SOCKET s, uint16_t offset, const uint8_t* buf,
                    uint16_t len);
/*
  @brief Send a UDP datagram built up from a sequence of startUDP followed by one or more
  calls to bufferData.
  @return 1 if the datagram was successfully sent, or 0 if there was an error
*/
int sendUDP(EthernetChip* chip, SOCKET s);

#endif
/* _SOCKET_H_ */

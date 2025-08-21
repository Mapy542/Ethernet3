// Arduino DNS client for WizNet5100-based Ethernet shield
// (c) Copyright 2009-2010 MCQN Ltd.
// Released under Apache License, version 2.0

#ifndef DNSClient_h
#define DNSClient_h

#include <Arduino.h>
#include <string.h>

#include "Ethernet3.h"
#include "EthernetUdp2.h"
#include "chips/utility/socket.h"
#include "chips/utility/wiznet_registers.h"
#include "chips/w5500.h"

// Forward declare the class to break the circular dependency.
class EthernetClass;

#ifdef ETHERNET_BACKWARDS_COMPATIBILITY
// Forward declarations for global instances
extern class W5500 defaultChip;
extern class EthernetClass Ethernet;
#endif
class DNSClient {
   public:
    DNSClient(EthernetClass* eth, EthernetChip* chip);
    DNSClient(EthernetClass* eth, EthernetChip* chip, unsigned long timeout);
    
#ifdef ETHERNET_BACKWARDS_COMPATIBILITY
    // Backwards compatibility constructors that use global instances
    DNSClient();
    DNSClient(unsigned long timeout);
#endif

    // ctor
    void begin(const IPAddress& aDNSServer);

    /** Convert a numeric IP address string into a four-byte IP address.
        @param aIPAddrString IP address to convert
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int inet_aton(const char* aIPAddrString, IPAddress& aResult);

    /** Resolve the given hostname to an IP address.
        @param aHostname Name to be resolved
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int getHostByName(const char* aHostname, IPAddress& aResult);

   protected:
    EthernetClass* _ethernet;  // Pointer to the Ethernet class instance
    EthernetChip* _chip;       // Pointer to the Ethernet chip interface
    uint16_t BuildRequest(const char* aName);
    uint16_t ProcessResponse(uint16_t aTimeout, IPAddress& aAddress);

    IPAddress iDNSServer;
    uint16_t iRequestId;
    EthernetUDP iUdp;
};

#endif

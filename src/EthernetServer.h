#ifndef ethernetserver_h
#define ethernetserver_h

#include "Ethernet3.h"
#include "EthernetClient.h"
#include "Server.h"
#include "chips/EthernetChip.h"
#include "chips/utility/socket.h"

#ifdef ETHERNET_BACKWARDS_COMPATIBILITY
// Forward declarations for global instances
extern class W5500 defaultChip;
extern class EthernetClass Ethernet;
#endif

class EthernetClient;

class EthernetServer : public Server {
   private:
    EthernetClass* _ethernet;  // Pointer to the Ethernet class instance
    EthernetChip* _chip;       // Pointer to the Ethernet chip interface
    uint16_t _port;

    void accept();

   public:
    EthernetServer(EthernetClass* eth, EthernetChip* chip, uint16_t port);
    
#ifdef ETHERNET_BACKWARDS_COMPATIBILITY
    // Backwards compatibility constructor that uses global instances
    EthernetServer(uint16_t port);
#endif
    EthernetClient available();
    virtual void begin();
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t* buf, size_t size);
    using Print::write;
};

#endif

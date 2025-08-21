#ifndef ethernetserver_h
#define ethernetserver_h

#include "Ethernet3.h"
#include "EthernetClient.h"
#include "Server.h"
#include "chips/EthernetChip.h"
#include "chips/utility/socket.h"

class EthernetClient;

class EthernetServer : public Server {
   private:
    EthernetChip* _chip;  // Pointer to the Ethernet chip interface
    uint16_t _port;

    void accept();

   public:
    EthernetServer(EthernetChip* chip, uint16_t port);
    EthernetClient available();
    virtual void begin();
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t* buf, size_t size);
    using Print::write;
};

#endif

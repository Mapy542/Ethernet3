#ifndef ethernetclient_h
#define ethernetclient_h

#include "Client.h"
#include "Dns.h"
#include "Ethernet3.h"
#include "IPAddress.h"
#include "chips/EthernetChip.h"
#include "chips/utility/socket.h"

class EthernetClient : public Client {
   private:
    EthernetClass *_ethernet;  // Pointer to the Ethernet class instance
    EthernetChip *_chip;       // Pointer to the Ethernet chip interface
   public:
    EthernetClient(EthernetClass *eth, EthernetChip *chip);
    EthernetClient(EthernetClass *eth, EthernetChip *chip, uint8_t sock);

    uint8_t status();
    virtual int connect(IPAddress ip, uint16_t port);
    virtual int connect(const char *host, uint16_t port);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual int available();
    virtual int read();
    virtual int read(uint8_t *buf, size_t size);
    virtual int peek();
    virtual void flush();
    virtual void stop();
    virtual uint8_t connected();
    virtual operator bool();
    virtual bool operator==(const EthernetClient &);
    virtual bool operator!=(const EthernetClient &rhs) { return !this->operator==(rhs); };

    friend class EthernetServer;

    using Print::write;

   private:
    static uint16_t _srcport;
    uint8_t _sock;
};

#endif

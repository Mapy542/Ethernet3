#ifndef ethernetclient_h
#define ethernetclient_h
#include "Arduino.h"
#include "Client.h"
#include "IPAddress.h"
#include "Print.h"

// Forward declaration for multi-instance support
class Ethernet3Class;

class EthernetClient : public Client {
   public:
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
    EthernetClient();              // Backward compatibility constructor (uses global Ethernet)
    EthernetClient(uint8_t sock);  // Backward compatibility with socket number
#endif
    EthernetClient(Ethernet3Class* ethernet_instance);  // Modern multi-instance constructor

    uint8_t status();
    virtual int connect(IPAddress ip, uint16_t port);
    virtual int connect(const char* host, uint16_t port);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t* buf, size_t size);
    virtual int available();
    virtual int read();
    virtual int read(uint8_t* buf, size_t size);
    virtual int peek();
    virtual void flush();
    virtual void stop();
    virtual uint8_t connected();
    virtual operator bool();
    virtual bool operator==(const EthernetClient&);
    virtual bool operator!=(const EthernetClient& rhs) { return !this->operator==(rhs); };

    friend class EthernetServer;

    using Print::write;

   private:
    uint16_t _srcport;  // Instance-specific source port (no longer static)
    uint8_t _sock;
    Ethernet3Class* _ethernet;  // Associated Ethernet instance

    // Helper methods for multi-instance support
    Ethernet3Class* getEthernetInstance();
    uint8_t getMaxSockets();
};

#endif

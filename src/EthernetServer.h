#ifndef ethernetserver_h
#define ethernetserver_h

#include "Server.h"

class EthernetClient;
class Ethernet3Class;  // Forward declaration for multi-instance support

class EthernetServer : 
public Server {
private:
  uint16_t _port;
  Ethernet3Class* _ethernet;  // Associated Ethernet instance
  void accept();
public:
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
  EthernetServer(uint16_t);  // Backward compatibility constructor (uses global Ethernet)
#endif
  EthernetServer(uint16_t port, Ethernet3Class* ethernet_instance);  // Modern multi-instance constructor
  EthernetClient available();
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
  
private:
  // Helper methods for multi-instance support
  Ethernet3Class* getEthernetInstance();
};

#endif

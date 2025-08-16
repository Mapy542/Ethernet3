#include "chips/utility/socket.h"
#include "chips/utility/w5500.h"
extern "C" {
#include "string.h"
}

#include "Ethernet3.h"  // Modern multi-instance support
#include "EthernetClient.h"
#include "EthernetServer.h"

// Backward compatibility includes
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
// Global Ethernet instance will be available via Ethernet3.h
#endif

// Constructors

#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
EthernetServer::EthernetServer(uint16_t port) {
    _port = port;
    _ethernet = nullptr;  // Will use global instance
}
#endif

EthernetServer::EthernetServer(uint16_t port, Ethernet3Class* ethernet_instance) {
    _port = port;
    _ethernet = ethernet_instance;
}

// Helper method for multi-instance support
Ethernet3Class* EthernetServer::getEthernetInstance() {
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
    return _ethernet
               ? _ethernet
               : &Ethernet;  // Fallback to global instance when backward compatibility enabled
#else
    return _ethernet;  // Must have been provided in constructor
#endif
}

void EthernetServer::begin() {
    Ethernet3Class* eth = getEthernetInstance();
    uint8_t maxSockets = eth->getMaxSockets();

    for (int sock = 0; sock < maxSockets; sock++) {
        EthernetClient client(sock);
        client = EthernetClient(eth);  // Associate with our Ethernet instance
        if (client.status() == SnSR::CLOSED) {
            eth->socket(sock, SnMR::TCP, _port, 0);
            eth->listen(sock);
            eth->_server_port[sock] = _port;
            break;
        }
    }
}

void EthernetServer::accept() {
    int listening = 0;
    Ethernet3Class* eth = getEthernetInstance();
    uint8_t maxSockets = eth->getMaxSockets();

    for (int sock = 0; sock < maxSockets; sock++) {
        EthernetClient client(eth);  // Associate with our Ethernet instance

        if (eth->_server_port[sock] == _port) {
            if (client.status() == SnSR::LISTEN) {
                listening = 1;
            } else if (client.status() == SnSR::CLOSE_WAIT && !client.available()) {
                client.stop();
            }
        }
    }

    if (!listening) {
        begin();
    }
}

EthernetClient EthernetServer::available() {
    accept();

    Ethernet3Class* eth = getEthernetInstance();
    uint8_t maxSockets = eth->getMaxSockets();

    for (int sock = 0; sock < maxSockets; sock++) {
        EthernetClient client(eth);  // Associate with our Ethernet instance
        if (eth->_server_port[sock] == _port &&
            (client.status() == SnSR::ESTABLISHED || client.status() == SnSR::CLOSE_WAIT)) {
            if (client.available()) {
                // XXX: don't always pick the lowest numbered socket.
                return client;
            }
        }
    }

    return EthernetClient(MAX_SOCK_NUM);
}

size_t EthernetServer::write(uint8_t b) { return write(&b, 1); }

size_t EthernetServer::write(const uint8_t* buffer, size_t size) {
    size_t n = 0;

    accept();

    Ethernet3Class* eth = getEthernetInstance();
    uint8_t maxSockets = eth->getMaxSockets();

    for (int sock = 0; sock < maxSockets; sock++) {
        EthernetClient client(eth);  // Associate with our Ethernet instance

        if (eth->_server_port[sock] == _port && client.status() == SnSR::ESTABLISHED) {
            n += client.write(buffer, size);
        }
    }

    return n;
}

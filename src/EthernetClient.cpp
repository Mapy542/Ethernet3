#include "chips/utility/socket.h"

extern "C" {
#include "string.h"
}

#include "Arduino.h"
#include "Dns.h"
#include "Ethernet3.h"  // Modern multi-instance support
#include "EthernetClient.h"
#include "EthernetServer.h"

// Backward compatibility includes
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
// Global Ethernet instance will be available via Ethernet3.h
#endif

// Constructors

#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
EthernetClient::EthernetClient() : _sock(MAX_SOCK_NUM), _srcport(1024), _ethernet(nullptr) {}

EthernetClient::EthernetClient(uint8_t sock) : _sock(sock), _srcport(1024), _ethernet(nullptr) {}
#endif

EthernetClient::EthernetClient(Ethernet3Class* ethernet_instance)
    : _sock(MAX_SOCK_NUM), _srcport(1024), _ethernet(ethernet_instance) {
    if (!ethernet_instance) {
        // Throw error or handle gracefully - instance is required
        // For Arduino, we'll just set to null and handle in getEthernetInstance
    }
}

// Helper methods for multi-instance support
Ethernet3Class* EthernetClient::getEthernetInstance() {
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
    return _ethernet
               ? _ethernet
               : &Ethernet;  // Fallback to global instance when backward compatibility enabled
#else
    return _ethernet;  // Must have been provided in constructor
#endif
}

uint8_t EthernetClient::getMaxSockets() {
    Ethernet3Class* eth = getEthernetInstance();
    return eth->getMaxSockets();
}

int EthernetClient::connect(const char* host, uint16_t port) {
    // Look up the host first
    int ret = 0;
    DNSClient dns;
    IPAddress remote_addr;

    Ethernet3Class* eth = getEthernetInstance();
    dns.begin(eth->dnsServerIP());
    ret = dns.getHostByName(host, remote_addr);
    if (ret == 1) {
        return connect(remote_addr, port);
    } else {
        return ret;
    }
}

int EthernetClient::connect(IPAddress ip, uint16_t port) {
    if (_sock != MAX_SOCK_NUM) return 0;

    Ethernet3Class* eth = getEthernetInstance();
    uint8_t maxSockets = getMaxSockets();

    for (int i = 0; i < maxSockets; i++) {
        uint8_t s = eth->getChip()->readSnSR(i);
        if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT || s == SnSR::CLOSE_WAIT) {
            _sock = i;
            break;
        }
    }

    if (_sock == maxSockets) return 0;

    _srcport++;
    if (_srcport == 0) _srcport = 1024;
    eth->socket(_sock, SnMR::TCP, _srcport, 0);

    if (!eth->connect(_sock, rawIPAddress(ip), port)) {
        _sock = MAX_SOCK_NUM;
        return 0;
    }

    while (status() != SnSR::ESTABLISHED) {
        delay(1);
        if (status() == SnSR::CLOSED) {
            _sock = MAX_SOCK_NUM;
            return 0;
        }
    }

    return 1;
}

size_t EthernetClient::write(uint8_t b) { return write(&b, 1); }

size_t EthernetClient::write(const uint8_t* buf, size_t size) {
    if (_sock == MAX_SOCK_NUM) {
        setWriteError();
        return 0;
    }
    Ethernet3Class* eth = getEthernetInstance();
    if (!eth->send(_sock, buf, size)) {
        setWriteError();
        return 0;
    }
    return size;
}

int EthernetClient::available() {
    if (_sock != MAX_SOCK_NUM) {
        Ethernet3Class* eth = getEthernetInstance();
        return eth->getChip()->getRXReceivedSize(_sock);
    }
    return 0;
}

int EthernetClient::read() {
    uint8_t b;
    Ethernet3Class* eth = getEthernetInstance();
    if (eth->recv(_sock, &b, 1) > 0) {
        // recv worked
        return b;
    } else {
        // No data available
        return -1;
    }
}

int EthernetClient::read(uint8_t* buf, size_t size) {
    Ethernet3Class* eth = getEthernetInstance();
    return eth->recv(_sock, buf, size);
}

int EthernetClient::peek() {
    uint8_t b;
    // Unlike recv, peek doesn't check to see if there's any data available, so we must
    if (!available()) return -1;
    Ethernet3Class* eth = getEthernetInstance();
    eth->peek(_sock, &b);
    return b;
}

void EthernetClient::flush() {
    Ethernet3Class* eth = getEthernetInstance();
    eth->flush(_sock);
}

void EthernetClient::stop() {
    if (_sock == MAX_SOCK_NUM) return;

    Ethernet3Class* eth = getEthernetInstance();
    // attempt to close the connection gracefully (send a FIN to other side)
    eth->disconnect(_sock);
    unsigned long start = millis();

    // wait a second for the connection to close
    while (status() != SnSR::CLOSED && millis() - start < 1000) delay(1);

    // if it hasn't closed, close it forcefully
    if (status() != SnSR::CLOSED) eth->close(_sock);

    eth->_server_port[_sock] = 0;
    _sock = MAX_SOCK_NUM;
}

uint8_t EthernetClient::connected() {
    if (_sock == MAX_SOCK_NUM) return 0;

    uint8_t s = status();
    return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
             (s == SnSR::CLOSE_WAIT && !available()));
}

uint8_t EthernetClient::status() {
    if (_sock == MAX_SOCK_NUM) return SnSR::CLOSED;
    Ethernet3Class* eth = getEthernetInstance();
    return eth->getChip()->readSnSR(_sock);
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.

EthernetClient::operator bool() { return _sock != MAX_SOCK_NUM; }

bool EthernetClient::operator==(const EthernetClient& rhs) {
    return _sock == rhs._sock && _sock != MAX_SOCK_NUM && rhs._sock != MAX_SOCK_NUM;
}

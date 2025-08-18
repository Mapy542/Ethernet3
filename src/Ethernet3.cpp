/*
 * Ethernet3.cpp - Enhanced Ethernet class implementation with multi-instance support
 */

#include "Ethernet3.h"

#include "Dhcp.h"
#include "EthernetClient.h"
#include "EthernetServer.h"
#include "chips/utility/wiznet_registers.h"
#include "hal/ArduinoPlatform.h"

// Conditional backward compatibility - only create global instances when needed
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
static ArduinoPlatform global_platform;
static W5500Chip global_w5500_chip(&global_platform, 10);
Ethernet3Class Ethernet(&global_w5500_chip, false);
#endif

// Constructors

#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
// Default constructor for backward compatibility
Ethernet3Class::Ethernet3Class()
    : _dhcp(nullptr),
      _chip(&global_w5500_chip),
      _platform(&global_platform),
      _state(nullptr),
      _server_port(nullptr),
      _max_sockets(global_w5500_chip.maxSockets()),
      _cs_pin(10),
      _owns_chip(false) {
    initializeInstance();
}
#endif

// Constructor with specific chip instance
Ethernet3Class::Ethernet3Class(EthernetChip* chip, bool owns_chip)
    : _dhcp(nullptr),
      _chip(chip),
      _platform(nullptr),
      _state(nullptr),
      _server_port(nullptr),
      _owns_chip(owns_chip) {
    if (_chip) {
        _max_sockets = _chip->maxSockets();
        _cs_pin = _chip->getCSPin();
        _platform = _chip->getPlatform();
    } else {
        _max_sockets = 8;
        _cs_pin = 10;
    }

    initializeInstance();
}

// Constructor for creating new chip instances
Ethernet3Class::Ethernet3Class(uint8_t chip_type, uint8_t cs_pin,
                               EthernetPlatform* platform_interface)
    : _dhcp(nullptr),
      _chip(nullptr),
      _state(nullptr),
      _server_port(nullptr),
      _cs_pin(cs_pin),
      _owns_chip(true) {
    // Create platform if not provided
    if (!platform_interface) {
        _platform = new ArduinoPlatform();
    } else {
        _platform = platform_interface;
    }

    // Create appropriate chip instance
    if (chip_type == CHIP_TYPE_W5100) {
        _chip = new W5100Chip(_platform, cs_pin);
    } else {  // Default to W5500
        _chip = new W5500Chip(_platform, cs_pin);
    }
    _max_sockets = _chip->maxSockets();

    initializeInstance();
}

Ethernet3Class::~Ethernet3Class() { cleanup(); }

void Ethernet3Class::initializeInstance() {
    // Allocate per-instance arrays
    _state = new uint8_t[_max_sockets];
    _server_port = new uint16_t[_max_sockets];

    // Initialize arrays
    for (uint8_t i = 0; i < _max_sockets; i++) {
        _state[i] = 0;
        _server_port[i] = 0;
    }

    _dnsServerAddress = IPAddress(0, 0, 0, 0);
    _dnsDomainName = nullptr;
    _hostName = nullptr;
}

void Ethernet3Class::cleanup() {
    if (_dhcp) {
        delete _dhcp;
        _dhcp = nullptr;
    }

    if (_state) {
        delete[] _state;
        _state = nullptr;
    }

    if (_server_port) {
        delete[] _server_port;
        _server_port = nullptr;
    }

    if (_owns_chip) {
        if (_chip) {
            delete _chip;
            _chip = nullptr;
        }
        if (_platform) {
            delete _platform;
            _platform = nullptr;
        }
    }
}

void Ethernet3Class::init(uint8_t cs_pin) {
    _cs_pin = cs_pin;
    if (_chip) {
        (void)_chip->init();
    }
}

bool Ethernet3Class::linkActive() {
    if (_chip) {
        return _chip->linkActive();
    }
    return false;
}

uint8_t Ethernet3Class::hardwareStatus() {
    if (!_chip) {
        return EthernetNoHardware;
    }

    uint8_t chip_type = _chip->getChipType();
    if (chip_type == CHIP_TYPE_W5100) {
        return EthernetW5100;
    } else if (chip_type == CHIP_TYPE_W5500) {
        return EthernetW5500;
    }

    return EthernetNoHardware;
}

uint8_t Ethernet3Class::linkStatus() {
    if (linkActive()) {
        return LinkON;
    } else {
        return LinkOFF;
    }
}

// Per-instance socket state management
uint8_t Ethernet3Class::getSocketState(uint8_t sock) const {
    if (sock < _max_sockets && _state) {
        return _state[sock];
    }
    return 0;
}

void Ethernet3Class::setSocketState(uint8_t sock, uint8_t state) {
    if (sock < _max_sockets && _state) {
        _state[sock] = state;
    }
}

uint16_t Ethernet3Class::getServerPort(uint8_t sock) const {
    if (sock < _max_sockets && _server_port) {
        return _server_port[sock];
    }
    return 0;
}

void Ethernet3Class::setServerPort(uint8_t sock, uint16_t port) {
    if (sock < _max_sockets && _server_port) {
        _server_port[sock] = port;
    }
}

// The rest of the methods delegate to the existing Ethernet2 implementation
// For brevity, I'll implement key methods and indicate where others would go

int Ethernet3Class::begin(uint8_t* mac_address) {
    if (!_chip) return 0;
    if (_dhcp) {
        delete _dhcp;
    }
    _dhcp = new DhcpClass();
    _chip->init();
    _chip->setMACAddress(mac_address);
    uint8_t zero_ip[4] = {0, 0, 0, 0};
    _chip->setIPAddress(zero_ip);
    int ret = _dhcp->beginWithDHCP(mac_address);
    if (ret == 1) {
        _chip->setIPAddress(&(_dhcp->getLocalIp()[0]));
        _chip->setGatewayIp(&(_dhcp->getGatewayIp()[0]));
        _chip->setSubnetMask(&(_dhcp->getSubnetMask()[0]));
        _dnsServerAddress = _dhcp->getDnsServerIp();
        _dnsDomainName = _dhcp->getDnsDomainName();
        _hostName = _dhcp->getHostName();
    }
    return ret;
}

void Ethernet3Class::begin(uint8_t* mac_address, IPAddress local_ip) {
    if (!_chip) return;
    _chip->init();
    _chip->setMACAddress(mac_address);
    _chip->setIPAddress(&(local_ip[0]));
}

int Ethernet3Class::maintain() {
    if (_dhcp != nullptr) {
        return _dhcp->checkLease();
    }
    return 0;
}

IPAddress Ethernet3Class::localIP() {
    IPAddress ret;
    if (_chip) _chip->getIPAddress(&ret[0]);
    return ret;
}

IPAddress Ethernet3Class::subnetMask() {
    IPAddress ret;
    if (_chip) _chip->getSubnetMask(&ret[0]);
    return ret;
}

IPAddress Ethernet3Class::gatewayIP() {
    IPAddress ret;
    if (_chip) _chip->getGatewayIp(&ret[0]);
    return ret;
}

IPAddress Ethernet3Class::dnsServerIP() { return _dnsServerAddress; }

char* Ethernet3Class::dnsDomainName() { return _dnsDomainName; }

char* Ethernet3Class::hostName() { return _hostName; }

// ---------------------------------------------------------------------------
// Internal unified socket management implementation
// ---------------------------------------------------------------------------

uint8_t Ethernet3Class::allocateSocket() {
    for (uint8_t i = 0; i < _max_sockets; i++) {
        if (_state[i] == 0 || _chip->readSocketStatus(i) == WIZ_SnSR_CLOSED) {
            _state[i] = 1;  // mark allocated
            return i;
        }
    }
    return 0xFF;
}

void Ethernet3Class::releaseSocket(uint8_t sock) {
    if (sock < _max_sockets) {
        _state[sock] = 0;
    }
}

uint8_t Ethernet3Class::openSocket(uint8_t protocolMode, uint16_t localPort, uint8_t flags) {
    if (!_chip) return 0xFF;
    uint8_t s = allocateSocket();
    if (s == 0xFF) return 0xFF;
    if (localPort == 0) {
        // allocate ephemeral port
        if (_next_ephemeral_port < 49152) _next_ephemeral_port = 49152;
        localPort = _next_ephemeral_port++;
        if (_next_ephemeral_port == 65535) _next_ephemeral_port = 49152;
    }
    _chip->setSocketMode(s, (uint8_t)(protocolMode | flags));
    _chip->setSocketSourcePort(s, localPort);
    _chip->execSocketCommand(s, Sock_OPEN);
    return s;
}

void Ethernet3Class::closeSocket(uint8_t sock) {
    if (!_chip || sock >= _max_sockets) return;
    _chip->execSocketCommand(sock, Sock_CLOSE);
    _chip->writeSocketInterrupt(sock, 0xFF);  // clear interrupts
    releaseSocket(sock);
}

bool Ethernet3Class::listenSocket(uint8_t sock) {
    if (!_chip || sock >= _max_sockets) return false;
    if (_chip->readSocketStatus(sock) != WIZ_SnSR_INIT) return false;
    _chip->execSocketCommand(sock, Sock_LISTEN);
    return true;
}

bool Ethernet3Class::connectSocket(uint8_t sock, const uint8_t* ip, uint16_t port) {
    if (!_chip || sock >= _max_sockets) return false;
    _chip->setSocketDestination(sock, ip, port);
    _chip->execSocketCommand(sock, Sock_CONNECT);
    return true;
}

uint16_t Ethernet3Class::sendSocket(uint8_t sock, const uint8_t* data, uint16_t len) {
    if (!_chip || sock >= _max_sockets) return 0;
    if (len == 0) return 0;
    uint16_t sendLen = len;
    uint16_t space;
    while ((space = _chip->getTXFreeSize(sock)) < sendLen) {
        uint8_t st = _chip->readSocketStatus(sock);
        if (st != WIZ_SnSR_ESTABLISHED && st != WIZ_SnSR_CLOSE_WAIT) return 0;
        // busy wait - could add timeout
    }
    _chip->writeSocketData(sock, data, sendLen);
    _chip->commitTX(sock);
    // wait for SEND_OK or closure
    while (true) {
        uint8_t ir = _chip->readSocketInterrupt(sock);
        if (ir & WIZ_SnIR_SEND_OK) {
            _chip->writeSocketInterrupt(sock, WIZ_SnIR_SEND_OK);
            break;
        }
        uint8_t st = _chip->readSocketStatus(sock);
        if (st == WIZ_SnSR_CLOSED) return 0;  // failed
    }
    return sendLen;
}

uint16_t Ethernet3Class::recvSocket(uint8_t sock, uint8_t* data, uint16_t len) {
    if (!_chip || sock >= _max_sockets) return 0;
    uint16_t avail = _chip->getRXReceivedSize(sock);
    if (avail == 0) return 0;
    if (avail < len) len = avail;
    uint16_t rd = _chip->getSocketRXReadPointer(sock);
    _chip->readSocketData(sock, rd, data, len, true);  // peek read at current pointer
    rd += len;
    _chip->setSocketRXReadPointer(sock, rd);
    _chip->execSocketCommand(sock, Sock_RECV);  // notify chip new data consumed
    return len;
}

bool Ethernet3Class::startUDPPacket(uint8_t sock, const uint8_t* ip, uint16_t port) {
    if (!_chip || sock >= _max_sockets) return false;
    _chip->setSocketDestination(sock, ip, port);
    return true;
}

uint16_t Ethernet3Class::bufferUDPData(uint8_t sock, uint16_t offset, const uint8_t* data,
                                       uint16_t len) {
    if (!_chip || sock >= _max_sockets) return 0;
    _chip->writeSocketDataOffset(sock, offset, data, len);
    return len;
}

bool Ethernet3Class::sendUDPPacket(uint8_t sock, uint16_t totalLen) {
    if (!_chip || sock >= _max_sockets) return false;
    _chip->advanceTX(sock, totalLen);
    _chip->commitTX(sock);
    while (true) {
        uint8_t ir = _chip->readSocketInterrupt(sock);
        if (ir & WIZ_SnIR_SEND_OK) {
            _chip->writeSocketInterrupt(sock, WIZ_SnIR_SEND_OK);
            return true;
        }
        uint8_t st = _chip->readSocketStatus(sock);
        if (st == WIZ_SnSR_CLOSED) return false;
    }
}


extern "C" {
#include "string.h"
}

#include "EthernetClient.h"

uint16_t EthernetClient::_srcport = 1024;  // default ephemeral port?

EthernetClient::EthernetClient(EthernetClass* eth, EthernetChip* chip)
    : _ethernet(eth), _chip(chip), _sock(MAX_SOCK_NUM) {}

EthernetClient::EthernetClient(EthernetClass* eth, EthernetChip* chip, uint8_t sock)
    : _ethernet(eth), _chip(chip), _sock(sock) {}

int EthernetClient::connect(const char* host, uint16_t port) {
    // Look up the host first
    int ret = 0;
    DNSClient dns(_ethernet, _chip);
    IPAddress remote_addr;

    dns.begin(_ethernet->dnsServerIP());
    ret = dns.getHostByName(host, remote_addr);
    if (ret == 1) {
        return connect(remote_addr, port);
    } else {
        return ret;
    }
}

int EthernetClient::connect(IPAddress ip, uint16_t port) {
    if (_sock != MAX_SOCK_NUM) return 0;

    for (int i = 0; i < MAX_SOCK_NUM; i++) {
        uint8_t s = _chip->readSnSR(i);
        if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT || s == SnSR::CLOSE_WAIT) {
            _sock = i;
            break;
        }
    }

    if (_sock == MAX_SOCK_NUM) return 0;

    _srcport++;
    if (_srcport == 0) _srcport = 1024;
    socket(_chip, _sock, SnMR::TCP, _srcport, 0);

    if (!::connect(_chip, _sock, rawIPAddress(ip), port)) {
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
    if (!send(_chip, _sock, buf, size)) {
        setWriteError();
        return 0;
    }
    return size;
}

int EthernetClient::available() {
    if (_sock != MAX_SOCK_NUM) return _chip->getRXReceivedSize(_sock);
    return 0;
}

int EthernetClient::read() {
    uint8_t b;
    if (recv(_chip, _sock, &b, 1) > 0) {
        // recv worked
        return b;
    } else {
        // No data available
        return -1;
    }
}

int EthernetClient::read(uint8_t* buf, size_t size) { return recv(_chip, _sock, buf, size); }

int EthernetClient::peek() {
    uint8_t b;
    // Unlike recv, peek doesn't check to see if there's any data available, so we must
    if (!available()) return -1;
    ::peek(_chip, _sock, &b);
    return b;
}

void EthernetClient::flush() { ::flush(_chip, _sock); }

void EthernetClient::stop() {
    if (_sock == MAX_SOCK_NUM) return;

    // attempt to close the connection gracefully (send a FIN to other side)
    disconnect(_chip, _sock);
    unsigned long start = millis();

    // wait a second for the connection to close
    while (status() != SnSR::CLOSED && millis() - start < 1000) delay(1);

    // if it hasn't closed, close it forcefully
    if (status() != SnSR::CLOSED) close(_chip, _sock);

    _ethernet->_server_port[_sock] = 0;
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
    return _chip->readSnSR(_sock);
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.

EthernetClient::operator bool() { return _sock != MAX_SOCK_NUM; }

bool EthernetClient::operator==(const EthernetClient& rhs) {
    return _sock == rhs._sock && _sock != MAX_SOCK_NUM && rhs._sock != MAX_SOCK_NUM;
}

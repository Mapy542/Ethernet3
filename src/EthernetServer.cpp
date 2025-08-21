
extern "C" {
#include "string.h"
}

#include "EthernetServer.h"

EthernetServer::EthernetServer(EthernetClass* eth, EthernetChip* chip, uint16_t port)
    : _ethernet(eth), _chip(chip), _port(port) {}

void EthernetServer::begin() {
    for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
        EthernetClient client(_ethernet, _chip, sock);
        if (client.status() == SnSR::CLOSED) {
            socket(_chip, sock, SnMR::TCP, _port, 0);
            listen(_chip, sock);
            EthernetClass::_server_port[sock] = _port;
            break;
        }
    }
}

void EthernetServer::accept() {
    int listening = 0;

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
        EthernetClient client(_ethernet, _chip, sock);

        if (EthernetClass::_server_port[sock] == _port) {
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

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
        EthernetClient client(_ethernet, _chip, sock);
        if (EthernetClass::_server_port[sock] == _port &&
            (client.status() == SnSR::ESTABLISHED || client.status() == SnSR::CLOSE_WAIT)) {
            if (client.available()) {
                // XXX: don't always pick the lowest numbered socket.
                return client;
            }
        }
    }

    return EthernetClient(_ethernet, _chip, MAX_SOCK_NUM);
}

size_t EthernetServer::write(uint8_t b) { return write(&b, 1); }

size_t EthernetServer::write(const uint8_t* buffer, size_t size) {
    size_t n = 0;

    accept();

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
        EthernetClient client(_ethernet, _chip, sock);

        if (EthernetClass::_server_port[sock] == _port && client.status() == SnSR::ESTABLISHED) {
            n += client.write(buffer, size);
        }
    }

    return n;
}

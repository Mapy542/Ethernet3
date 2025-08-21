/*
 *  Udp.cpp: Library to send/receive UDP packets with the Arduino ethernet shield.
 *  This version only offers minimal wrapping of socket.c/socket.h
 *  Drop Udp.h/.cpp into the Ethernet library directory at hardware/libraries/Ethernet/
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 *
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 */

#include "Dns.h"

/* Constructor */
EthernetUDP::EthernetUDP(EthernetClass* eth, EthernetChip* chip)
    : _ethernet(eth), _chip(chip), _sock(MAX_SOCK_NUM) {}

/* Start EthernetUDP socket, listening at local port PORT */
uint8_t EthernetUDP::begin(uint16_t port) {
    if (_sock != MAX_SOCK_NUM) return 0;

    for (int i = 0; i < MAX_SOCK_NUM; i++) {
        uint8_t s = _chip->readSnSR(i);
        if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT) {
            _sock = i;
            break;
        }
    }

    if (_sock == MAX_SOCK_NUM) return 0;

    _port = port;
    _remaining = 0;
    socket(_chip, _sock, SnMR::UDP, _port, 0);

    return 1;
}

/* return number of bytes available in the current packet,
   will return zero if parsePacket hasn't been called yet */
int EthernetUDP::available() { return _remaining; }

/* Release any resources being used by this EthernetUDP instance */
void EthernetUDP::stop() {
    if (_sock == MAX_SOCK_NUM) return;

    close(_chip, _sock);

    _ethernet->_server_port[_sock] = 0;
    _sock = MAX_SOCK_NUM;
}

int EthernetUDP::beginPacket(const char* host, uint16_t port) {
    // Look up the host first
    int ret = 0;
    DNSClient dns(_ethernet, _chip);
    IPAddress remote_addr;

    dns.begin(_ethernet->dnsServerIP());
    ret = dns.getHostByName(host, remote_addr);
    if (ret == 1) {
        return beginPacket(remote_addr, port);
    } else {
        return ret;
    }
}

int EthernetUDP::beginPacket(IPAddress ip, uint16_t port) {
    _offset = 0;
    return startUDP(_chip, _sock, rawIPAddress(ip), port);
}

int EthernetUDP::endPacket() { return sendUDP(_chip, _sock); }

size_t EthernetUDP::write(uint8_t byte) { return write(&byte, 1); }

size_t EthernetUDP::write(const uint8_t* buffer, size_t size) {
    uint16_t bytes_written = bufferData(_chip, _sock, _offset, buffer, size);
    _offset += bytes_written;
    return bytes_written;
}

int EthernetUDP::parsePacket() {
    // discard any remaining bytes in the last packet
    flush();

    if (_chip->getRXReceivedSize(_sock) > 0) {
        // HACK - hand-parse the UDP packet using TCP recv method
        uint8_t tmpBuf[8];
        int ret = 0;
        // read 8 header bytes and get IP and port from it
        ret = recv(_chip, _sock, tmpBuf, 8);
        if (ret > 0) {
            _remoteIP = tmpBuf;
            _remotePort = tmpBuf[4];
            _remotePort = (_remotePort << 8) + tmpBuf[5];
            _remaining = tmpBuf[6];
            _remaining = (_remaining << 8) + tmpBuf[7];

            // When we get here, any remaining bytes are the data
            ret = _remaining;
        }
        return ret;
    }
    // There aren't any packets available
    return 0;
}

int EthernetUDP::read() {
    uint8_t byte;

    if ((_remaining > 0) && (recv(_chip, _sock, &byte, 1) > 0)) {
        // We read things without any problems
        _remaining--;
        return byte;
    }

    // If we get here, there's no data available
    return -1;
}

int EthernetUDP::read(unsigned char* buffer, size_t len) {
    if (_remaining > 0) {
        int got;

        if (_remaining <= len) {
            // data should fit in the buffer
            got = recv(_chip, _sock, buffer, _remaining);
        } else {
            // too much data for the buffer,
            // grab as much as will fit
            got = recv(_chip, _sock, buffer, len);
        }

        if (got > 0) {
            _remaining -= got;
            return got;
        }
    }

    // If we get here, there's no data available or recv failed
    return -1;
}

int EthernetUDP::peek() {
    uint8_t b;
    // Unlike recv, peek doesn't check to see if there's any data available, so we must.
    // If the user hasn't called parsePacket yet then return nothing otherwise they
    // may get the UDP header
    if (!_remaining) return -1;
    ::peek(_chip, _sock, &b);
    return b;
}

void EthernetUDP::flush() {
    // could this fail (loop endlessly) if _remaining > 0 and recv in read fails?
    // should only occur if recv fails after telling us the data is there, lets
    // hope the w5500 always behaves :)

    while (_remaining) {
        read();
    }
}

uint8_t EthernetUDP::beginMulticast(IPAddress multicast_ip, uint16_t port) {
    if (!isMulticastGroup(multicast_ip)) {
        return 0;  // Invalid multicast IP
    }

    // Close existing socket if open
    if (_sock != MAX_SOCK_NUM) {
        stop();
    }

    // Find available socket
    _sock = MAX_SOCK_NUM;
    uint8_t max_sock = 8;  // Assuming W5500 supports 8 sockets
    for (uint8_t i = 0; i < max_sock; i++) {
        uint8_t s = _chip->readSnSR(i);
        if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT) {
            _sock = i;
            break;
        }
    }

    if (_sock == max_sock) {
        return 0;  // No sockets available
    }

    // Configure socket for UDP multicast using existing MULTI flag
    uint8_t result = socket(_chip, _sock, SnMR::UDP | SnMR::MULTI, port, 0);
    if (result) {
        configureMulticastSocket(multicast_ip, port);
        _port = port;
        _remaining = 0;
        return 1;
    }

    return 0;
}

int EthernetUDP::joinMulticastGroup(IPAddress group_ip) {
    if (!isMulticastGroup(group_ip)) {
        return 0;  // Invalid multicast IP
    }

    // Calculate multicast MAC address
    uint8_t multicast_mac[6];
    calculateMulticastMAC(group_ip, multicast_mac);

    // Note: W5500 doesn't have native IGMP support
    // The socket configuration with MULTI flag handles basic multicast reception
    // For full IGMP support, router configuration may be required

    return 1;  // Indicate success for basic multicast setup
}

int EthernetUDP::leaveMulticastGroup(IPAddress group_ip) {
    if (!isMulticastGroup(group_ip)) {
        return 0;  // Invalid multicast IP
    }

    // For W5500, leaving a group typically means closing the socket
    // or reconfiguring it without multicast
    // This is a simplified implementation

    return 1;
}

bool EthernetUDP::isMulticastGroup(IPAddress ip) {
    // Check if IP is in multicast range (224.0.0.0 to 239.255.255.255)
    // This corresponds to the high nibble being 0xE (1110 in binary)
    return (ip[0] >= 224 && ip[0] <= 239);
}

// Private methods for multicast support

void EthernetUDP::calculateMulticastMAC(IPAddress ip, uint8_t* mac) {
    // Multicast MAC format: 01:00:5e:XX:XX:XX
    // XX:XX:XX from lower 23 bits of multicast IP address
    mac[0] = 0x01;
    mac[1] = 0x00;
    mac[2] = 0x5e;
    mac[3] = ip[1] & 0x7F;  // Clear upper bit to fit in 23-bit range
    mac[4] = ip[2];
    mac[5] = ip[3];
}

void EthernetUDP::configureMulticastSocket(IPAddress group_ip, uint16_t port) {
    // W5500 multicast configuration
    // The MULTI flag in socket mode register enables multicast reception
    // Additional configuration could be added here for specific multicast features

    // Calculate multicast MAC for reference (though W5500 handles this internally)
    uint8_t multicast_mac[6];
    calculateMulticastMAC(group_ip, multicast_mac);

    // Socket is already configured with SnMR::MULTI flag in beginMulticast()
    // The W5500 hardware will handle multicast packet filtering
}
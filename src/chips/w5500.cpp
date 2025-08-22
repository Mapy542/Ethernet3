#include "w5500.h"

/*
 * Copyright (c) 2010 by WIZnet <support@wiznet.co.kr>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 *
 * - 21 Aug. 2025
 * Added W5500 class for new chip support
 * by Eli
 */

void W5500::getGatewayIp(uint8_t *_addr) { readGAR(_addr); }

void W5500::setGatewayIp(uint8_t *_addr) { writeGAR(_addr); }

void W5500::getSubnetMask(uint8_t *_addr) { readSUBR(_addr); }

void W5500::setSubnetMask(uint8_t *_addr) { writeSUBR(_addr); }

void W5500::getMACAddress(uint8_t *_addr) { readSHAR(_addr); }

void W5500::setMACAddress(uint8_t *_addr) { writeSHAR(_addr); }

void W5500::getIPAddress(uint8_t *_addr) { readSIPR(_addr); }

void W5500::setIPAddress(uint8_t *_addr) { writeSIPR(_addr); }

void W5500::setRetransmissionTime(uint16_t _timeout) { writeRTR(_timeout); }

void W5500::setRetransmissionCount(uint8_t _retry) { writeRCR(_retry); }

void W5500::setPHYCFGR(uint8_t _val) { writePHYCFGR(_val); }

uint8_t W5500::getPHYCFGR() {
    //  readPHYCFGR();
    return read(0x002E, 0x00);
}

void W5500::swReset() { writeMR((readMR() | 0x80)); }

uint8_t W5500::readSn(SOCKET _s, uint16_t _addr) {
    uint8_t cntl_byte = (_s << 5) + 0x08;
    return read(_addr, cntl_byte);
}

uint8_t W5500::writeSn(SOCKET _s, uint16_t _addr, uint8_t _data) {
    uint8_t cntl_byte = (_s << 5) + 0x0C;
    return write(_addr, cntl_byte, _data);
}

uint16_t W5500::readSn(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
    uint8_t cntl_byte = (_s << 5) + 0x08;
    return read(_addr, cntl_byte, _buf, _len);
}

uint16_t W5500::writeSn(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
    uint8_t cntl_byte = (_s << 5) + 0x0C;
    return write(_addr, cntl_byte, _buf, _len);
}

bool W5500::init() {
    initSS();
    SPI.begin();
    this->swReset();
    for (int i = 0; i < this->maxSockets(); i++) {
        uint8_t cntl_byte = (0x0C + (i << 5));
        write(0x1E, cntl_byte, 2);  // 0x1E - Sn_RXBUF_SIZE
        write(0x1F, cntl_byte, 2);  // 0x1F - Sn_TXBUF_SIZE
    }
    return true;  // assuming initialization is always successful
}

bool W5500::linkActive() {
    return getPHYCFGR() & W5500PHYCFGR::LNK_ON;  // Check if link is active
}

uint16_t W5500::getTXFreeSize(SOCKET s) {
    uint16_t val = 0, val1 = 0;
    do {
        val1 = readSnTX_FSR(s);
        if (val1 != 0) val = readSnTX_FSR(s);
    } while (val != val1);
    return val;
}

uint16_t W5500::getRXReceivedSize(SOCKET s) {
    uint16_t val = 0, val1 = 0;
    do {
        val1 = readSnRX_RSR(s);
        if (val1 != 0) val = readSnRX_RSR(s);
    } while (val != val1);
    return val;
}

void W5500::send_data_processing(SOCKET s, const uint8_t *data, uint16_t len) {
    // This is same as having no offset in a call to send_data_processing_offset
    send_data_processing_offset(s, 0, data, len);
}

void W5500::send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data,
                                        uint16_t len) {
    uint16_t ptr = readSnTX_WR(s);
    uint8_t cntl_byte = (0x14 + (s << 5));
    ptr += data_offset;
    write(ptr, cntl_byte, data, len);
    ptr += len;
    writeSnTX_WR(s, ptr);
}

void W5500::recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek) {
    uint16_t ptr;
    ptr = readSnRX_RD(s);

    read_data(s, ptr, data, len);
    if (!peek) {
        ptr += len;
        writeSnRX_RD(s, ptr);
    }
}

void W5500::read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len) {
    uint8_t cntl_byte = (0x18 + (s << 5));
    read((uint16_t)src, cntl_byte, (uint8_t *)dst, len);
}

uint8_t W5500::write(uint16_t _addr, uint8_t _cb, uint8_t _data) {
    SPI.beginTransaction(wiznet_SPI_settings);
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    SPI.transfer(_data);
    resetSS();
    SPI.endTransaction();

    return 1;
}

uint16_t W5500::write(uint16_t _addr, uint8_t _cb, const uint8_t *_buf, uint16_t _len) {
    SPI.beginTransaction(wiznet_SPI_settings);
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    for (uint16_t i = 0; i < _len; i++) {
        SPI.transfer(_buf[i]);
    }
    resetSS();
    SPI.endTransaction();

    return _len;
}

uint8_t W5500::read(uint16_t _addr, uint8_t _cb) {
    SPI.beginTransaction(wiznet_SPI_settings);
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    uint8_t _data = SPI.transfer(0);
    resetSS();
    SPI.endTransaction();

    return _data;
}

uint16_t W5500::read(uint16_t _addr, uint8_t _cb, uint8_t *_buf, uint16_t _len) {
    SPI.beginTransaction(wiznet_SPI_settings);
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    for (uint16_t i = 0; i < _len; i++) {
        _buf[i] = SPI.transfer(0);
    }
    resetSS();
    SPI.endTransaction();

    return _len;
}

void W5500::execCmdSn(SOCKET s, SockCMD _cmd) {
    // Send command to socket
    writeSnCR(s, _cmd);
    // Wait for command to complete
    while (readSnCR(s));
}

uint8_t W5500::getChipType() {
    // Read the chip type from the MR register
    return readMR() & 0x07;  // Mask to get only the chip type bits
}
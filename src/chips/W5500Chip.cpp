/*
 * W5500Chip.cpp - Implementation of W5500Chip for Ethernet3
 */

#include "W5500Chip.h"

// Static SPI settings definition
SPISettings W5500Chip::spi_settings(8000000, MSBFIRST, SPI_MODE0);

// -----------------------------------------------------------------------------
// Low-level primitive SPI register access (common + socket + buffer)
// -----------------------------------------------------------------------------
uint8_t W5500Chip::writeReg(uint16_t addr, uint8_t control, uint8_t data) {
    SPI.beginTransaction(spi_settings);
    select();
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(control);
    SPI.transfer(data);
    deselect();
    SPI.endTransaction();
    return 1;
}

uint16_t W5500Chip::writeReg(uint16_t addr, uint8_t control, const uint8_t* buf, uint16_t len) {
    SPI.beginTransaction(spi_settings);
    select();
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(control);
    for (uint16_t i = 0; i < len; i++) {
        SPI.transfer(buf[i]);
    }
    deselect();
    SPI.endTransaction();
    return len;
}

uint8_t W5500Chip::readReg(uint16_t addr, uint8_t control) {
    SPI.beginTransaction(spi_settings);
    select();
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(control);
    uint8_t data = SPI.transfer(0);
    deselect();
    SPI.endTransaction();
    return data;
}

uint16_t W5500Chip::readReg(uint16_t addr, uint8_t control, uint8_t* buf, uint16_t len) {
    SPI.beginTransaction(spi_settings);
    select();
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);
    SPI.transfer(control);
    for (uint16_t i = 0; i < len; i++) {
        buf[i] = SPI.transfer(0);
    }
    deselect();
    SPI.endTransaction();
    return len;
}

// -----------------------------------------------------------------------------
// Core interface implementation
// -----------------------------------------------------------------------------
bool W5500Chip::init() {
    pinMode(cs_pin, OUTPUT);
    deselect();
    SPI.begin();
    delay(100);  // Allow chip power-up stabilization
    swReset();

    // Configure default TX/RX buffer sizes (2KB each like legacy driver)
    for (uint8_t s = 0; s < W5500_MAX_SOCK_NUM; s++) {
        uint8_t ctl = (uint8_t)(CB_SOCK_WRITE | (s << 5));
        // Sn_RXBUF_SIZE register (0x1E), Sn_TXBUF_SIZE register (0x1F)
        writeReg(0x1E, ctl, (uint8_t)2);
        writeReg(0x1F, ctl, (uint8_t)2);
    }
    return true;
}

bool W5500Chip::linkActive() {
    // PHYCFGR bit 0 (LNK) indicates link status
    return (getPHYCFGR() & 0x01) != 0;
}

void W5500Chip::swReset() {
    // Write MR with RST bit set, register address 0x0000
    uint8_t mr = readReg(WIZ_MR, CB_COMMON_READ);
    writeReg(WIZ_MR, CB_COMMON_WRITE, (uint8_t)(mr | WIZ_MR_RST));
    delay(2);
}

// -----------------------------------------------------------------------------
// Network configuration accessors
// -----------------------------------------------------------------------------
void W5500Chip::setGatewayIp(const uint8_t* addr) { writeReg(WIZ_GAR, CB_COMMON_WRITE, addr, 4); }
void W5500Chip::getGatewayIp(uint8_t* addr) { readReg(WIZ_GAR, CB_COMMON_READ, addr, 4); }
void W5500Chip::setSubnetMask(const uint8_t* addr) { writeReg(WIZ_SUBR, CB_COMMON_WRITE, addr, 4); }
void W5500Chip::getSubnetMask(uint8_t* addr) { readReg(WIZ_SUBR, CB_COMMON_READ, addr, 4); }
void W5500Chip::setMACAddress(const uint8_t* addr) { writeReg(WIZ_SHAR, CB_COMMON_WRITE, addr, 6); }
void W5500Chip::getMACAddress(uint8_t* addr) { readReg(WIZ_SHAR, CB_COMMON_READ, addr, 6); }
void W5500Chip::setIPAddress(const uint8_t* addr) { writeReg(WIZ_SIPR, CB_COMMON_WRITE, addr, 4); }
void W5500Chip::getIPAddress(uint8_t* addr) { readReg(WIZ_SIPR, CB_COMMON_READ, addr, 4); }
void W5500Chip::setRetransmissionTime(uint16_t timeout) {
    uint8_t b[2] = {(uint8_t)(timeout >> 8), (uint8_t)(timeout & 0xFF)};
    writeReg(W5500_RTR, CB_COMMON_WRITE, b, 2);
}
void W5500Chip::setRetransmissionCount(uint8_t retry) {
    writeReg(W5500_RCR, CB_COMMON_WRITE, retry);
}

void W5500Chip::setPHYCFGR(uint8_t val) { writeReg(W5500_PHYCFGR, CB_COMMON_WRITE, val); }
uint8_t W5500Chip::getPHYCFGR() { return readReg(W5500_PHYCFGR, CB_COMMON_READ); }

// -----------------------------------------------------------------------------
// Socket operations
// -----------------------------------------------------------------------------
void W5500Chip::execSocketCommand(uint8_t socket, uint8_t cmd) {
    writeSn(socket, WIZ_Sn_CR, cmd);
    // Wait for command register to clear
    while (readSn(socket, WIZ_Sn_CR)) {
        // small delay to avoid tight spin that blocks interrupts
        delayMicroseconds(10);
    }
}

uint8_t W5500Chip::readSocketStatus(uint8_t socket) { return readSn(socket, WIZ_Sn_SR); }
uint8_t W5500Chip::readSocketInterrupt(uint8_t socket) { return readSn(socket, WIZ_Sn_IR); }
void W5500Chip::writeSocketInterrupt(uint8_t socket, uint8_t flags) {
    writeSn(socket, WIZ_Sn_IR, flags);
}

uint16_t W5500Chip::getTXFreeSize(uint8_t socket) {
    uint16_t v = 0, v1 = 0;
    do {
        uint8_t hi = readSn(socket, WIZ_Sn_TX_FSR);
        uint8_t lo = readSn(socket, WIZ_Sn_TX_FSR + 1);
        v = ((uint16_t)hi << 8) | lo;
        uint8_t hi2 = readSn(socket, WIZ_Sn_TX_FSR);
        uint8_t lo2 = readSn(socket, WIZ_Sn_TX_FSR + 1);
        v1 = ((uint16_t)hi2 << 8) | lo2;
    } while (v != v1);
    return v;
}

uint16_t W5500Chip::getRXReceivedSize(uint8_t socket) {
    uint16_t v = 0, v1 = 0;
    do {
        uint8_t hi = readSn(socket, WIZ_Sn_RX_RSR);
        uint8_t lo = readSn(socket, WIZ_Sn_RX_RSR + 1);
        v = ((uint16_t)hi << 8) | lo;
        uint8_t hi2 = readSn(socket, WIZ_Sn_RX_RSR);
        uint8_t lo2 = readSn(socket, WIZ_Sn_RX_RSR + 1);
        v1 = ((uint16_t)hi2 << 8) | lo2;
    } while (v != v1);
    return v;
}

void W5500Chip::readSocketData(uint8_t socket, uint16_t srcPtr, uint8_t* dest, uint16_t len,
                               bool peek) {
    readSocketBuffer(socket, srcPtr, dest, len);
    if (!peek) {
        // advance RX read pointer
        uint8_t hi = readSn(socket, WIZ_Sn_RX_RD);
        uint8_t lo = readSn(socket, WIZ_Sn_RX_RD + 1);
        uint16_t ptr = ((uint16_t)hi << 8) | lo;
        ptr += len;
        writeSn(socket, WIZ_Sn_RX_RD, (uint8_t)(ptr >> 8));
        writeSn(socket, WIZ_Sn_RX_RD + 1, (uint8_t)(ptr & 0xFF));
    }
}

void W5500Chip::writeSocketData(uint8_t socket, const uint8_t* data, uint16_t len) {
    // Write at current TX write pointer
    uint8_t hi = readSn(socket, WIZ_Sn_TX_WR);
    uint8_t lo = readSn(socket, WIZ_Sn_TX_WR + 1);
    uint16_t ptr = ((uint16_t)hi << 8) | lo;
    writeSocketBuffer(socket, ptr, data, len);
    ptr += len;
    writeSn(socket, WIZ_Sn_TX_WR, (uint8_t)(ptr >> 8));
    writeSn(socket, WIZ_Sn_TX_WR + 1, (uint8_t)(ptr & 0xFF));
}

void W5500Chip::writeSocketDataOffset(uint8_t socket, uint16_t offset, const uint8_t* data,
                                      uint16_t len) {
    uint8_t hi = readSn(socket, WIZ_Sn_TX_WR);
    uint8_t lo = readSn(socket, WIZ_Sn_TX_WR + 1);
    uint16_t ptr = ((uint16_t)hi << 8) | lo;
    ptr += offset;
    writeSocketBuffer(socket, ptr, data, len);
    // Do not advance base pointer here; caller will call advanceTX when finished
}

void W5500Chip::advanceRX(uint8_t socket, uint16_t len) {
    uint8_t hi = readSn(socket, WIZ_Sn_RX_RD);
    uint8_t lo = readSn(socket, WIZ_Sn_RX_RD + 1);
    uint16_t ptr = ((uint16_t)hi << 8) | lo;
    ptr += len;
    writeSn(socket, WIZ_Sn_RX_RD, (uint8_t)(ptr >> 8));
    writeSn(socket, WIZ_Sn_RX_RD + 1, (uint8_t)(ptr & 0xFF));
}

void W5500Chip::advanceTX(uint8_t socket, uint16_t len) {
    uint8_t hi = readSn(socket, WIZ_Sn_TX_WR);
    uint8_t lo = readSn(socket, WIZ_Sn_TX_WR + 1);
    uint16_t ptr = ((uint16_t)hi << 8) | lo;
    ptr += len;
    writeSn(socket, WIZ_Sn_TX_WR, (uint8_t)(ptr >> 8));
    writeSn(socket, WIZ_Sn_TX_WR + 1, (uint8_t)(ptr & 0xFF));
}

void W5500Chip::commitTX(uint8_t socket) { execSocketCommand(socket, Sock_SEND); }

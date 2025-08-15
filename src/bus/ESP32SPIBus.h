/*
 * ESP32SPIBus.h - ESP32 SPI Bus implementation for Ethernet3 library
 * Provides ESP32-specific SPI optimizations and features
 */

#ifndef ESP32_SPI_BUS_H
#define ESP32_SPI_BUS_H

#include "EthernetBus.h"

#ifdef ESP32
#include <SPI.h>

/**
 * ESP32-specific SPI Bus implementation with hardware optimizations
 */
class ESP32SPIBus : public EthernetBus {
private:
    uint8_t _cs_pin;
    SPIClass* _spi;
    uint32_t _spi_freq;
    bool _initialized;
    
public:
    ESP32SPIBus(SPIClass* spi = &SPI, uint32_t frequency = 8000000) 
        : _spi(spi), _spi_freq(frequency), _initialized(false) {
    }
    
    void begin(uint8_t cs_pin) override {
        _cs_pin = cs_pin;
        pinMode(_cs_pin, OUTPUT);
        digitalWrite(_cs_pin, HIGH);
        
        _spi->begin();
        _initialized = true;
    }
    
    void end() override {
        if (_initialized) {
            _spi->end();
            _initialized = false;
        }
    }
    
    uint8_t transfer(uint8_t data) override {
        if (!_initialized) return 0;
        
        _spi->beginTransaction(SPISettings(_spi_freq, MSBFIRST, SPI_MODE0));
        digitalWrite(_cs_pin, LOW);
        uint8_t result = _spi->transfer(data);
        digitalWrite(_cs_pin, HIGH);
        _spi->endTransaction();
        return result;
    }
    
    void transfer(uint8_t* buffer, size_t length) override {
        if (!_initialized || !buffer || length == 0) return;
        
        _spi->beginTransaction(SPISettings(_spi_freq, MSBFIRST, SPI_MODE0));
        digitalWrite(_cs_pin, LOW);
        _spi->transfer(buffer, length);
        digitalWrite(_cs_pin, HIGH);
        _spi->endTransaction();
    }
    
    void setBitOrder(uint8_t order) override {
        // ESP32 SPI settings are handled in beginTransaction
    }
    
    void setDataMode(uint8_t mode) override {
        // ESP32 SPI settings are handled in beginTransaction
    }
    
    void setClockDivider(uint8_t rate) override {
        // ESP32 uses frequency directly, convert rate to frequency
        switch(rate) {
            case SPI_CLOCK_DIV2:   _spi_freq = 8000000; break;
            case SPI_CLOCK_DIV4:   _spi_freq = 4000000; break;
            case SPI_CLOCK_DIV8:   _spi_freq = 2000000; break;
            case SPI_CLOCK_DIV16:  _spi_freq = 1000000; break;
            case SPI_CLOCK_DIV32:  _spi_freq = 500000; break;
            case SPI_CLOCK_DIV64:  _spi_freq = 250000; break;
            case SPI_CLOCK_DIV128: _spi_freq = 125000; break;
            default: _spi_freq = 4000000; break;
        }
    }
    
    // ESP32-specific features
    void setFrequency(uint32_t frequency) {
        _spi_freq = frequency;
    }
    
    void useHSPI() {
        _spi = new SPIClass(HSPI);
    }
    
    void useVSPI() {
        _spi = new SPIClass(VSPI);
    }
    
    // DMA support (if implemented in ESP32 Arduino core)
    bool supportsDMA() {
        return true;  // ESP32 has DMA capability
    }
};

#endif // ESP32

#endif // ESP32_SPI_BUS_H
/*
 * STM32SPIBus.h - STM32 SPI Bus implementation for Ethernet3 library
 * Provides STM32-specific SPI optimizations and features
 */

#ifndef STM32_SPI_BUS_H
#define STM32_SPI_BUS_H

#include "EthernetBus.h"

#if defined(STM32F1) || defined(STM32F4) || defined(STM32) || defined(__STM32__)
#include <SPI.h>

/**
 * STM32-specific SPI Bus implementation with hardware optimizations
 */
class STM32SPIBus : public EthernetBus {
private:
    uint8_t _cs_pin;
    SPIClass* _spi;
    uint32_t _spi_freq;
    bool _initialized;
    
public:
    STM32SPIBus(SPIClass* spi = &SPI, uint32_t frequency = 8000000) 
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
        
        // STM32 may not have bulk transfer, use loop
        for (size_t i = 0; i < length; i++) {
            buffer[i] = _spi->transfer(buffer[i]);
        }
        
        digitalWrite(_cs_pin, HIGH);
        _spi->endTransaction();
    }
    
    void setBitOrder(uint8_t order) override {
        // STM32 SPI settings are handled in beginTransaction
    }
    
    void setDataMode(uint8_t mode) override {
        // STM32 SPI settings are handled in beginTransaction
    }
    
    void setClockDivider(uint8_t rate) override {
        // STM32 uses frequency directly, convert rate to frequency
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
    
    // STM32-specific features
    void setFrequency(uint32_t frequency) {
        _spi_freq = frequency;
    }
    
    // DMA support indicator
    bool supportsDMA() {
        #ifdef DMA1
        return true;  // STM32 has DMA capability
        #else
        return false;
        #endif
    }
    
    // Hardware SPI port selection (if supported)
    void useSPI1() {
        #ifdef SPI1
        _spi = new SPIClass(SPI1);
        #endif
    }
    
    void useSPI2() {
        #ifdef SPI2
        _spi = new SPIClass(SPI2);
        #endif
    }
    
    void useSPI3() {
        #ifdef SPI3
        _spi = new SPIClass(SPI3);
        #endif
    }
};

#endif // STM32 variants

#endif // STM32_SPI_BUS_H
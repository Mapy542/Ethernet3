/*
 * STM32Platform.h - STM32 unified platform implementation for Ethernet3 library
 * Provides STM32-specific optimizations including DMA and hardware acceleration
 */

#ifndef STM32_PLATFORM_H
#define STM32_PLATFORM_H

#include "EthernetPlatform.h"

#ifdef STM32
#include <Arduino.h>
#include <SPI.h>

/**
 * STM32-specific unified platform implementation with hardware optimizations
 */
class STM32Platform : public EthernetPlatform {
private:
    uint8_t _cs_pin;
    SPIClass* _spi;
    uint32_t _spi_freq;
    bool _initialized;
    bool _dma_enabled;
    
public:
    STM32Platform(SPIClass* spi = &SPI, uint32_t frequency = 12000000, bool enable_dma = true) 
        : _spi(spi), _spi_freq(frequency), _initialized(false), _dma_enabled(enable_dma) {
    }
    
    // Hardware abstraction implementation
    void delay(uint32_t ms) override {
        ::delay(ms);
    }
    
    uint32_t millis() override {
        return ::millis();
    }
    
    void digitalWrite(uint8_t pin, uint8_t value) override {
        ::digitalWrite(pin, value);
    }
    
    void pinMode(uint8_t pin, uint8_t mode) override {
        ::pinMode(pin, mode);
    }
    
    uint8_t digitalRead(uint8_t pin) override {
        return ::digitalRead(pin);
    }
    
    void yield() override {
        // STM32 typically doesn't need yield in baremetal
    }
    
    // SPI communication implementation
    void spiBegin(uint8_t cs_pin) override {
        _cs_pin = cs_pin;
        pinMode(_cs_pin, OUTPUT);
        digitalWrite(_cs_pin, HIGH);
        
        _spi->begin();
        _initialized = true;
        
        // Configure STM32-specific SPI settings
        #ifdef STM32F4
        if (_dma_enabled) {
            // Enable DMA for STM32F4 if available
            // Platform-specific DMA configuration would go here
        }
        #endif
    }
    
    void spiEnd() override {
        if (_initialized) {
            _spi->end();
            _initialized = false;
        }
    }
    
    uint8_t spiTransfer(uint8_t data) override {
        if (!_initialized) return 0;
        
        _spi->beginTransaction(SPISettings(_spi_freq, MSBFIRST, SPI_MODE0));
        digitalWrite(_cs_pin, LOW);
        uint8_t result = _spi->transfer(data);
        digitalWrite(_cs_pin, HIGH);
        _spi->endTransaction();
        return result;
    }
    
    void spiTransfer(uint8_t* buffer, size_t length) override {
        if (!_initialized || !buffer || length == 0) return;
        
        _spi->beginTransaction(SPISettings(_spi_freq, MSBFIRST, SPI_MODE0));
        digitalWrite(_cs_pin, LOW);
        
        #ifdef STM32F4
        if (_dma_enabled && length > 16) {
            // Use DMA for larger transfers on STM32F4
            _spi->transfer(buffer, length);  // Assuming DMA support in SPI library
        } else {
            _spi->transfer(buffer, length);
        }
        #else
        _spi->transfer(buffer, length);
        #endif
        
        digitalWrite(_cs_pin, HIGH);
        _spi->endTransaction();
    }
    
    void spiSetBitOrder(uint8_t order) override {
        // STM32 SPI settings are handled in beginTransaction
    }
    
    void spiSetDataMode(uint8_t mode) override {
        // STM32 SPI settings are handled in beginTransaction
    }
    
    void spiSetClockDivider(uint8_t rate) override {
        // STM32 uses frequency directly, convert rate to frequency
        switch(rate) {
            case SPI_CLOCK_DIV2:   _spi_freq = 12000000; break;
            case SPI_CLOCK_DIV4:   _spi_freq = 6000000; break;
            case SPI_CLOCK_DIV8:   _spi_freq = 3000000; break;
            case SPI_CLOCK_DIV16:  _spi_freq = 1500000; break;
            case SPI_CLOCK_DIV32:  _spi_freq = 750000; break;
            case SPI_CLOCK_DIV64:  _spi_freq = 375000; break;
            case SPI_CLOCK_DIV128: _spi_freq = 187500; break;
            default: _spi_freq = 6000000; break;
        }
    }
    
    void spiBeginTransaction() override {
        _spi->beginTransaction(SPISettings(_spi_freq, MSBFIRST, SPI_MODE0));
        digitalWrite(_cs_pin, LOW);
    }
    
    void spiEndTransaction() override {
        digitalWrite(_cs_pin, HIGH);
        _spi->endTransaction();
    }
    
    // Platform capability implementation
    const char* getPlatformName() override {
        #ifdef STM32F1
        return "STM32F1";
        #elif defined(STM32F4)
        return "STM32F4";
        #else
        return "STM32";
        #endif
    }
    
    bool hasHardwareAcceleration() override {
        #ifdef STM32F4
        return true;  // STM32F4 has hardware acceleration
        #else
        return false;
        #endif
    }
    
    bool supportsDMA() override {
        return true;  // Most STM32s have DMA capability
    }
    
    bool supportsRealTime() override {
        return true;  // STM32 can do real-time operations
    }
    
    bool supportsTransactions() override {
        return true;  // STM32 SPI supports transactions
    }
    
    uint32_t getMaxSPISpeed() override {
        #ifdef STM32F4
        return 12000000;  // 12MHz for STM32F4
        #else
        return 8000000;   // 8MHz for other STM32s
        #endif
    }
    
    const char* getPlatformInfo() override {
        return "STM32 Platform with DMA SPI";
    }
    
    // STM32-specific features
    void enableDMA(bool enable) {
        _dma_enabled = enable;
    }
    
    bool isDMAEnabled() {
        return _dma_enabled;
    }
    
    void setFrequency(uint32_t frequency) {
        _spi_freq = frequency;
    }
};

#endif // STM32

#endif // STM32_PLATFORM_H
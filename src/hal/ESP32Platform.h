/*
 * ESP32Platform.h - ESP32 unified platform implementation for Ethernet3 library
 * Provides ESP32-specific optimizations and features
 */

#ifndef ESP32_PLATFORM_H
#define ESP32_PLATFORM_H

#include "EthernetPlatform.h"

#ifdef ESP32
#include <Arduino.h>
#include <SPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * ESP32-specific unified platform implementation with RTOS optimizations
 */
class ESP32Platform : public EthernetPlatform {
private:
    uint8_t _cs_pin;
    SPIClass* _spi;
    uint32_t _spi_freq;
    bool _initialized;
    portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;
    
public:
    ESP32Platform(SPIClass* spi = &SPI, uint32_t frequency = 16000000) 
        : _spi(spi), _spi_freq(frequency), _initialized(false) {
    }
    
    // Hardware abstraction implementation
    void delay(uint32_t ms) override {
        if (ms > 0) {
            vTaskDelay(pdMS_TO_TICKS(ms));  // Use FreeRTOS delay for better task scheduling
        }
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
        vTaskDelay(1);  // Yield to other tasks
    }
    
    // SPI communication implementation
    void spiBegin(uint8_t cs_pin) override {
        _cs_pin = cs_pin;
        pinMode(_cs_pin, OUTPUT);
        digitalWrite(_cs_pin, HIGH);
        
        _spi->begin();
        _initialized = true;
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
        _spi->transfer(buffer, length);
        digitalWrite(_cs_pin, HIGH);
        _spi->endTransaction();
    }
    
    void spiSetBitOrder(uint8_t order) override {
        // ESP32 SPI settings are handled in beginTransaction
    }
    
    void spiSetDataMode(uint8_t mode) override {
        // ESP32 SPI settings are handled in beginTransaction
    }
    
    void spiSetClockDivider(uint8_t rate) override {
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
        return "ESP32 with FreeRTOS";
    }
    
    bool hasHardwareAcceleration() override {
        return true;  // ESP32 has hardware acceleration
    }
    
    bool supportsDMA() override {
        return true;  // ESP32 has DMA capability
    }
    
    bool supportsRealTime() override {
        return true;  // ESP32 has FreeRTOS
    }
    
    bool supportsTransactions() override {
        return true;  // ESP32 SPI supports transactions
    }
    
    uint32_t getMaxSPISpeed() override {
        return 16000000;  // 16MHz for ESP32
    }
    
    const char* getPlatformInfo() override {
        return "ESP32 Platform with FreeRTOS and DMA SPI";
    }
    
    // ESP32-specific features
    uint32_t getCoreId() {
        return xPortGetCoreID();
    }
    
    void enterCriticalSection() {
        portENTER_CRITICAL_ISR(&_mux);
    }
    
    void exitCriticalSection() {
        portEXIT_CRITICAL_ISR(&_mux);
    }
    
    void setFrequency(uint32_t frequency) {
        _spi_freq = frequency;
    }
    
    void useHSPI() {
        _spi = new SPIClass(HSPI);
    }
    
    void useVSPI() {
        _spi = new SPIClass(VSPI);
    }
};

#endif // ESP32

#endif // ESP32_PLATFORM_H
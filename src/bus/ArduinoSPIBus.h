/*
 * ArduinoSPIBus.h - Arduino SPI bus implementation for Ethernet3 library
 */

#ifndef ARDUINO_SPI_BUS_H
#define ARDUINO_SPI_BUS_H

#include "../EthernetBus.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>

/**
 * Arduino-specific SPI bus implementation
 */
class ArduinoSPIBus : public EthernetBus {
private:
    SPISettings settings;
    uint8_t cs_pin;
    
public:
    ArduinoSPIBus() : settings(8000000, MSBFIRST, SPI_MODE0), cs_pin(10) {}
    
    void begin(uint8_t cs_pin) override {
        this->cs_pin = cs_pin;
        pinMode(cs_pin, OUTPUT);
        digitalWrite(cs_pin, HIGH);
        SPI.begin();
    }
    
    uint8_t transfer(uint8_t data) override {
        return SPI.transfer(data);
    }
    
    void transfer(uint8_t* buffer, size_t length) override {
        SPI.transfer(buffer, length);
    }
    
    void setBitOrder(uint8_t order) override {
        // Update settings and apply
        settings = SPISettings(8000000, order, SPI_MODE0);
    }
    
    void setDataMode(uint8_t mode) override {
        // Update settings and apply
        settings = SPISettings(8000000, MSBFIRST, mode);
    }
    
    void setClockDivider(uint8_t rate) override {
        // Convert Arduino clock divider to frequency
        uint32_t freq = 16000000 / rate;  // Assuming 16MHz Arduino
        settings = SPISettings(freq, MSBFIRST, SPI_MODE0);
    }
    
    void beginTransaction() override {
        SPI.beginTransaction(settings);
        digitalWrite(cs_pin, LOW);
    }
    
    void endTransaction() override {
        digitalWrite(cs_pin, HIGH);
        SPI.endTransaction();
    }
};

#endif // ARDUINO

#endif // ARDUINO_SPI_BUS_H
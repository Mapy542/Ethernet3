/*
 * Ethernet3.h - Enhanced Ethernet class with multi-instance support
 *
 * Modern multi-instance Ethernet library built on Ethernet2 foundation
 *
 * Backward Compatibility:
 * - By default, provides backward compatibility with legacy Ethernet/Ethernet2 code
 * - Define ETHERNET3_NO_BACKWARDS_COMPATIBILITY to disable singleton instances
 * - When disabled, all network classes require an Ethernet3Class instance
 */

#ifndef ETHERNET3_H
#define ETHERNET3_H

#include <inttypes.h>

#include "IPAddress.h"
#include "chips/EthernetChip.h"
#include "chips/W5100Chip.h"
#include "chips/W5500Chip.h"
#include "hal/ArduinoPlatform.h"

// Constants for backward compatibility
#define MAX_SOCK_NUM 8  // Default to W5500's socket count for backward compatibility

// Forward declarations
class EthernetClient;
class EthernetServer;
class DhcpClass;

/**
 * Enhanced EthernetClass with multi-instance support
 * Modern replacement for singleton-based Ethernet/Ethernet2 libraries
 */
class Ethernet3Class {
   private:
    IPAddress _dnsServerAddress;
    char* _dnsDomainName;
    char* _hostName;
    DhcpClass* _dhcp;

    // Multi-instance support
    EthernetChip* _chip;
    EthernetPlatform* _platform;

    // Per-instance state (replaces static arrays)
    uint8_t* _state;
    uint16_t* _server_port;
    uint8_t _max_sockets;

    // Chip configuration
    uint8_t _cs_pin;
    bool _owns_chip;  // Whether this instance owns the chip/bus/hal objects

   public:
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
    /**
     * Default constructor for backward compatibility
     * Only available when ETHERNET3_NO_BACKWARDS_COMPATIBILITY is not defined
     * Uses global W5500 instance
     */
    Ethernet3Class();
#endif

    /**
     * Constructor with specific chip instance
     * @param chip Pointer to EthernetChip implementation
     * @param owns_chip Whether this instance should delete the chip on destruction
     */
    Ethernet3Class(EthernetChip* chip, bool owns_chip = false);

    /**
     * Constructor for creating new chip instances
     * @param chip_type Type of chip (CHIP_TYPE_W5100 or CHIP_TYPE_W5500)
     * @param cs_pin Chip select pin
     * @param bus_interface Optional bus interface (uses Arduino SPI if null)
     * @param platform_interface Optional platform interface (uses Arduino platform if null)
     */
    Ethernet3Class(uint8_t chip_type, uint8_t cs_pin,
                   EthernetPlatform* platform_interface = nullptr);

    /**
     * Destructor
     */
    ~Ethernet3Class();

    /**
     * Initialize with chip select pin (backward compatibility)
     */
    void init(uint8_t cs_pin = 10);

#if defined(WIZ550io_WITH_MACADDRESS)
    // Initialize function when use the ioShield series (included WIZ550io)
    int begin(void);
    void begin(IPAddress local_ip);
    void begin(IPAddress local_ip, IPAddress dns_server);
    void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
    void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
#else
    // Initialize the Ethernet shield to use the provided MAC address and gain the rest of the
    // configuration through DHCP.
    int begin(uint8_t* mac_address);
    void begin(uint8_t* mac_address, IPAddress local_ip);
    void begin(uint8_t* mac_address, IPAddress local_ip, IPAddress dns_server);
    void begin(uint8_t* mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
    void begin(uint8_t* mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway,
               IPAddress subnet);
#endif

    int maintain();

    IPAddress localIP();
    IPAddress subnetMask();
    IPAddress gatewayIP();
    IPAddress dnsServerIP();
    char* dnsDomainName();
    char* hostName();

    // Link status monitoring - checks physical link state via PHY configuration register
    bool linkActive();

    // Hardware status checking
    uint8_t hardwareStatus();
    uint8_t linkStatus();

    // Multi-instance specific methods

    /**
     * Get the chip instance
     * @return Pointer to EthernetChip implementation
     */
    EthernetChip* getChip() const { return _chip; }

    /**
     * Get socket state for this instance
     * @param sock Socket number
     * @return Socket state
     */
    uint8_t getSocketState(uint8_t sock) const;

    /**
     * Set socket state for this instance
     * @param sock Socket number
     * @param state Socket state
     */
    void setSocketState(uint8_t sock, uint8_t state);

    /**
     * Get server port for this instance
     * @param sock Socket number
     * @return Server port
     */
    uint16_t getServerPort(uint8_t sock) const;

    /**
     * Set server port for this instance
     * @param sock Socket number
     * @param port Server port
     */
    void setServerPort(uint8_t sock, uint16_t port);

    /**
     * Get maximum number of sockets for this chip
     * @return Maximum socket count
     */
    uint8_t getMaxSockets() const { return _max_sockets; }

    friend class EthernetClient;
    friend class EthernetServer;

   private:
    /**
     * Initialize internal structures
     */
    void initializeInstance();

    /**
     * Cleanup internal structures
     */
    void cleanup();
};

// Hardware status constants
#define EthernetNoHardware 0
#define EthernetW5100 1
#define EthernetW5200 2
#define EthernetW5500 3

// Link status constants
#define Unknown 0
#define LinkON 1
#define LinkOFF 2

// Backward compatibility support (conditional)
#ifndef ETHERNET3_NO_BACKWARDS_COMPATIBILITY
// Global instance for backward compatibility with Ethernet/Ethernet2
extern Ethernet3Class Ethernet;

// Type alias for legacy code compatibility
typedef Ethernet3Class EthernetClass;
#endif

#endif  // ETHERNET3_H
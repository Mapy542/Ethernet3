#ifndef ethernet_h
#define ethernet_h

/**
 * @file Ethernet3.h
 * @brief Main Ethernet3 library header providing enhanced WIZnet chip support
 * 
 * modified 12 Aug 2013
 * by Soohwan Kim (suhwan@wiznet.co.kr)
 *
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 *
 * - 21 Aug. 2025
 *    Added W5500 class for new chip support and rm singleton system
 *
 * The Ethernet3 library provides an improved interface for WIZnet-based Ethernet
 * shields and modules, with enhanced support for W5500 chips and an abstracted
 * chip interface allowing support for multiple chip types.
 */

#include "Dhcp.h"
#include "EthernetClient.h"
#include "EthernetServer.h"
#include "IPAddress.h"
#include "chips/utility/socket.h"
#include "chips/utility/wiznet_registers.h"
#include "chips/w5500.h"

class DhcpClass;  // Forward declaration to avoid circular dependency
class EthernetChip;

/**
 * @brief Main Ethernet class providing network connectivity using WIZnet chips
 * 
 * The EthernetClass provides the primary interface for initializing and managing
 * Ethernet connectivity using WIZnet chips (W5100, W5500, etc.). Unlike the
 * standard Arduino Ethernet library, this implementation uses an abstract chip
 * interface allowing support for multiple chip types and enhanced functionality.
 * 
 * Key differences from standard Arduino Ethernet library:
 * - Abstract chip interface supporting multiple WIZnet chip types
 * - Enhanced W5500 support with improved performance
 * - Non-singleton design allowing multiple instances
 * - Improved DHCP handling and network configuration
 */
class EthernetClass {
   private:
    EthernetChip* _chip;         ///< Pointer to the Ethernet chip interface
    IPAddress _dnsServerAddress; ///< DNS server IP address
    char* _dnsDomainName;        ///< DNS domain name from DHCP
    char* _hostName;             ///< Host name from DHCP
    DhcpClass* _dhcp;            ///< DHCP client instance

   public:
    uint8_t _state[MAX_SOCK_NUM];      ///< Socket state array
    uint16_t _server_port[MAX_SOCK_NUM]; ///< Server port array for each socket

    /**
     * @brief Constructor for EthernetClass
     * @param chip Pointer to EthernetChip implementation (W5500, W5100, etc.)
     * 
     * Creates an Ethernet instance using the specified chip interface.
     * The chip pointer must remain valid for the lifetime of this object.
     */
    EthernetClass(EthernetChip* chip) : _chip(chip) { _dhcp = nullptr; }

#if defined(WIZ550io_WITH_MACADDRESS)
    /**
     * @brief Initialize Ethernet with WIZ550io module (has built-in MAC address)
     * @return 1 if DHCP configuration succeeded, 0 if failed
     * 
     * For WIZ550io modules that have a pre-programmed MAC address. The module
     * will attempt to configure itself using DHCP. This is only available when
     * WIZ550io_WITH_MACADDRESS is defined.
     * 
     * @note WIZ550io requires initialization time after reset - refer to datasheet
     */
    int begin(void);
    
    /**
     * @brief Initialize Ethernet with WIZ550io and static IP
     * @param local_ip Static IP address to assign
     * 
     * Uses WIZ550io's built-in MAC with static IP configuration.
     * DNS server and gateway are automatically set to x.x.x.1 on the same subnet.
     */
    void begin(IPAddress local_ip);
    
    /**
     * @brief Initialize Ethernet with WIZ550io, static IP and DNS server
     * @param local_ip Static IP address to assign
     * @param dns_server DNS server IP address
     * 
     * Uses WIZ550io's built-in MAC with static IP and custom DNS.
     * Gateway is automatically set to x.x.x.1 on the same subnet.
     */
    void begin(IPAddress local_ip, IPAddress dns_server);
    
    /**
     * @brief Initialize Ethernet with WIZ550io, static IP, DNS and gateway
     * @param local_ip Static IP address to assign
     * @param dns_server DNS server IP address
     * @param gateway Gateway IP address
     * 
     * Uses WIZ550io's built-in MAC with static network configuration.
     * Subnet mask defaults to 255.255.255.0.
     */
    void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
    
    /**
     * @brief Initialize Ethernet with WIZ550io and full static configuration
     * @param local_ip Static IP address to assign
     * @param dns_server DNS server IP address  
     * @param gateway Gateway IP address
     * @param subnet Subnet mask
     * 
     * Uses WIZ550io's built-in MAC with complete static network configuration.
     */
    void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
#else
    /**
     * @brief Initialize Ethernet with MAC address using DHCP
     * @param mac_address 6-byte MAC address array
     * @return 1 if DHCP configuration succeeded, 0 if failed
     * 
     * Initialize the Ethernet shield with the provided MAC address and obtain
     * network configuration (IP, gateway, subnet, DNS) automatically via DHCP.
     * This is the most common initialization method.
     * 
     * @note If DHCP fails, manual configuration with static IP is required
     */
    int begin(uint8_t* mac_address);
    
    /**
     * @brief Initialize Ethernet with MAC and static IP
     * @param mac_address 6-byte MAC address array
     * @param local_ip Static IP address to assign
     * 
     * Initialize with static IP configuration. DNS server and gateway
     * are automatically set to x.x.x.1 on the same subnet.
     */
    void begin(uint8_t* mac_address, IPAddress local_ip);
    
    /**
     * @brief Initialize Ethernet with MAC, static IP and DNS server
     * @param mac_address 6-byte MAC address array
     * @param local_ip Static IP address to assign
     * @param dns_server DNS server IP address
     * 
     * Initialize with static IP and custom DNS server. Gateway is
     * automatically set to x.x.x.1 on the same subnet.
     */
    void begin(uint8_t* mac_address, IPAddress local_ip, IPAddress dns_server);
    
    /**
     * @brief Initialize Ethernet with MAC, static IP, DNS and gateway
     * @param mac_address 6-byte MAC address array
     * @param local_ip Static IP address to assign
     * @param dns_server DNS server IP address
     * @param gateway Gateway IP address
     * 
     * Initialize with static network configuration. Subnet mask defaults
     * to 255.255.255.0.
     */
    void begin(uint8_t* mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
    
    /**
     * @brief Initialize Ethernet with complete static configuration
     * @param mac_address 6-byte MAC address array
     * @param local_ip Static IP address to assign
     * @param dns_server DNS server IP address
     * @param gateway Gateway IP address  
     * @param subnet Subnet mask
     * 
     * Initialize with complete manual network configuration. This provides
     * full control over all network parameters.
     */
    void begin(uint8_t* mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway,
               IPAddress subnet);

#endif

    /**
     * @brief Maintain DHCP lease and handle renewal/rebinding
     * @return DHCP status code (DHCP_CHECK_NONE, DHCP_CHECK_RENEW_OK, etc.)
     * 
     * Call this regularly in loop() to maintain DHCP lease. Handles automatic
     * lease renewal and rebinding. Returns status codes indicating any changes
     * to network configuration.
     * 
     * @note Only needed when using DHCP initialization
     */
    int maintain();

    /**
     * @brief Get current local IP address
     * @return Current IP address assigned to this device
     */
    IPAddress localIP();
    
    /**
     * @brief Get current subnet mask
     * @return Current subnet mask
     */
    IPAddress subnetMask();
    
    /**
     * @brief Get current gateway IP address
     * @return Current gateway IP address
     */
    IPAddress gatewayIP();
    
    /**
     * @brief Get current DNS server IP address
     * @return Current DNS server IP address
     */
    IPAddress dnsServerIP();
    
    /**
     * @brief Get DNS domain name from DHCP
     * @return Pointer to DNS domain name string, or NULL if not available
     * 
     * Returns the domain name provided by DHCP server, if available.
     * Only valid after successful DHCP initialization.
     */
    char* dnsDomainName();
    
    /**
     * @brief Get host name from DHCP
     * @return Pointer to host name string, or NULL if not available
     * 
     * Returns the host name provided by DHCP server, if available.
     * Only valid after successful DHCP initialization.
     */
    char* hostName();

    /**
     * @brief Allow EthernetClient to access private members
     */
    friend class EthernetClient;
    
    /**
     * @brief Allow EthernetServer to access private members  
     */
    friend class EthernetServer;
};

#endif

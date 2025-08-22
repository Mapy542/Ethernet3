/**
 * @file Ethernet3.cpp
 * @brief Implementation of the main Ethernet3 class
 * 
 * modified 12 Aug 2013
 * by Soohwan Kim (suhwan@wiznet.co.kr)
 *
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 *
 * This file implements the core Ethernet functionality using an abstract
 * chip interface, allowing support for multiple WIZnet chip types with
 * enhanced performance and features compared to the standard Arduino library.
 */

#include "Ethernet3.h"

#if defined(WIZ550io_WITH_MACADDRESS)
/**
 * @brief Initialize Ethernet using WIZ550io's built-in MAC address with DHCP
 * @return 1 if DHCP succeeded, 0 if failed
 * 
 * This function initializes the Ethernet connection for WIZ550io modules that
 * have a pre-programmed MAC address. It attempts to configure the network
 * settings automatically using DHCP.
 * 
 * The function performs the following operations:
 * 1. Initializes the chip hardware
 * 2. Retrieves the built-in MAC address
 * 3. Attempts DHCP configuration
 * 4. Updates network settings if DHCP succeeds
 * 
 * @note WIZ550io modules require initialization time after reset
 */
int EthernetClass::begin(void) {
    byte mac_address[6] = {
        0,
    };
    if (_dhcp != NULL) {
        delete _dhcp;
    }
    _dhcp = new DhcpClass();

    // Initialise the basic info
    _chip->init();
    _chip->setIPAddress(IPAddress(0, 0, 0, 0).raw_address());
    _chip->getMACAddress(mac_address);

    // Now try to get our config info from a DHCP server
    int ret = _dhcp->beginWithDHCP(mac_address);
    if (ret == 1) {
        // We've successfully found a DHCP server and got our configuration info, so set things
        // accordingly
        _chip->setIPAddress(_dhcp->getLocalIp().raw_address());
        _chip->setGatewayIp(_dhcp->getGatewayIp().raw_address());
        _chip->setSubnetMask(_dhcp->getSubnetMask().raw_address());
        _dnsServerAddress = _dhcp->getDnsServerIp();
        _dnsDomainName = _dhcp->getDnsDomainName();
        _hostName = _dhcp->getHostName();
    }

    return ret;
}

/**
 * @brief Initialize WIZ550io with static IP, auto-configured DNS and gateway
 * @param local_ip Static IP address to assign
 * 
 * Initializes with static IP configuration. DNS server and gateway are
 * automatically set to .1 on the same subnet (e.g., if IP is 192.168.1.100,
 * DNS and gateway become 192.168.1.1).
 */
void EthernetClass::begin(IPAddress local_ip) {
    // Assume the DNS server will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress dns_server = local_ip;
    dns_server[3] = 1;
    begin(local_ip, dns_server);
}

/**
 * @brief Initialize WIZ550io with static IP and DNS, auto-configured gateway
 * @param local_ip Static IP address to assign
 * @param dns_server DNS server IP address
 * 
 * Initializes with static IP and custom DNS. Gateway is automatically set
 * to .1 on the same subnet as the local IP.
 */
void EthernetClass::begin(IPAddress local_ip, IPAddress dns_server) {
    // Assume the gateway will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress gateway = local_ip;
    gateway[3] = 1;
    begin(local_ip, dns_server, gateway);
}

/**
 * @brief Initialize WIZ550io with static IP, DNS and gateway, default subnet mask
 * @param local_ip Static IP address to assign
 * @param dns_server DNS server IP address
 * @param gateway Gateway IP address
 * 
 * Initializes with static configuration. Subnet mask defaults to 255.255.255.0.
 */
void EthernetClass::begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway) {
    IPAddress subnet(255, 255, 255, 0);
    begin(local_ip, dns_server, gateway, subnet);
}

/**
 * @brief Initialize WIZ550io with complete static network configuration
 * @param local_ip Static IP address to assign
 * @param dns_server DNS server IP address
 * @param gateway Gateway IP address
 * @param subnet Subnet mask
 * 
 * Provides complete manual control over network configuration for WIZ550io modules.
 */
void EthernetClass::begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway,
                          IPAddress subnet) {
    _chip->init();
    _chip->setIPAddress(local_ip.raw_address());
    _chip->setGatewayIp(gateway.raw_address());
    _chip->setSubnetMask(subnet.raw_address());
    _dnsServerAddress = dns_server;
}
#else
/**
 * @brief Initialize Ethernet with MAC address using DHCP
 * @param mac_address 6-byte MAC address array
 * @return 1 if DHCP succeeded, 0 if failed
 * 
 * Standard initialization method for most Ethernet shields. Initializes the
 * chip with the provided MAC address and attempts to configure network
 * settings automatically using DHCP.
 * 
 * This is the most commonly used initialization method. If DHCP fails,
 * you should fall back to static IP configuration.
 * 
 * @note The mac_address array must remain valid during initialization
 */
int EthernetClass::begin(uint8_t *mac_address) {
    if (_dhcp != NULL) {
        delete _dhcp;
    }
    _dhcp = new DhcpClass(this, _chip);
    // Initialise the basic info
    _chip->init();
    _chip->setMACAddress(mac_address);
    _chip->setIPAddress(IPAddress(0, 0, 0, 0).raw_address());

    // Now try to get our config info from a DHCP server
    int ret = _dhcp->beginWithDHCP(mac_address);
    if (ret == 1) {
        // We've successfully found a DHCP server and got our configuration info, so set things
        // accordingly
        _chip->setIPAddress(_dhcp->getLocalIp().raw_address());
        _chip->setGatewayIp(_dhcp->getGatewayIp().raw_address());
        _chip->setSubnetMask(_dhcp->getSubnetMask().raw_address());
        _dnsServerAddress = _dhcp->getDnsServerIp();
        _dnsDomainName = _dhcp->getDnsDomainName();
        _hostName = _dhcp->getHostName();
    }

    return ret;
}

/**
 * @brief Initialize Ethernet with MAC and static IP, auto-configured DNS and gateway
 * @param mac_address 6-byte MAC address array
 * @param local_ip Static IP address to assign
 * 
 * Initializes with static IP configuration. DNS server and gateway are
 * automatically set to .1 on the same subnet (e.g., if IP is 192.168.1.100,
 * DNS and gateway become 192.168.1.1).
 */
void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip) {
    // Assume the DNS server will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress dns_server = local_ip;
    dns_server[3] = 1;
    begin(mac_address, local_ip, dns_server);
}

/**
 * @brief Initialize Ethernet with MAC, static IP and DNS, auto-configured gateway
 * @param mac_address 6-byte MAC address array
 * @param local_ip Static IP address to assign
 * @param dns_server DNS server IP address
 * 
 * Initializes with static IP and custom DNS. Gateway is automatically set
 * to .1 on the same subnet as the local IP.
 */
void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server) {
    // Assume the gateway will be the machine on the same network as the local IP
    // but with last octet being '1'
    IPAddress gateway = local_ip;
    gateway[3] = 1;
    begin(mac_address, local_ip, dns_server, gateway);
}

/**
 * @brief Initialize Ethernet with MAC, static IP, DNS and gateway, default subnet mask
 * @param mac_address 6-byte MAC address array
 * @param local_ip Static IP address to assign
 * @param dns_server DNS server IP address
 * @param gateway Gateway IP address
 * 
 * Initializes with static configuration. Subnet mask defaults to 255.255.255.0.
 */
void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server,
                          IPAddress gateway) {
    IPAddress subnet(255, 255, 255, 0);
    begin(mac_address, local_ip, dns_server, gateway, subnet);
}

/**
 * @brief Initialize Ethernet with complete static network configuration
 * @param mac MAC address (6 bytes)
 * @param local_ip Static IP address to assign
 * @param dns_server DNS server IP address
 * @param gateway Gateway IP address
 * @param subnet Subnet mask
 * 
 * Provides complete manual control over network configuration. This is the
 * most flexible initialization method allowing full customization.
 */
void EthernetClass::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway,
                          IPAddress subnet) {
    _chip->init();
    _chip->setMACAddress(mac);
    _chip->setIPAddress(local_ip.raw_address());
    _chip->setGatewayIp(gateway.raw_address());
    _chip->setSubnetMask(subnet.raw_address());
    _dnsServerAddress = dns_server;
}

#endif

/**
 * @brief Maintain DHCP lease and handle renewal/rebinding
 * @return DHCP status code indicating the result of maintenance operations
 * 
 * This function should be called regularly in your main loop() to maintain
 * the DHCP lease. It handles automatic lease renewal and rebinding when necessary.
 * 
 * Return values:
 * - DHCP_CHECK_NONE: No action taken (no DHCP in use or lease still valid)
 * - DHCP_CHECK_RENEW_OK: Lease successfully renewed
 * - DHCP_CHECK_REBIND_OK: Lease successfully rebound
 * - DHCP_CHECK_RENEW_FAIL: Lease renewal failed
 * - DHCP_CHECK_REBIND_FAIL: Lease rebinding failed
 * 
 * When renewal or rebinding succeeds, network configuration is automatically
 * updated with any new settings from the DHCP server.
 * 
 * @note Only call this if you initialized with DHCP (not static IP)
 */
int EthernetClass::maintain() {
    int rc = DHCP_CHECK_NONE;
    if (_dhcp != NULL) {
        // we have a pointer to dhcp, use it
        rc = _dhcp->checkLease();
        switch (rc) {
            case DHCP_CHECK_NONE:
                // nothing done
                break;
            case DHCP_CHECK_RENEW_OK:
            case DHCP_CHECK_REBIND_OK:
                // we might have got a new IP.
                _chip->setIPAddress(_dhcp->getLocalIp().raw_address());
                _chip->setGatewayIp(_dhcp->getGatewayIp().raw_address());
                _chip->setSubnetMask(_dhcp->getSubnetMask().raw_address());
                _dnsServerAddress = _dhcp->getDnsServerIp();
                _dnsDomainName = _dhcp->getDnsDomainName();
                _hostName = _dhcp->getHostName();
                break;
            default:
                // this is actually a error, it will retry though
                break;
        }
    }
    return rc;
}

/**
 * @brief Get the current local IP address
 * @return Current IP address assigned to this device
 * 
 * Returns the IP address currently configured on the Ethernet chip.
 * This may change if using DHCP and the lease is renewed.
 */
IPAddress EthernetClass::localIP() {
    IPAddress ret;
    _chip->getIPAddress(ret.raw_address());
    return ret;
}

/**
 * @brief Get the current subnet mask
 * @return Current subnet mask
 * 
 * Returns the subnet mask currently configured on the Ethernet chip.
 */
IPAddress EthernetClass::subnetMask() {
    IPAddress ret;
    _chip->getSubnetMask(ret.raw_address());
    return ret;
}

/**
 * @brief Get the current gateway IP address
 * @return Current gateway IP address
 * 
 * Returns the gateway IP address currently configured on the Ethernet chip.
 */
IPAddress EthernetClass::gatewayIP() {
    IPAddress ret;
    _chip->getGatewayIp(ret.raw_address());
    return ret;
}

/**
 * @brief Get the current DNS server IP address
 * @return Current DNS server IP address
 * 
 * Returns the DNS server IP address. This is stored locally, not read from
 * the chip, as it's used by the DNS client functionality.
 */
IPAddress EthernetClass::dnsServerIP() { return _dnsServerAddress; }

/**
 * @brief Get the DNS domain name from DHCP
 * @return Pointer to DNS domain name string, or NULL if not available
 * 
 * Returns the domain name provided by the DHCP server during lease negotiation.
 * Only available when using DHCP initialization and if the DHCP server provides this option.
 */
char* EthernetClass::dnsDomainName() { return _dnsDomainName; }

/**
 * @brief Get the host name from DHCP
 * @return Pointer to host name string, or NULL if not available
 * 
 * Returns the host name provided by the DHCP server during lease negotiation.
 * Only available when using DHCP initialization and if the DHCP server provides this option.
 */
char* EthernetClass::hostName() { return _hostName; }

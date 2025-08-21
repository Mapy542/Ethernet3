#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "Arduino.h"
#include "EthernetClient.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HTTPConfig.h"
#include "IPAddress.h"

class HTTPClient {
private:
    EthernetClient _client;
    String _userAgent;
    unsigned long _timeout;
    
    // Helper method to read response from client
    String readResponse();
    
public:
    HTTPClient(EthernetClass* eth, EthernetChip* chip);
    
    // Configuration
    void setUserAgent(const String& userAgent);
    void setTimeout(unsigned long timeout);
    
    // Connection management
    bool connect(const char* host, uint16_t port = 80);
    bool connect(IPAddress ip, uint16_t port = 80);
    void disconnect();
    bool connected();
    
    // HTTP request methods
    HTTPResponse GET(const String& path);
    HTTPResponse POST(const String& path, const String& body = "", const String& contentType = "text/plain");
    HTTPResponse PUT(const String& path, const String& body = "", const String& contentType = "text/plain");
    HTTPResponse DELETE(const String& path);
    
    // Generic request method
    HTTPResponse sendRequest(const HTTPRequest& request);
    
    // Low-level methods for custom requests
    bool sendRawRequest(const String& requestString);
    HTTPResponse readResponseObject();
    
    // URL parsing helpers
    static bool parseURL(const String& url, String& protocol, String& host, uint16_t& port, String& path);
    
    // Convenience method for complete URL requests
    HTTPResponse request(const String& method, const String& url, const String& body = "");
};

#endif
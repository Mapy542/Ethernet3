#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "Arduino.h"
#include "HTTPConfig.h"

class HTTPRequest {
private:
    String _method;
    String _path;
    String _protocol;
    String _headers[HTTP_MAX_HEADERS];  // Support for configurable headers
    uint8_t _headerCount;
    String _body;
    
public:
    HTTPRequest();
    HTTPRequest(const String& method, const String& path);
    
    // Basic setters
    void setMethod(const String& method);
    void setPath(const String& path);
    void setProtocol(const String& protocol);
    void setBody(const String& body);
    
    // Header management
    void addHeader(const String& name, const String& value);
    void setHeader(const String& name, const String& value);
    String getHeader(const String& name);
    
    // Getters
    String getMethod() const;
    String getPath() const;
    String getProtocol() const;
    String getBody() const;
    uint8_t getHeaderCount() const;
    
    // Parse from raw HTTP request string
    bool parseFromString(const String& requestString);
    
    // Generate HTTP request string
    String toString() const;
    
    // Common HTTP methods
    static HTTPRequest GET(const String& path);
    static HTTPRequest POST(const String& path, const String& body = "");
    static HTTPRequest PUT(const String& path, const String& body = "");
    static HTTPRequest DELETE(const String& path);
};

#endif
#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include "Arduino.h"
#include "HTTPConfig.h"

class HTTPResponse {
private:
    String _protocol;
    int _statusCode;
    String _statusMessage;
    String _headers[HTTP_MAX_HEADERS];  // Support for configurable headers
    uint8_t _headerCount;
    String _body;
    
public:
    HTTPResponse();
    HTTPResponse(int statusCode, const String& statusMessage = "");
    
    // Basic setters
    void setStatusCode(int statusCode);
    void setStatusMessage(const String& statusMessage);
    void setProtocol(const String& protocol);
    void setBody(const String& body);
    
    // Header management
    void addHeader(const String& name, const String& value);
    void setHeader(const String& name, const String& value);
    String getHeader(const String& name);
    
    // Getters
    int getStatusCode() const;
    String getStatusMessage() const;
    String getProtocol() const;
    String getBody() const;
    uint8_t getHeaderCount() const;
    
    // Parse from raw HTTP response string
    bool parseFromString(const String& responseString);
    
    // Generate HTTP response string
    String toString() const;
    
    // Common status code helpers
    static HTTPResponse OK(const String& body = "");
    static HTTPResponse NotFound(const String& body = "404 Not Found");
    static HTTPResponse BadRequest(const String& body = "400 Bad Request");
    static HTTPResponse InternalServerError(const String& body = "500 Internal Server Error");
    
    // Helper to get standard status message for code
    static String getStandardStatusMessage(int statusCode);
};

#endif
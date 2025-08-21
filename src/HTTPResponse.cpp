#include "HTTPResponse.h"

HTTPResponse::HTTPResponse() : _protocol("HTTP/1.1"), _statusCode(200), _statusMessage("OK"), _headerCount(0) {
}

HTTPResponse::HTTPResponse(int statusCode, const String& statusMessage) 
    : _protocol("HTTP/1.1"), _statusCode(statusCode), _headerCount(0) {
    if (statusMessage.length() > 0) {
        _statusMessage = statusMessage;
    } else {
        _statusMessage = getStandardStatusMessage(statusCode);
    }
}

void HTTPResponse::setStatusCode(int statusCode) {
    _statusCode = statusCode;
    if (_statusMessage.length() == 0) {
        _statusMessage = getStandardStatusMessage(statusCode);
    }
}

void HTTPResponse::setStatusMessage(const String& statusMessage) {
    _statusMessage = statusMessage;
}

void HTTPResponse::setProtocol(const String& protocol) {
    _protocol = protocol;
}

void HTTPResponse::setBody(const String& body) {
    _body = body;
    // Automatically set Content-Length header
    setHeader("Content-Length", String(body.length()));
}

void HTTPResponse::addHeader(const String& name, const String& value) {
    if (_headerCount < HTTP_MAX_HEADERS) {
        _headers[_headerCount] = name + ": " + value;
        _headerCount++;
    }
}

void HTTPResponse::setHeader(const String& name, const String& value) {
    // First try to find existing header
    for (uint8_t i = 0; i < _headerCount; i++) {
        if (_headers[i].startsWith(name + ":")) {
            _headers[i] = name + ": " + value;
            return;
        }
    }
    // If not found, add new header
    addHeader(name, value);
}

String HTTPResponse::getHeader(const String& name) {
    for (uint8_t i = 0; i < _headerCount; i++) {
        if (_headers[i].startsWith(name + ":")) {
            int colonIndex = _headers[i].indexOf(':');
            if (colonIndex != -1) {
                String value = _headers[i].substring(colonIndex + 1);
                value.trim();
                return value;
            }
        }
    }
    return "";
}

int HTTPResponse::getStatusCode() const {
    return _statusCode;
}

String HTTPResponse::getStatusMessage() const {
    return _statusMessage;
}

String HTTPResponse::getProtocol() const {
    return _protocol;
}

String HTTPResponse::getBody() const {
    return _body;
}

uint8_t HTTPResponse::getHeaderCount() const {
    return _headerCount;
}

bool HTTPResponse::parseFromString(const String& responseString) {
    int firstLineEnd = responseString.indexOf('\n');
    if (firstLineEnd == -1) return false;
    
    String firstLine = responseString.substring(0, firstLineEnd);
    firstLine.trim();
    
    // Parse status line (e.g., "HTTP/1.1 200 OK")
    int firstSpace = firstLine.indexOf(' ');
    int secondSpace = firstLine.indexOf(' ', firstSpace + 1);
    
    if (firstSpace == -1) return false;
    
    _protocol = firstLine.substring(0, firstSpace);
    
    if (secondSpace == -1) {
        // No status message, just code
        String statusCodeStr = firstLine.substring(firstSpace + 1);
        _statusCode = statusCodeStr.toInt();
        _statusMessage = getStandardStatusMessage(_statusCode);
    } else {
        String statusCodeStr = firstLine.substring(firstSpace + 1, secondSpace);
        _statusCode = statusCodeStr.toInt();
        _statusMessage = firstLine.substring(secondSpace + 1);
    }
    
    // Parse headers
    _headerCount = 0;
    int currentPos = firstLineEnd + 1;
    
    while (currentPos < responseString.length() && _headerCount < HTTP_MAX_HEADERS) {
        int lineEnd = responseString.indexOf('\n', currentPos);
        if (lineEnd == -1) lineEnd = responseString.length();
        
        String line = responseString.substring(currentPos, lineEnd);
        line.trim();
        
        if (line.length() == 0) {
            // Empty line marks end of headers
            currentPos = lineEnd + 1;
            break;
        }
        
        _headers[_headerCount] = line;
        _headerCount++;
        currentPos = lineEnd + 1;
    }
    
    // Rest is body
    if (currentPos < responseString.length()) {
        _body = responseString.substring(currentPos);
    }
    
    return true;
}

String HTTPResponse::toString() const {
    String response = _protocol + " " + String(_statusCode) + " " + _statusMessage + "\r\n";
    
    for (uint8_t i = 0; i < _headerCount; i++) {
        response += _headers[i] + "\r\n";
    }
    
    response += "\r\n";
    
    if (_body.length() > 0) {
        response += _body;
    }
    
    return response;
}

HTTPResponse HTTPResponse::OK(const String& body) {
    HTTPResponse response(200, "OK");
    if (body.length() > 0) {
        response.setBody(body);
    }
    return response;
}

HTTPResponse HTTPResponse::NotFound(const String& body) {
    HTTPResponse response(404, "Not Found");
    response.setBody(body);
    return response;
}

HTTPResponse HTTPResponse::BadRequest(const String& body) {
    HTTPResponse response(400, "Bad Request");
    response.setBody(body);
    return response;
}

HTTPResponse HTTPResponse::InternalServerError(const String& body) {
    HTTPResponse response(500, "Internal Server Error");
    response.setBody(body);
    return response;
}

String HTTPResponse::getStandardStatusMessage(int statusCode) {
    switch (statusCode) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}
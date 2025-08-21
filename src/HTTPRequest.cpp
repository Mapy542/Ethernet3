#include "HTTPRequest.h"

HTTPRequest::HTTPRequest() : _method("GET"), _path("/"), _protocol("HTTP/1.1"), _headerCount(0) {
}

HTTPRequest::HTTPRequest(const String& method, const String& path) 
    : _method(method), _path(path), _protocol("HTTP/1.1"), _headerCount(0) {
}

void HTTPRequest::setMethod(const String& method) {
    _method = method;
}

void HTTPRequest::setPath(const String& path) {
    _path = path;
}

void HTTPRequest::setProtocol(const String& protocol) {
    _protocol = protocol;
}

void HTTPRequest::setBody(const String& body) {
    _body = body;
}

void HTTPRequest::addHeader(const String& name, const String& value) {
    if (_headerCount < 10) {
        _headers[_headerCount] = name + ": " + value;
        _headerCount++;
    }
}

void HTTPRequest::setHeader(const String& name, const String& value) {
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

String HTTPRequest::getHeader(const String& name) {
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

String HTTPRequest::getMethod() const {
    return _method;
}

String HTTPRequest::getPath() const {
    return _path;
}

String HTTPRequest::getProtocol() const {
    return _protocol;
}

String HTTPRequest::getBody() const {
    return _body;
}

uint8_t HTTPRequest::getHeaderCount() const {
    return _headerCount;
}

bool HTTPRequest::parseFromString(const String& requestString) {
    int firstLineEnd = requestString.indexOf('\n');
    if (firstLineEnd == -1) return false;
    
    String firstLine = requestString.substring(0, firstLineEnd);
    firstLine.trim();
    
    // Parse request line (e.g., "GET /path HTTP/1.1")
    int firstSpace = firstLine.indexOf(' ');
    int secondSpace = firstLine.lastIndexOf(' ');
    
    if (firstSpace == -1 || secondSpace == -1 || firstSpace == secondSpace) {
        return false;
    }
    
    _method = firstLine.substring(0, firstSpace);
    _path = firstLine.substring(firstSpace + 1, secondSpace);
    _protocol = firstLine.substring(secondSpace + 1);
    
    // Parse headers
    _headerCount = 0;
    int currentPos = firstLineEnd + 1;
    
    while (currentPos < requestString.length() && _headerCount < 10) {
        int lineEnd = requestString.indexOf('\n', currentPos);
        if (lineEnd == -1) lineEnd = requestString.length();
        
        String line = requestString.substring(currentPos, lineEnd);
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
    if (currentPos < requestString.length()) {
        _body = requestString.substring(currentPos);
    }
    
    return true;
}

String HTTPRequest::toString() const {
    String request = _method + " " + _path + " " + _protocol + "\r\n";
    
    for (uint8_t i = 0; i < _headerCount; i++) {
        request += _headers[i] + "\r\n";
    }
    
    request += "\r\n";
    
    if (_body.length() > 0) {
        request += _body;
    }
    
    return request;
}

HTTPRequest HTTPRequest::GET(const String& path) {
    return HTTPRequest("GET", path);
}

HTTPRequest HTTPRequest::POST(const String& path, const String& body) {
    HTTPRequest request("POST", path);
    if (body.length() > 0) {
        request.setBody(body);
        request.addHeader("Content-Length", String(body.length()));
    }
    return request;
}

HTTPRequest HTTPRequest::PUT(const String& path, const String& body) {
    HTTPRequest request("PUT", path);
    if (body.length() > 0) {
        request.setBody(body);
        request.addHeader("Content-Length", String(body.length()));
    }
    return request;
}

HTTPRequest HTTPRequest::DELETE(const String& path) {
    return HTTPRequest("DELETE", path);
}
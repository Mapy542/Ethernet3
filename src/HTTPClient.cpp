#include "HTTPClient.h"

HTTPClient::HTTPClient(EthernetClass* eth, EthernetChip* chip)
    : _client(eth, chip), _userAgent("Arduino-Ethernet3/1.0"), _timeout(HTTP_DEFAULT_TIMEOUT) {}

void HTTPClient::setUserAgent(const String& userAgent) { _userAgent = userAgent; }

void HTTPClient::setTimeout(unsigned long timeout) { _timeout = timeout; }

bool HTTPClient::connect(const char* host, uint16_t port) { return _client.connect(host, port); }

bool HTTPClient::connect(IPAddress ip, uint16_t port) { return _client.connect(ip, port); }

void HTTPClient::disconnect() { _client.stop(); }

bool HTTPClient::connected() { return _client.connected(); }

HTTPResponse HTTPClient::GET(const String& path) {
    HTTPRequest request = HTTPRequest::GET(path);
    request.addHeader("User-Agent", _userAgent);
    request.addHeader("Connection", "close");
    return sendRequest(request);
}

HTTPResponse HTTPClient::POST(const String& path, const String& body, const String& contentType) {
    HTTPRequest request = HTTPRequest::POST(path, body);
    request.addHeader("User-Agent", _userAgent);
    request.addHeader("Connection", "close");
    if (body.length() > 0) {
        request.addHeader("Content-Type", contentType);
    }
    return sendRequest(request);
}

HTTPResponse HTTPClient::PUT(const String& path, const String& body, const String& contentType) {
    HTTPRequest request = HTTPRequest::PUT(path, body);
    request.addHeader("User-Agent", _userAgent);
    request.addHeader("Connection", "close");
    if (body.length() > 0) {
        request.addHeader("Content-Type", contentType);
    }
    return sendRequest(request);
}

HTTPResponse HTTPClient::DELETE(const String& path) {
    HTTPRequest request = HTTPRequest::DELETE(path);
    request.addHeader("User-Agent", _userAgent);
    request.addHeader("Connection", "close");
    return sendRequest(request);
}

HTTPResponse HTTPClient::sendRequest(const HTTPRequest& request) {
    if (!connected()) {
        return HTTPResponse(0, "Not Connected");
    }

    String requestString = request.toString();

    if (!sendRawRequest(requestString)) {
        return HTTPResponse(0, "Send Failed");
    }

    return readResponseObject();
}

bool HTTPClient::sendRawRequest(const String& requestString) {
    if (!connected()) {
        return false;
    }

    size_t written = _client.print(requestString);
    return written == requestString.length();
}

HTTPResponse HTTPClient::readResponseObject() {
    String responseString = readResponse();
    HTTPResponse response;

    if (responseString.length() == 0) {
        return HTTPResponse(0, "No Response");
    }

    if (!response.parseFromString(responseString)) {
        return HTTPResponse(0, "Parse Error");
    }

    return response;
}

String HTTPClient::readResponse() {
    String response = "";
    unsigned long startTime = millis();

    // Wait for response to start arriving
    while (!_client.available() && (millis() - startTime) < _timeout) {
        delay(1);
    }

    if (!_client.available()) {
        return "";  // Timeout
    }

    // Read response
    while (_client.available() || (millis() - startTime) < _timeout) {
        if (_client.available()) {
            char c = _client.read();
            response += c;

            // Reset timeout on each character received
            startTime = millis();
        } else {
            delay(1);
        }
    }

    return response;
}

bool HTTPClient::parseURL(const String& url, String& protocol, String& host, uint16_t& port,
                          String& path) {
    // Parse URL like "http://example.com:8080/path"
    protocol = "";
    host = "";
    port = 80;
    path = "/";

    int protocolEnd = url.indexOf("://");
    if (protocolEnd == -1) {
        // No protocol specified, assume http
        protocol = "http";
        protocolEnd = -3;  // Adjust for missing ://
    } else {
        protocol = url.substring(0, protocolEnd);
        protocol.toLowerCase();
    }

    int hostStart = protocolEnd + 3;
    int pathStart = url.indexOf('/', hostStart);
    if (pathStart == -1) {
        pathStart = url.length();  // Shouldn't allow negative index
    }

    String hostAndPort = url.substring(hostStart, pathStart);

    // Extract port if specified
    int portSeparator = hostAndPort.indexOf(':');
    if (portSeparator != -1) {
        host = hostAndPort.substring(0, portSeparator);
        String portStr = hostAndPort.substring(portSeparator + 1);
        port = portStr.toInt();
        if (port == 0) {
            port = (protocol == "https") ? 443 : 80;
        }
    } else {
        host = hostAndPort;
        port = (protocol == "https") ? 443 : 80;
    }

    if (static_cast<unsigned int>(pathStart) < url.length()) {
        path = url.substring(pathStart);
    }

    return host.length() > 0;
}

HTTPResponse HTTPClient::request(const String& method, const String& url, const String& body) {
    String protocol, host, path;
    uint16_t port;

    if (!parseURL(url, protocol, host, port, path)) {
        return HTTPResponse(0, "Invalid URL");
    }

    // Note: This implementation only supports HTTP, not HTTPS
    if (protocol != "http") {
        return HTTPResponse(0, "Protocol Not Supported");
    }

    if (!connect(host.c_str(), port)) {
        return HTTPResponse(0, "Connection Failed");
    }

    HTTPResponse response;

    if (method == "GET") {
        response = GET(path);
    } else if (method == "POST") {
        response = POST(path, body);
    } else if (method == "PUT") {
        response = PUT(path, body);
    } else if (method == "DELETE") {
        response = DELETE(path);
    } else {
        HTTPRequest request(method, path);
        request.addHeader("User-Agent", _userAgent);
        request.addHeader("Connection", "close");
        if (body.length() > 0) {
            request.setBody(body);
        }
        response = sendRequest(request);
    }

    disconnect();
    return response;
}
#include "HTTPServer.h"

HTTPServer::HTTPServer(EthernetClass* eth, EthernetChip* chip, uint16_t port)
    : _server(eth, chip, port), _routeCount(0), _defaultHandler(nullptr) {
}

void HTTPServer::begin() {
    _server.begin();
}

void HTTPServer::handleClient() {
    EthernetClient client = _server.available();
    if (client) {
        String requestString = readRequestFromClient(client);
        
        if (requestString.length() > 0) {
            HTTPRequest request;
            if (request.parseFromString(requestString)) {
                HTTPResponse response;
                Route matchedRoute;
                
                if (matchRoute(request.getMethod(), request.getPath(), matchedRoute)) {
                    // Call the matched route handler
                    response = matchedRoute.handler(request);
                } else if (_defaultHandler != nullptr) {
                    // Call the default handler
                    response = _defaultHandler(request);
                } else {
                    // Use built-in 404 handler
                    response = defaultNotFoundHandler(request);
                }
                
                sendResponseToClient(client, response);
            } else {
                // Failed to parse request
                HTTPResponse errorResponse = HTTPResponse::BadRequest("Invalid HTTP Request");
                sendResponseToClient(client, errorResponse);
            }
        }
        
        // Give client time to receive data
        delay(1);
        client.stop();
    }
}

void HTTPServer::on(const String& method, const String& path, RequestHandler handler) {
    if (_routeCount < HTTP_MAX_ROUTES) {
        _routes[_routeCount].method = method;
        _routes[_routeCount].path = path;
        _routes[_routeCount].handler = handler;
        _routeCount++;
    }
}

void HTTPServer::onGET(const String& path, RequestHandler handler) {
    on("GET", path, handler);
}

void HTTPServer::onPOST(const String& path, RequestHandler handler) {
    on("POST", path, handler);
}

void HTTPServer::onPUT(const String& path, RequestHandler handler) {
    on("PUT", path, handler);
}

void HTTPServer::onDELETE(const String& path, RequestHandler handler) {
    on("DELETE", path, handler);
}

void HTTPServer::onNotFound(RequestHandler handler) {
    _defaultHandler = handler;
}

String HTTPServer::readRequestFromClient(EthernetClient& client) {
    String request = "";
    bool headerComplete = false;
    int contentLength = 0;
    int bodyBytesRead = 0;
    
    unsigned long timeout = millis() + 5000; // 5 second timeout
    
    while (client.connected() && millis() < timeout) {
        if (client.available()) {
            char c = client.read();
            request += c;
            
            if (!headerComplete) {
                // Check for end of headers (double CRLF)
                if (request.endsWith("\r\n\r\n")) {
                    headerComplete = true;
                    
                    // Parse Content-Length header if present
                    int contentLengthIndex = request.indexOf("Content-Length:");
                    if (contentLengthIndex != -1) {
                        int valueStart = contentLengthIndex + 15; // Length of "Content-Length:"
                        int lineEnd = request.indexOf('\r', valueStart);
                        if (lineEnd != -1) {
                            String lengthStr = request.substring(valueStart, lineEnd);
                            lengthStr.trim();
                            contentLength = lengthStr.toInt();
                        }
                    }
                    
                    // If no content length, we're done
                    if (contentLength == 0) {
                        break;
                    }
                }
            } else {
                // Reading body
                bodyBytesRead++;
                if (bodyBytesRead >= contentLength) {
                    break;
                }
            }
            
            // Reset timeout when data is received
            timeout = millis() + 5000;
        } else {
            delay(1);
        }
    }
    
    return request;
}

void HTTPServer::sendResponseToClient(EthernetClient& client, const HTTPResponse& response) {
    String responseString = response.toString();
    client.print(responseString);
}

bool HTTPServer::matchRoute(const String& method, const String& path, Route& matchedRoute) {
    for (uint8_t i = 0; i < _routeCount; i++) {
        if (_routes[i].method == method && _routes[i].path == path) {
            matchedRoute = _routes[i];
            return true;
        }
    }
    return false;
}

HTTPResponse HTTPServer::defaultNotFoundHandler(const HTTPRequest& request) {
    String body = "<!DOCTYPE html><html><head><title>404 Not Found</title></head>";
    body += "<body><h1>404 - Not Found</h1>";
    body += "<p>The requested resource " + request.getPath() + " was not found on this server.</p>";
    body += "</body></html>";
    
    HTTPResponse response = HTTPResponse::NotFound(body);
    response.setHeader("Content-Type", "text/html");
    return response;
}

HTTPResponse HTTPServer::send(int statusCode, const String& contentType, const String& content) {
    HTTPResponse response(statusCode);
    response.setHeader("Content-Type", contentType);
    response.setBody(content);
    return response;
}

HTTPResponse HTTPServer::sendHTML(const String& html) {
    return send(200, "text/html", html);
}

HTTPResponse HTTPServer::sendJSON(const String& json) {
    return send(200, "application/json", json);
}

HTTPResponse HTTPServer::sendPlain(const String& text) {
    return send(200, "text/plain", text);
}
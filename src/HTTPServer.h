#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "Arduino.h"
#include "EthernetServer.h"
#include "EthernetClient.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

// Forward declaration for request handler function type
typedef HTTPResponse (*RequestHandler)(const HTTPRequest& request);

struct Route {
    String method;
    String path;
    RequestHandler handler;
};

class HTTPServer {
private:
    EthernetServer _server;
    Route _routes[10];  // Support up to 10 routes
    uint8_t _routeCount;
    RequestHandler _defaultHandler;
    
    // Helper methods
    String readRequestFromClient(EthernetClient& client);
    void sendResponseToClient(EthernetClient& client, const HTTPResponse& response);
    bool matchRoute(const String& method, const String& path, Route& matchedRoute);
    HTTPResponse defaultNotFoundHandler(const HTTPRequest& request);
    
public:
    HTTPServer(EthernetClass* eth, EthernetChip* chip, uint16_t port = 80);
    
    // Server lifecycle
    void begin();
    void handleClient();
    
    // Route management
    void on(const String& method, const String& path, RequestHandler handler);
    void onGET(const String& path, RequestHandler handler);
    void onPOST(const String& path, RequestHandler handler);
    void onPUT(const String& path, RequestHandler handler);
    void onDELETE(const String& path, RequestHandler handler);
    
    // Default handler for unmatched routes
    void onNotFound(RequestHandler handler);
    
    // Static utility methods
    static HTTPResponse send(int statusCode, const String& contentType, const String& content);
    static HTTPResponse sendHTML(const String& html);
    static HTTPResponse sendJSON(const String& json);
    static HTTPResponse sendPlain(const String& text);
};

#endif
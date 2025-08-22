# HTTP Implementation Guide for Ethernet3

This guide provides comprehensive information about the HTTP protocol implementation in Ethernet3, including technical details, usage patterns, and configuration options.

## Overview

The HTTP implementation provides a high-level interface for HTTP communication while leveraging the existing proven TCP stack in Ethernet3. It consists of four main classes that work together to provide both client and server HTTP functionality:

- **HTTPRequest**: Represents and parses HTTP requests
- **HTTPResponse**: Represents and builds HTTP responses  
- **HTTPClient**: Provides HTTP client functionality
- **HTTPServer**: Provides HTTP server functionality with routing

## Design Principles

- **Built on existing proven TCP implementation** - Leverages the tested and reliable TCP socket functionality
- **Maintains full backward compatibility** - Existing TCP/UDP code continues to work unchanged
- **Memory-conscious design for Arduino constraints** - Configurable limits to fit available memory
- **Simple, intuitive API** - Easy-to-use interface similar to popular HTTP libraries
- **Configurable memory limits** - Adjust memory usage based on your project's needs

## Architecture

The HTTP implementation leverages the existing Ethernet3 architecture:

- **HTTPClient** wraps `EthernetClient` for HTTP request functionality
- **HTTPServer** wraps `EthernetServer` to provide HTTP response handling
- Both classes maintain the same constructor patterns as existing Ethernet classes
- All HTTP functionality is built on top of the proven TCP socket implementation

## Memory Management

The implementation is designed for Arduino memory constraints with configurable limits:

### Default Memory Limits

```cpp
#define HTTP_MAX_HEADERS 8         // Maximum headers per request/response
#define HTTP_MAX_ROUTES 8          // Maximum server routes
#define HTTP_MAX_BODY_SIZE 1024    // Maximum body size in bytes
#define HTTP_DEFAULT_TIMEOUT 5000  // Default timeout in milliseconds
```

### Typical Memory Usage

- **HTTPClient**: ~200 bytes + dynamic strings
- **HTTPServer**: ~300 bytes + routes + dynamic strings  
- **HTTPRequest**: ~100 bytes + headers + body
- **HTTPResponse**: ~100 bytes + headers + body

String usage is the main memory consumer - consider using shorter strings in production on memory-constrained devices.

### Configuring Memory Limits

Edit `HTTPConfig.h` to adjust memory limits based on your available memory:

```cpp
#define HTTP_MAX_HEADERS 10        // Support more headers
#define HTTP_MAX_BODY_SIZE 2048    // Allow larger request/response bodies
#define HTTP_MAX_ROUTES 15         // Support more server routes
#define HTTP_DEFAULT_TIMEOUT 10000 // Longer timeout for slow connections
```

## HTTP Client Usage

### Basic Client Operations

```cpp
#include <Ethernet3.h>
#include <HTTP.h>

// Initialize hardware
W5500 chip(10);
EthernetClass ethernet(&chip);
HTTPClient client(&ethernet, &chip);

void setup() {
    // Initialize network
    uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    ethernet.begin(mac);
}

void loop() {
    // Simple URL-based request
    HTTPResponse response = client.request("GET", "http://example.com/api/data");
    
    if (response.getStatusCode() == 200) {
        Serial.println("Success!");
        Serial.println(response.getBody());
    } else {
        Serial.print("Error: ");
        Serial.println(response.getStatusCode());
    }
    
    delay(5000);
}
```

### Manual Connection Management

For multiple requests to the same server, maintain a connection:

```cpp
void multipleRequests() {
    if (client.connect("api.example.com", 80)) {
        HTTPResponse resp1 = client.GET("/endpoint1");
        HTTPResponse resp2 = client.POST("/endpoint2", "{\"key\":\"value\"}", "application/json");
        HTTPResponse resp3 = client.PUT("/endpoint3", "updated data", "text/plain");
        
        client.disconnect();
    }
}
```

### Different HTTP Methods

```cpp
// GET request
HTTPResponse response = client.GET("/api/users/123");

// POST with JSON data
String jsonData = "{\"name\":\"Arduino\",\"type\":\"microcontroller\"}";
HTTPResponse response = client.POST("/api/devices", jsonData, "application/json");

// PUT request
HTTPResponse response = client.PUT("/api/config", "new_value=123", "application/x-www-form-urlencoded");

// DELETE request
HTTPResponse response = client.DELETE("/api/items/456");
```

## HTTP Server Usage

### Basic Server Setup

```cpp
#include <Ethernet3.h>
#include <HTTP.h>

// Initialize hardware
W5500 chip(10);
EthernetClass ethernet(&chip);
HTTPServer server(&ethernet, &chip, 80);

// Handler functions
HTTPResponse handleRoot(const HTTPRequest& request) {
    return HTTPServer::sendHTML("<h1>Welcome to Arduino Web Server</h1>");
}

HTTPResponse handleAPI(const HTTPRequest& request) {
    return HTTPServer::sendJSON("{\"status\":\"ok\",\"device\":\"Arduino\"}");
}

HTTPResponse handle404(const HTTPRequest& request) {
    return HTTPServer::sendError(404, "Page not found");
}

void setup() {
    // Initialize network
    uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    ethernet.begin(mac);
    
    // Set up routes
    server.onGET("/", handleRoot);
    server.onGET("/api/status", handleAPI);
    server.onNotFound(handle404);
    
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}
```

### Advanced Routing

```cpp
// Different methods for the same path
server.onGET("/api/data", handleGetData);
server.onPOST("/api/data", handlePostData);
server.onPUT("/api/data", handleUpdateData);
server.onDELETE("/api/data", handleDeleteData);

// RESTful API pattern
server.onGET("/api/sensors", handleListSensors);
server.onGET("/api/sensors/temperature", handleTemperature);
server.onGET("/api/sensors/humidity", handleHumidity);
server.onPOST("/api/config", handleConfigUpdate);
```

### Working with Request Data

```cpp
HTTPResponse handleConfigUpdate(const HTTPRequest& request) {
    // Check request method
    if (request.getMethod() != "POST") {
        return HTTPServer::sendError(405, "Method not allowed");
    }
    
    // Access headers
    String contentType = request.getHeader("Content-Type");
    if (contentType != "application/json") {
        return HTTPServer::sendError(400, "JSON required");
    }
    
    // Process request body
    String body = request.getBody();
    // Parse JSON, update configuration, etc.
    
    // Return success response
    HTTPResponse response = HTTPServer::sendJSON("{\"result\":\"success\"}");
    response.setHeader("Location", "/api/config");
    return response;
}
```

## Common Usage Patterns

### RESTful API Client

```cpp
class APIClient {
private:
    HTTPClient& client;
    String baseURL;
    
public:
    APIClient(HTTPClient& httpClient, String url) : client(httpClient), baseURL(url) {}
    
    HTTPResponse getUser(int userId) {
        String url = baseURL + "/users/" + String(userId);
        return client.GET(url);
    }
    
    HTTPResponse createUser(String userData) {
        String url = baseURL + "/users";
        return client.POST(url, userData, "application/json");
    }
    
    HTTPResponse updateUser(int userId, String userData) {
        String url = baseURL + "/users/" + String(userId);
        return client.PUT(url, userData, "application/json");
    }
    
    HTTPResponse deleteUser(int userId) {
        String url = baseURL + "/users/" + String(userId);
        return client.DELETE(url);
    }
};
```

### IoT Device Web Interface

```cpp
HTTPResponse handleDeviceStatus(const HTTPRequest& request) {
    String html = "<!DOCTYPE html><html><head><title>Device Status</title></head><body>";
    html += "<h1>Arduino Device Status</h1>";
    html += "<p>Uptime: " + String(millis() / 1000) + " seconds</p>";
    html += "<p>Free Memory: " + String(freeMemory()) + " bytes</p>";
    html += "<p>Temperature: " + String(readTemperature()) + " °C</p>";
    html += "</body></html>";
    
    return HTTPServer::sendHTML(html);
}

HTTPResponse handleSensorData(const HTTPRequest& request) {
    String json = "{";
    json += "\"temperature\":" + String(readTemperature()) + ",";
    json += "\"humidity\":" + String(readHumidity()) + ",";
    json += "\"timestamp\":" + String(millis());
    json += "}";
    
    HTTPResponse response = HTTPServer::sendJSON(json);
    response.setHeader("Access-Control-Allow-Origin", "*"); // CORS for web apps
    return response;
}
```

## Configuration Options

### Timeout Settings

```cpp
// Set custom timeout for slow servers
client.setTimeout(10000); // 10 second timeout

// Check current timeout
unsigned long timeout = client.getTimeout();
```

### Custom Headers

```cpp
// Client request with custom headers
HTTPResponse response = client.GET("/api/data");
// Headers are automatically managed, but you can access them in responses

// Server response with custom headers
HTTPResponse response = HTTPServer::sendJSON(data);
response.setHeader("Cache-Control", "no-cache");
response.setHeader("Access-Control-Allow-Origin", "*");
return response;
```

## Limitations and Considerations

### Current Limitations

- **HTTP only** - No HTTPS/TLS support
- **Basic header parsing** - No advanced HTTP features like chunked encoding
- **Memory-constrained parsing** - Suitable for Arduino but limited compared to full HTTP libraries
- **Limited concurrent connections** - Inherited from underlying TCP layer

### Best Practices

1. **Memory Management**: Monitor memory usage, especially with long strings and multiple concurrent connections
2. **Error Handling**: Always check HTTP status codes and handle network failures gracefully
3. **Timeouts**: Use appropriate timeouts for your network conditions
4. **Connection Management**: Close connections when done to free resources
5. **String Usage**: Keep strings as short as possible on memory-constrained devices

### Performance Tips

- Use connection reuse for multiple requests to the same server
- Keep request/response bodies small
- Limit the number of HTTP headers
- Consider using shorter URLs and paths
- Monitor and limit the number of concurrent server connections

## Troubleshooting

### Common Issues

**Problem**: HTTP requests timeout or fail
**Solution**: Check network connectivity, increase timeout, verify server availability

**Problem**: Server runs out of memory
**Solution**: Reduce HTTP_MAX_BODY_SIZE, HTTP_MAX_HEADERS, limit concurrent connections

**Problem**: Responses are truncated
**Solution**: Increase HTTP_MAX_BODY_SIZE in HTTPConfig.h

**Problem**: Server doesn't respond to requests
**Solution**: Ensure server.handleClient() is called regularly in loop(), check network configuration

### Debugging

Enable debugging output to monitor HTTP operations:

```cpp
void debugResponse(const HTTPResponse& response) {
    Serial.print("Status Code: ");
    Serial.println(response.getStatusCode());
    Serial.print("Body Length: ");
    Serial.println(response.getBody().length());
    Serial.print("Headers: ");
    Serial.println(response.getHeaderCount());
}
```

## Examples

Complete examples are available in the `examples/` directory:

- **HTTPClient/** - Complete HTTP client usage example
- **HTTPServer/** - Complete HTTP server with routing example  
- **HTTPBasicTest/** - Basic compilation and functionality test

These examples demonstrate real-world usage patterns and can serve as starting points for your projects.

## Compatibility

This HTTP implementation maintains 100% backward compatibility with existing Ethernet3 functionality:

- Existing TCP/UDP code continues to work unchanged
- No modifications to existing Ethernet3 classes
- HTTP functionality is completely optional
- Same chip support (W5100, W5500, etc.)

The HTTP implementation provides a solid foundation for IoT projects, web-based device control, RESTful APIs, and other HTTP-based communication needs while maintaining the reliability and performance of the existing Ethernet3 TCP stack.
# HTTP Protocol Implementation for Ethernet3

This document describes the HTTP protocol implementation built on top of the Ethernet3 library's TCP functionality.

## Overview

The HTTP implementation provides a high-level interface for HTTP communication while leveraging the existing proven TCP stack in Ethernet3. It consists of four main classes:

- **HTTPRequest**: Represents and parses HTTP requests
- **HTTPResponse**: Represents and builds HTTP responses  
- **HTTPClient**: Provides HTTP client functionality
- **HTTPServer**: Provides HTTP server functionality with routing

## Key Features

- Built on top of existing TCP implementation
- Support for common HTTP methods (GET, POST, PUT, DELETE)
- Basic header parsing and generation
- Simple routing system for HTTP server
- Memory-conscious design for Arduino constraints
- URL parsing utilities
- Configurable buffer sizes

## Basic Usage

### HTTP Client

```cpp
#include <Ethernet3.h>
#include <HTTP.h>

// Initialize Ethernet and HTTP client
W5500 chip(10);
EthernetClass Ethernet(&chip);
HTTPClient client(&Ethernet, &chip);

// Make HTTP requests
HTTPResponse response = client.request("GET", "http://example.com/api");
Serial.println(response.getBody());
```

### HTTP Server

```cpp
#include <Ethernet3.h>
#include <HTTP.h>

// Initialize Ethernet and HTTP server
W5500 chip(10);
EthernetClass Ethernet(&chip);
HTTPServer server(&Ethernet, &chip, 80);

// Set up routes
server.onGET("/", handleRoot);
server.onPOST("/api", handleAPI);

void setup() {
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() {
  server.handleClient();
}

HTTPResponse handleRoot(const HTTPRequest& request) {
  return HTTPServer::sendHTML("<h1>Hello World</h1>");
}
```

## Memory Considerations

The implementation is designed to work within Arduino memory constraints:

- Configurable maximum header count (default: 8)
- Configurable maximum body size (default: 1024 bytes)  
- Configurable request buffer size (default: 512 bytes)
- Limited route count for server (default: 8)

These limits can be adjusted in `HTTPConfig.h` if more memory is available.

## Configuration

Edit `HTTPConfig.h` to adjust memory limits:

```cpp
#define HTTP_MAX_HEADERS 10        // More headers
#define HTTP_MAX_BODY_SIZE 2048    // Larger bodies
#define HTTP_MAX_ROUTES 15         // More server routes
```

## API Reference

### HTTPClient Methods

- `connect(host, port)` - Connect to server
- `GET(path)` - Make GET request
- `POST(path, body, contentType)` - Make POST request
- `PUT(path, body, contentType)` - Make PUT request  
- `DELETE(path)` - Make DELETE request
- `request(method, url, body)` - Generic request method

### HTTPServer Methods

- `begin()` - Start the server
- `handleClient()` - Process incoming requests (call in loop)
- `onGET(path, handler)` - Register GET route
- `onPOST(path, handler)` - Register POST route
- `onPUT(path, handler)` - Register PUT route
- `onDELETE(path, handler)` - Register DELETE route
- `onNotFound(handler)` - Register 404 handler

### HTTPRequest Methods

- `getMethod()` - Get HTTP method
- `getPath()` - Get request path
- `getBody()` - Get request body
- `getHeader(name)` - Get header value
- `setHeader(name, value)` - Set header value

### HTTPResponse Methods

- `getStatusCode()` - Get HTTP status code
- `getBody()` - Get response body
- `setBody(body)` - Set response body  
- `setHeader(name, value)` - Set header value
- `toString()` - Generate HTTP response string

## Examples

See the `examples/` directory for complete usage examples:

- `HTTPClient/` - HTTP client usage
- `HTTPServer/` - HTTP server with routing
- `HTTPBasicTest/` - Basic compilation test

## Limitations

- HTTP only (no HTTPS support)
- Limited concurrent connections (inherited from TCP layer)
- Memory-constrained parsing (suitable for Arduino)
- Basic header parsing (no advanced HTTP features)

## Compatibility

This implementation maintains full backward compatibility with existing Ethernet3 TCP functionality. Existing TCP code will continue to work unchanged.
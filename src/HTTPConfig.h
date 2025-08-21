#ifndef HTTP_CONFIG_H
#define HTTP_CONFIG_H

/*
 * HTTP Library Configuration
 * 
 * This file contains configuration constants for the HTTP library
 * to allow tuning for different Arduino memory configurations.
 */

// Maximum number of headers per request/response
#ifndef HTTP_MAX_HEADERS
#define HTTP_MAX_HEADERS 8
#endif

// Maximum URL length for parsing
#ifndef HTTP_MAX_URL_LENGTH  
#define HTTP_MAX_URL_LENGTH 256
#endif

// Maximum body size for requests/responses
#ifndef HTTP_MAX_BODY_SIZE
#define HTTP_MAX_BODY_SIZE 1024
#endif

// HTTP request timeout in milliseconds
#ifndef HTTP_DEFAULT_TIMEOUT
#define HTTP_DEFAULT_TIMEOUT 5000
#endif

// Maximum routes for HTTP server
#ifndef HTTP_MAX_ROUTES
#define HTTP_MAX_ROUTES 8
#endif

// Buffer size for reading HTTP requests
#ifndef HTTP_REQUEST_BUFFER_SIZE
#define HTTP_REQUEST_BUFFER_SIZE 512
#endif

// Common HTTP strings (stored in PROGMEM to save RAM)
#define HTTP_VERSION_1_1 "HTTP/1.1"
#define HTTP_METHOD_GET "GET"
#define HTTP_METHOD_POST "POST"
#define HTTP_METHOD_PUT "PUT"
#define HTTP_METHOD_DELETE "DELETE"

#define HTTP_HEADER_CONTENT_TYPE "Content-Type"
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"
#define HTTP_HEADER_CONNECTION "Connection"
#define HTTP_HEADER_USER_AGENT "User-Agent"

#define HTTP_CONTENT_TYPE_HTML "text/html"
#define HTTP_CONTENT_TYPE_JSON "application/json"
#define HTTP_CONTENT_TYPE_PLAIN "text/plain"

#endif
#ifndef HTTP_H
#define HTTP_H

/*
 * HTTP Protocol Implementation for Ethernet3 Library
 * 
 * This library provides HTTP client and server functionality
 * built on top of the existing TCP implementation in Ethernet3.
 * 
 * Features:
 * - HTTPClient for making HTTP requests (GET, POST, PUT, DELETE)
 * - HTTPServer for handling HTTP requests with routing
 * - HTTPRequest and HTTPResponse classes for message parsing
 * - Support for common HTTP headers and status codes
 * - URL parsing utilities
 * 
 * Usage:
 * #include <Ethernet3.h>
 * #include <HTTP.h>
 */

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HTTPClient.h"
#include "HTTPServer.h"

#endif
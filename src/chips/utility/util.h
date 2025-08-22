#ifndef UTIL_H
#define UTIL_H

// Cast to a larger type (uint32_t) before shifting to prevent overflow on 16-bit platforms.
#define htons(x) ((uint16_t)((((uint32_t)(x)) << 8 & 0xFF00) | (((uint32_t)(x)) >> 8 & 0x00FF)))
#define ntohs(x) htons(x)

#define htonl(x)                                                                      \
    ((((uint32_t)(x)) << 24 & 0xFF000000UL) | (((uint32_t)(x)) << 8 & 0x00FF0000UL) | \
     (((uint32_t)(x)) >> 8 & 0x0000FF00UL) | (((uint32_t)(x)) >> 24 & 0x000000FFUL))
#define ntohl(x) htonl(x)

#endif
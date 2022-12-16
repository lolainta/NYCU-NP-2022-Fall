#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <cstdint>

// Assume content size is less than 32MB
#define PAYLOAD_SIZE 33554432

struct dataPayload {
    uint8_t notUsed[8];
    struct fileEntry {
        uint32_t contentOffset;
        uint16_t size;
        // uint16_t checksum;
    } __attribute__((packed)) fileEntries[1000];
    uint8_t content[PAYLOAD_SIZE];
} __attribute__((packed));

#endif

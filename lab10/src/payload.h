#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <cstdint>

#define PAYLOAD_SIZE 33554432

struct dataPayload{
    struct fileEntry{
        uint32_t contentOffset;
        uint16_t size;
    }__attribute__((packed))fileEntries[1000];
    uint8_t content[PAYLOAD_SIZE];
}__attribute__((packed));

#endif

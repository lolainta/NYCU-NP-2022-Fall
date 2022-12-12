#ifndef REQUEST_H
#define REQUEST

#include <cstdint>

typedef struct __attribute__((packed))request{
    uint32_t seq;
	uint8_t flag;
    request();
    request(uint32_t);
}request;

#endif

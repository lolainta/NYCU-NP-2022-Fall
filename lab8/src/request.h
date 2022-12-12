#ifndef REQUEST_H
#define REQUEST

#include <cstdint>

typedef struct __attribute__((packed))request{
    uint32_t seq;
    uint32_t par;
	uint8_t flag;
    request();
    request(uint32_t);
    request(uint32_t,uint32_t);
}request;

#endif

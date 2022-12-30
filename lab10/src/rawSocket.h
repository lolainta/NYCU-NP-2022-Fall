#ifndef RAW_SOCKET_H
#define RAW_SOCKET_H

#include <linux/ip.h>
#include <unistd.h>
#include <cstdint>

class RawSocket{
    unsigned int fragSize;
    char*dst_ip;
    iphdr*iph;
    int sock;
    void gen_iphdr(size_t);
public:
    RawSocket();
    void set_iphdr(char*,unsigned int);
    ssize_t send(uint8_t*,size_t);
    ssize_t read(uint8_t*,size_t);
};

#endif

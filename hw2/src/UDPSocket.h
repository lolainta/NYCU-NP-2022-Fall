#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <cstdint>
#include <arpa/inet.h>

class UDPSocket{
private:
    int port;
    char*ip;
    sockaddr_in sin;
    sockaddr_in csin;
    socklen_t csinlen=sizeof(sockaddr_in);
    int sock;
public:
    UDPSocket(int);
    UDPSocket(const char*,int);
    ssize_t send(uint8_t*,size_t);
    ssize_t read(uint8_t*,size_t);
    ssize_t reply(uint8_t*,size_t);
};

#endif

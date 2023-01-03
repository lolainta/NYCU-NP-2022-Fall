#include "UDPSocket.h"
#include <iostream>
#include <cstring>

UDPSocket::UDPSocket(int port){
    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
 	if((sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
        perror("socket");
        exit(1);
    }
    if(bind(sock,(sockaddr*)&sin,sizeof(sin))<0){
        perror("bind");
        exit(1);
    }
}

UDPSocket::UDPSocket(const char*ip,int _port){
    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(_port);
    sin.sin_addr.s_addr=inet_addr(ip);
    if((sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
        perror("socket");
        exit(1);
    }
}

ssize_t UDPSocket::send(uint8_t*msg,size_t len){
    ssize_t slen;
    if((slen=sendto(sock,msg,len,0,(sockaddr*)&sin,sizeof(sin)))<0){
        perror("sendto");
        exit(1);
    }
    return slen;
}

ssize_t UDPSocket::read(uint8_t*msg,size_t len){
	ssize_t rlen;
	if((rlen=recvfrom(sock,msg,len,0,(sockaddr*)&csin,&csinlen))<0){
        perror("recvfrom");
        exit(1);
    }
	return rlen;
}
ssize_t UDPSocket::reply(uint8_t*msg,size_t len){
	ssize_t rlen;
	if((rlen=sendto(sock,msg,len,0,(sockaddr*)&csin,sizeof(csin)))<0){
		perror("sendto");
		exit(1);
	}
	return rlen;
}

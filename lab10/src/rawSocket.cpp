#include "rawSocket.h"
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

RawSocket::RawSocket(bool mode,int channel):protocol(channel){
	if((sock=socket(AF_INET,SOCK_RAW,protocol))<0){
        perror("socket");
        exit(1);
    }
    if(mode==1){
        int yes=1;
        setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&yes,sizeof(yes));
        setsockopt(sock,IPPROTO_IP,IP_HDRINCL,&yes,sizeof(yes));
    }
}

void RawSocket::set_iphdr(char*ip){
    dst_ip=ip;
    struct sockaddr_in dst;
    bzero(&dst,sizeof(dst));
    dst.sin_family=AF_INET;
    dst.sin_addr.s_addr=inet_addr(ip);

    if(connect(sock,(sockaddr*)&dst,sizeof(dst))<0){
        perror("connect");
        exit(1);
	}
}

void RawSocket::gen_iphdr(size_t len){
    iph->ihl=5;
    iph->version=4;
    iph->tos=0;
    iph->tot_len=sizeof(iphdr)+len;
    iph->id=htonl(rand());
    iph->frag_off=0;
    iph->ttl=255;
    iph->protocol=protocol;
    iph->check=0;
    iph->saddr=inet_addr("255.255.255.255");
    iph->daddr=inet_addr(dst_ip);
}

ssize_t RawSocket::send(uint8_t*msg,size_t len){
    uint8_t*pkt=(uint8_t*)calloc(sizeof(iphdr)+len,1);
    iph=(iphdr*)pkt;
    gen_iphdr(len);
    memcpy(pkt+sizeof(iphdr),msg,len);
    ssize_t ret=write(sock,pkt,sizeof(iphdr)+len);
    free(pkt);
    return ret;
}

ssize_t RawSocket::read(uint8_t*msg,size_t len){
    uint8_t*pkt=(uint8_t*)calloc(sizeof(iphdr)+len,1);
    ssize_t ret=recv(sock,pkt,sizeof(iphdr)+len,MSG_TRUNC);
    memcpy(msg,pkt+sizeof(iphdr),ret-sizeof(iphdr));
    free(pkt);
    return ret-sizeof(iphdr);
}

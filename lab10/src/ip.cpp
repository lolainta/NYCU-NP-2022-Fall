#ifndef IP_H
#define IP_H
#include <linux/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void fill(iphdr*iph,char*dst_ip,int len){
	iph->ihl=5;
    iph->version=4;
    iph->tos=0;
    iph->tot_len=sizeof(iphdr)+len;
    iph->id=htonl(54321);
    iph->frag_off=0;
    iph->ttl=255;
    iph->protocol=PROTOCOL;
    iph->check=0;
    iph->saddr=inet_addr("255.255.255.255");
    iph->daddr=inet_addr("dst_ip");
}

#endif
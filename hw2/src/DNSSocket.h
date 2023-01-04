#ifndef DNSSOCKET_H
#define DNSSOCK_H

#include "UDPSocket.h"
#include "message.h"

using namespace std;

class DNSSocket:public UDPSocket{
    uint8_t last[1000];
    ssize_t llen;
    void dump(uint8_t*,ssize_t);
    void parse(Message&,uint8_t*);
    size_t parseHeader(Message&,uint8_t*);
    size_t parseName(vector<string>&,uint8_t*,size_t);
    size_t parseQuestion(Question&,uint8_t*,size_t);
    size_t parseResourceRecord(Message&,uint8_t*,size_t);
    size_t parseAnswer(Message&,uint8_t*,size_t);
    size_t parseAuthority(Message&,uint8_t*,size_t);
    size_t parseAdditional(Message&,uint8_t*,size_t);

    size_t gen(uint8_t*,const Message&);
    size_t genHeader(uint8_t*,const Header&);
    size_t genName(uint8_t*,const vector<string>&);
    size_t genQuestion(uint8_t*,const Question&);
    size_t genResourceRecord(uint8_t*,const ResourceRecord&);
    size_t genAnswer(uint8_t*,const ResourceRecord&);
    size_t genAuthority(uint8_t*,const ResourceRecord&);
    size_t genAdditional(uint8_t*,const ResourceRecord&);
public:
    DNSSocket(int);
    Message get();
    size_t put(const Message&);
    void info(const Message&)const;
    pair<uint8_t*,ssize_t> getLast();
};

#endif

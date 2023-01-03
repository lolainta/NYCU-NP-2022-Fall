#include <cstring>

#include "message.h"
#include "types.h"

Question::Question(){
    this->qname.clear();
}

Question::Question(const Question&q){
    this->qname=q.qname;
    this->qtype=q.qtype;
    this->qclass=q.qclass;
}

Message::Message(size_t qd,size_t an,size_t ns, size_t ar){
    memset(&hdr,0,sizeof(hdr));
    hdr.id=rand();
    hdr.qdcount=qd;
    hdr.ancount=an;
    hdr.nscount=ns;
    hdr.arcount=ar;
    ques.resize(qd);
    ans.resize(an);
    auth.resize(ns);
    add.resize(ar);
}

Message::Message():Message(0,0,0,0){}
/*
bool Message::operator ()const{
    return true;
}
*/

ResourceRecord::ResourceRecord(){};
ResourceRecord::ResourceRecord(TYPE _type,uint32_t _ttl):rtype(_type),ttl(_ttl),rclass(CLASS::IN){
    switch(_type){
    case TYPE::CNAME:
        this->cname=new CNAME_t;
        break;
    case TYPE::MX:
        this->mx=new MX_t;
        break;
    case TYPE::NS:
        this->ns=new NS_t;
        break;
    case TYPE::SOA:
        this->soa=new SOA_t;
        break;
    case TYPE::TXT:
        this->txt=new TXT_t;
        break;
    case TYPE::A:
        this->a=new A_t;
        break;
    case TYPE::AAAA:
        this->aaaa=new AAAA_t;
        break;
    default:
        break;
    }
}
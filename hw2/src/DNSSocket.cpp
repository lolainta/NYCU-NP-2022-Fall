#include "DNSSocket.h"

#include <iostream>
#include <cassert>
#include <cstring>

DNSSocket::DNSSocket(int port):UDPSocket(port){}

Message DNSSocket::get(){
    uint8_t buf[1000];
    ssize_t len=read(buf,sizeof(buf));
    memset(last,0,sizeof(last));
    memcpy(last,buf,len);
    llen=len;
    Message ret;
//    dump(buf,len);
    parse(ret,buf);
    return ret;
}

size_t DNSSocket::put(const Message&msg){
    uint8_t buf[1000];
    ssize_t len=gen(buf,msg);
    dump(buf,len);
    return reply(buf,len);
}

void DNSSocket::parse(Message&dst,uint8_t*src){
    size_t cur=0;
    cur+=parseHeader(dst,src);
    dst.ques.resize(dst.hdr.qdcount);
    for(size_t i=0;i<dst.hdr.qdcount;++i)
        cur+=parseQuestion(dst.ques[i],src,cur);

    dst.ans.resize(dst.hdr.ancount);
    for(size_t i=0;i<dst.hdr.ancount;++i)
        cur+=parseAnswer(dst,src,cur);
    
    dst.auth.resize(dst.hdr.nscount);
    for(size_t i=0;i<dst.hdr.nscount;++i)
        cur+=parseAuthority(dst,src,cur);

    dst.add.resize(dst.hdr.arcount);
    for(size_t i=0;i<dst.hdr.arcount;++i)
        cur+=parseAdditional(dst,src,cur);
}

size_t DNSSocket::parseHeader(Message&dst,uint8_t*src){
    memcpy(&dst.hdr,src,12); 
    Header&hdr=dst.hdr;
    hdr.id=__builtin_bswap16(hdr.id);
    hdr.qdcount=__builtin_bswap16(hdr.qdcount);
    hdr.ancount=__builtin_bswap16(hdr.ancount);
    hdr.nscount=__builtin_bswap16(hdr.nscount);
    hdr.arcount=__builtin_bswap16(hdr.arcount);
    return 12;
}

size_t DNSSocket::parseName(vector<string>&name,uint8_t*src,size_t offset){
    size_t cur=offset;
     while(src[cur]){
        name.emplace_back("");
        for(int i=1;i<=src[cur];++i)
            name.back().push_back(src[cur+i]);
        cur+=src[cur]+1;
    }
    ++cur;
    return cur-offset;
}

size_t DNSSocket::parseQuestion(Question&ques,uint8_t*src,size_t offset){
    size_t cur=offset;
    cur+=parseName(ques.qname,src,cur);
    memcpy(&ques.qtype,src+cur,sizeof(TYPE));
    ques.qtype=(TYPE)__builtin_bswap16(ques.qtype);
    cur+=sizeof(TYPE);
    memcpy(&ques.qclass,src+cur,sizeof(CLASS));
    ques.qclass=(CLASS)__builtin_bswap16(ques.qclass);
    cur+=sizeof(CLASS);
    return cur-offset;
}

size_t DNSSocket::parseResourceRecord(Message&dst,uint8_t*src,size_t offset){
    return 0;
}

size_t DNSSocket::parseAnswer(Message&dst,uint8_t*src,size_t offset){
    return parseResourceRecord(dst,src,offset);
}

size_t DNSSocket::parseAuthority(Message&dst,uint8_t*src,size_t offset){
    return 0;    
}

size_t DNSSocket::parseAdditional(Message&dst,uint8_t*src,size_t offset){
    return 0;
}

void DNSSocket::dump(uint8_t*msg,ssize_t len){
    printf("DUMP\n");
    assert(len>=12);
    for(ssize_t i=0;i<12;++i){
        if(i and i%2==0)
            printf(" ");
        printf("%02hhX",msg[i]); 
    }
    printf("\n");
    msg+=12;
    for(ssize_t i=0;i<len-12;++i){
        if(i and i%8==0)
            printf("\n");
        printf("%02hhX ",msg[i]);
    }
    printf("\n");
}

void DNSSocket::info(const Message&msg)const{
    printf("\n[Header]\n");
    const Header&hdr=msg.hdr;
    printf("id=%x,count=%d,%d,%d,%d\n",hdr.id,hdr.qdcount,hdr.ancount,hdr.nscount,hdr.arcount);
    printf("qr=%b.opcode=%x,aa=%b,tc=%b,rd=%b,ra=%b,z=%x,rcode=%x\n",hdr.qr,hdr.opcode,hdr.aa,hdr.tc,hdr.rd,hdr.ra,hdr.z,hdr.rcode);

    printf("\n[Question]\n");
    for(const auto&ques:msg.ques){
        printf("qname=");
        for(auto qn:ques.qname)
            printf("%s.",qn.c_str());
        printf("\n");
        printf("type=%d,class=%d\n",ques.qtype,ques.qclass);
    }

    // printf("\n[Answers]\n");
    // for(const auto&ans:msg.ans){
    //     printf("name=");
    //     for(auto n:ans.rname)
    //         printf("%s.",n.c_str());
    //     printf("\n");
    //     printf("type=%d,class=%d,ttl=%d,rdlength=%d\n",ans.rtype,ans.rclass,ans.ttl,ans.rdlength);
    //     printf("rdata=`not imlmeneted yet!`\n");
    // }
}

size_t DNSSocket::gen(uint8_t*dst,const Message&msg){
    size_t cur=0;
    cur+=genHeader(dst,msg.hdr);
    for(size_t i=0;i<msg.hdr.qdcount;++i)
        cur+=genQuestion(dst+cur,msg.ques[i]);
    for(size_t i=0;i<msg.hdr.ancount;++i)
        cur+=genAnswer(dst+cur,msg.ans[i]);
    for(size_t i=0;i<msg.hdr.nscount;++i)
        cur+=genAuthority(dst+cur,msg.auth[i]);
    for(size_t i=0;i<msg.hdr.arcount;++i)
        cur+=genAdditional(dst+cur,msg.add[i]);
    return cur; 
}

size_t DNSSocket::genHeader(uint8_t*dst,const Header&hdr){
    Header thdr=hdr;
    thdr.id=__builtin_bswap16(hdr.id);
    thdr.qdcount=__builtin_bswap16(hdr.qdcount);
    thdr.ancount=__builtin_bswap16(hdr.ancount);
    thdr.nscount=__builtin_bswap16(hdr.nscount);
    thdr.arcount=__builtin_bswap16(hdr.arcount);

    size_t cur=0;
    memcpy(dst,&thdr,sizeof(thdr));
    cur+=sizeof(thdr);
    assert(cur==12);
    return cur;
}

size_t DNSSocket::genName(uint8_t*dst,const vector<string>&name){
    size_t cur=0;
    for(const auto&n:name){
        dst[cur++]=n.size();
        for(const auto&c:n)
            dst[cur++]=c;
    }
    dst[cur++]=0;
    return cur;
}

size_t DNSSocket::genQuestion(uint8_t*dst,const Question&ques){
    Question tques;
    tques.qtype=ques.qtype;
    tques.qclass=ques.qclass;
    tques.qtype=(TYPE)__builtin_bswap16(tques.qtype);
    tques.qclass=(CLASS)__builtin_bswap16(tques.qclass);
    size_t cur=0;
    cur+=genName(dst,ques.qname);
    memcpy(dst+cur,&tques.qtype,sizeof(TYPE));
    cur+=sizeof(TYPE);
    memcpy(dst+cur,&tques.qclass,sizeof(CLASS));
    cur+=sizeof(CLASS);
    return cur;
}

size_t DNSSocket::genResourceRecord(uint8_t*dst,const ResourceRecord&rr){
    ResourceRecord trr;
    trr.rtype=rr.rtype;
    trr.rclass=rr.rclass;
    trr.ttl=rr.ttl;
    trr.rdlength=rr.rdlength;
    trr.rtype=(TYPE)__builtin_bswap16(trr.rtype);
    trr.rclass=(CLASS)__builtin_bswap16(trr.rclass);
    trr.ttl=__builtin_bswap32(trr.ttl);
    trr.rdlength=__builtin_bswap16(trr.rdlength);
    
    size_t cur=0;
    cur+=genName(dst,rr.rname);
    memcpy(dst+cur,&trr.rtype,sizeof(TYPE));
    cur+=sizeof(TYPE);
    memcpy(dst+cur,&trr.rclass,sizeof(CLASS));
    cur+=sizeof(CLASS);
    memcpy(dst+cur,&trr.ttl,sizeof(trr.ttl));
    cur+=sizeof(rr.ttl);
    memcpy(dst+cur,&trr.rdlength,sizeof(trr.rdlength));
    cur+=sizeof(rr.rdlength);

    trr.rtype=(TYPE)__builtin_bswap16(trr.rtype);
    trr.rclass=(CLASS)__builtin_bswap16(trr.rclass);
    trr.ttl=__builtin_bswap32(trr.ttl);
    trr.rdlength=__builtin_bswap16(trr.rdlength);

    switch(rr.rtype){
    case TYPE::A:
        for(size_t i=0;i<rr.rdlength;++i)
            dst[cur++]=rr.a->ip[i];
        break;
    case TYPE::NS:
        cur+=genName(dst+cur,rr.ns->nsdname);
        break;
    case TYPE::CNAME:
        cur+=genName(dst+cur,rr.cname->cname);
        break;
    case TYPE::SOA:
        trr.soa=rr.soa;
        trr.soa->serial=__builtin_bswap32(rr.soa->serial);
        trr.soa->refresh=__builtin_bswap32(rr.soa->refresh);
        trr.soa->retry=__builtin_bswap32(rr.soa->retry);
        trr.soa->expire=__builtin_bswap32(rr.soa->expire);
        trr.soa->minumun=__builtin_bswap32(rr.soa->minumun);
        cur+=genName(dst+cur,rr.soa->mname);
        cur+=genName(dst+cur,rr.soa->rname);
        memcpy(dst+cur,&rr.soa->serial,sizeof(rr.soa->serial));
        cur+=sizeof(rr.soa->serial);
        memcpy(dst+cur,&rr.soa->refresh,sizeof(rr.soa->refresh));
        cur+=sizeof(rr.soa->refresh);
        memcpy(dst+cur,&rr.soa->retry,sizeof(rr.soa->retry));
        cur+=sizeof(rr.soa->retry);
        memcpy(dst+cur,&rr.soa->expire,sizeof(rr.soa->expire));
        cur+=sizeof(rr.soa->expire);
        memcpy(dst+cur,&rr.soa->minumun,sizeof(rr.soa->minumun));
        cur+=sizeof(rr.soa->minumun);
        trr.soa->serial=__builtin_bswap32(rr.soa->serial);
        trr.soa->refresh=__builtin_bswap32(rr.soa->refresh);
        trr.soa->retry=__builtin_bswap32(rr.soa->retry);
        trr.soa->expire=__builtin_bswap32(rr.soa->expire);
        trr.soa->minumun=__builtin_bswap32(rr.soa->minumun);
        break;
    case TYPE::MX:
        trr.mx=rr.mx;
        trr.mx->preference=__builtin_bswap16(rr.mx->preference);
        memcpy(dst+cur,&rr.mx->preference,sizeof(rr.mx->preference));
        cur+=sizeof(rr.mx->preference);
        cur+=genName(dst+cur,rr.mx->exchange);
        trr.mx->preference=__builtin_bswap16(rr.mx->preference);
        break;
    case TYPE::TXT:
        trr.txt=rr.txt;
        memcpy(dst+cur,&rr.txt->len,sizeof(rr.txt->len));
        cur+=sizeof(rr.txt->len);
        memcpy(dst+cur,rr.txt->txt_data.c_str(),rr.txt->txt_data.size());
        cur+=rr.txt->txt_data.size();
        break;
    case TYPE::AAAA:
        for(size_t i=0;i<rr.rdlength;++i)
            dst[cur++]=rr.a->ip[i];
        break;
    default:
        break;
    }
//    memcpy(dst+cur,trr.a,sizeof(*trr.a));

    return cur;
}

size_t DNSSocket::genAnswer(uint8_t*dst,const ResourceRecord&ans){
    return genResourceRecord(dst,ans);
}

size_t DNSSocket::genAuthority(uint8_t*dst,const ResourceRecord&ans){
    return genResourceRecord(dst,ans);
}

size_t DNSSocket::genAdditional(uint8_t*dst,const ResourceRecord&ans){
    return genResourceRecord(dst,ans);
}

pair<uint8_t*,ssize_t> DNSSocket::getLast(){
    return make_pair(last,llen);
}

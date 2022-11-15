#include <unistd.h>
#include "response.h"

Response::Response(const str&_cname="Orphan"):cname(_cname){
};

const str Response::header(const int&status)const{
    str ret;
    ret+=':';
    ret+=this->server+' ';
    ret+=to_string(status)+' ';
    ret+=this->cname+' ';
    ret+=':';
    return ret;
}

void Response::reply(const int&fd,const int&status,const str&msg)const{
    str ret=this->header(status);
    ret+=msg+'\n';
    if(write(fd,ret.c_str(),ret.size())<0)
        exit(1);
}

void Response::reply(const int&fd,const RPL&rpl)const{
    str ret=this->header(rpl);
    ret+="Hello, reply!"+'\n';
    if(write(fd,ret.c_str(),ret.size())<0)
        exit(1);
}

void Response::reply(const int&fd,const ERR&err)const{
    str ret=this->header(err);
    ret+="Hello, error!"+'\n';
    if(write(fd,ret.c_str(),ret.size())<0)
        exit(1);
}



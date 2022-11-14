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
    ret+=msg;
    if(write(fd,ret.c_str(),ret.size())<0)
        exit(1);
}

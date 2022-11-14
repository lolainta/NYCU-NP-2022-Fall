#include <unistd.h>
#include "response.h"

Response::Response(const str&_cname="Orphan"):cname(_cname){
    this->header+=':';
    this->header+=this->server+' ';
    this->header+=to_string(this->status)+' ';
    this->header+=cname+' ';
    this->header+=':';
};

void Response::reply(const int&fd,const str&msg)const{
    str ret(this->header);
    ret+=msg;
    if(write(fd,ret.c_str(),ret.size())<0)
        exit(1);
}

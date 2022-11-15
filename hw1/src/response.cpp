#include <unistd.h>
#include <vector>
#include "response.h"

const str Response::header(const int&status)const{
    str ret;
    ret+=':';
    ret+=this->server+' ';
    ret+=to_string(status)+' ';
    ret+=this->cname+' ';
    return ret;
}

void Response::reply(const str&msg)const{
    if(write(this->fd,msg.c_str(),msg.size())<0)
        exit(1);
}

void Response::reply(const vector<string>&rep)const{
    for(auto line:rep)
        this->reply(line);
}

void Response::set_cname(const str&_cname){
    this->cname=_cname;
}




#include "response.h"

Response::Response(const str&_cname="Orphan"):cname(_cname){
};

const str Response::header()const{
    str ret;
    ret+=':';
    ret+=this->server+' ';
    ret+=to_string(this->status)+' ';
    ret+=cname+' ';
    ret+=':';
    return ret;
}

#include "client.h"

Client::Client(int _id,const str&_name):id(_id),resp(_name){
    
};

void Client::reply(const str&){
    
}

const str Client::info()const{
    str ret;
    ret+=to_string(id)+'\t';
    ret+="name: "+name+'\t';
    ret+="nick: "+nick+'\t';
    ret+="host<"+ip+':'+port+">"+'\t';
    return ret;
}


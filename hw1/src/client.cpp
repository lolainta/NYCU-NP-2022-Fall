#include "client.h"

Client::Client(int _id):id(_id),name(),nick(){};

const str Client::info()const{
    str ret;
    ret+=to_string(id)+'\t';
    ret+="name: "+name+'\t';
    ret+="nick: "+nick+'\t';
    ret+="host<"+ip+':'+port+">"+'\t';
    return ret;
}


#include <arpa/inet.h>
#include "client.h"

Client::Client(int listenfd,const str&_name):resp(_name){
    struct sockaddr_in cliaddr;
    socklen_t clilen=sizeof(cliaddr);
    this->fd=accept(listenfd,(struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
    this->port=to_string(htons(cliaddr.sin_port));
    this->ip=(inet_ntoa(cliaddr.sin_addr));
};

void Client::reply(const int&status)const{
    
}
void Client::reply(const str&msg)const{
    this->resp.reply(this->fd,msg);
    return;
}

const str Client::info()const{
    str ret;
    ret+=to_string(id)+'\t';
    ret+="name: "+name+'\t';
    ret+="nick: "+nick+'\t';
    ret+="host<"+ip+':'+port+">"+'\t';
    return ret;
}


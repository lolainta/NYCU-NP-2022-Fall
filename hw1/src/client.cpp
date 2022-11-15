#include <arpa/inet.h>
#include <unistd.h>
#include "client.h"

Client::Client(int listenfd,const str&_name):resp(_name){
    struct sockaddr_in cliaddr;
    socklen_t clilen=sizeof(cliaddr);
    this->fd=accept(listenfd,(struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
    this->port=to_string(htons(cliaddr.sin_port));
    this->ip=(inet_ntoa(cliaddr.sin_addr));
    this->connected();
};

void Client::reply(const RPL&rpl)const{
    this->resp.reply(this->fd,rpl);
}
void Client::reply(const ERR&err)const{
    this->resp.reply(this->fd,err);
}

const str Client::info()const{
    str ret;
    ret+=to_string(this->id)+'\t';
    ret+="name: "+this->username+'\t';
    ret+="nick: "+this->nick+'\t';
    ret+="host<"+this->ip+':'+this->port+">"+'\t';
    return ret;
}

const str Client::connected()const{
    str ret("User connected!!\t");
    ret+=this->info();
    return ret;
}

const str Client::disconnected(){
    str ret("User disconnected!!\t");
    ret+=this->info();
    close(this->fd);
    this->fd=-1;
    return ret;
}

const bool Client::notreg()const{
    return this->nick.empty() and this->username.empty();
}

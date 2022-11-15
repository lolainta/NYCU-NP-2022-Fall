#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include "client.h"

Client::Client(int listenfd){
    struct sockaddr_in cliaddr;
    socklen_t clilen=sizeof(cliaddr);
    this->resp.fd=this->fd=accept(listenfd,(struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
    this->port=to_string(htons(cliaddr.sin_port));
    this->ip=(inet_ntoa(cliaddr.sin_addr));
    this->connected();
};

void Client::reply(const RPL&rpl)const{
    vector<str> ret(1,resp.header(rpl));
    switch(rpl){
      case RPL::RPL_MOTDSTART:
        ret.front()+=":- "+resp.server+" Message of the day - \n";
        break;
      case RPL::RPL_MOTD:
        ret.clear();
        ret.push_back(resp.header(rpl)+":- Welcome\n");
        ret.push_back(resp.header(rpl)+":- This is the motd.\n");
        ret.push_back(resp.header(rpl)+":- This server is developed by lolainta.\n");
        break;
      case RPL_ENDOFMOTD:
        ret.front()+=":End of /MOTD command\n";
        break;
    }
    this->resp.reply(ret);
}
void Client::reply(const ERR&err)const{
    vector<str> ret(1,resp.header(err));
    switch(err){
    }
    this->resp.reply(ret);
}

void Client::reply(const ERR&err,const str&param)const{
    vector<str> ret(1,resp.header(err));
    switch(err){
      case ERR_NEEDMOREPARAMS:
        ret.front()+=param+" :Not enough parameters\n";
        break;
    }
    this->resp.reply(ret);
}


void Client::set_nick(const str&_nick){
    this->nick=_nick;
}

void Client::set_name(const str&uname,const str&hname,const str&sname,const str&rname){
    this->username=uname;
    this->servername=sname;
    this->hostname=hname;
    this->realname=rname;
    this->resp.set_cname(uname);
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
    return this->nick.empty() or this->username.empty();
}

void Client::reg()const{
    if(!this->notreg()){
        this->reply(RPL::RPL_MOTDSTART);
        this->reply(RPL::RPL_MOTD);
        this->reply(RPL::RPL_ENDOFMOTD);
    }
}



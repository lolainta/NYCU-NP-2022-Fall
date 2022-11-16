#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include "client.h"
#include "channel.h"

Client::Client(int listenfd):ch(nullptr){
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
      case RPL::RPL_LISTSTART:
        ret.front()+="Channel :Users  Name\n";
        break;
      case RPL::RPL_LISTEND:
        ret.front()+=":End of /LIST\n";
        break;
      case RPL::RPL_NOTOPIC:
        ret.front()+=this->ch->name+" :No topic is set\n";
        break;
      case RPL::RPL_TOPIC:
        ret.front()+=this->ch->name+" :"+this->ch->get_topic()+'\n';
        break;
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
      case RPL_USERSSTART:
        ret.front()+=":UserID   Terminal  Host\n";
        break;
      case RPL_ENDOFUSERS:
        ret.front()+=":End of users\n";
        break;
      default:
        ret.front()+=":Not implemented yet!\n";
    }
    this->resp.reply(ret);
}

void Client::reply(const RPL&rpl,cstr&param)const{
    vector<str> ret(1,resp.header(rpl));
    switch(rpl){
      case RPL_LIST:
        ret.front()+=param;
        break;
      case RPL_NAMREPLY:
        ret.front()+=param;
        break;
      case RPL_ENDOFNAMES:
        ret.front()+=param+" :End of /NAMES list\n";
        break;
      case RPL_USERS:
        ret.front()+=param;
        break;
      default:
        ret.front()+=":Not implemented yet!\n";
    }
    this->resp.reply(ret);
}
void Client::reply(const ERR&err)const{
    vector<str> ret(1,resp.header(err));
    switch(err){
      case ERR_NOORIGIN:
        ret.front()+=":No origin specified\n";
        break;
      case ERR_NOTREGISTERED:
        ret.front()+=":You have not registered\n";
        break;
      default:
        ret.front()+=":Not implemented yet!\n";
    }
    this->resp.reply(ret);
}

void Client::reply(const ERR&err,cstr&param)const{
    vector<str> ret(1,resp.header(err));
    switch(err){
      case ERR_NOSUCHCHANNEL:
        ret.front()+=param+" :No such channel\n";
        break;
      case ERR_UNKNOWNCOMMAND:
        ret.front()+=param+" :Unknown command\n";
        break;
      case ERR_NOTONCHANNEL:
        ret.front()+=param+" :You're not on that channel\n";
        break;
      case ERR_NEEDMOREPARAMS:
        ret.front()+=param+" :Not enough parameters\n";
        break;
      default:
        ret.front()+=":Not implemented yet!\n";
    }
    this->resp.reply(ret);
}

void Client::reply(cstr&msg)const{
    this->resp.reply(vector<str>(1,msg));
}


void Client::set_nick(cstr&_nick){
    this->nick=_nick;
}

void Client::set_name(cstr&uname,cstr&hname,cstr&sname,cstr&rname){
    this->username=uname;
    this->servername=sname;
    this->hostname=hname;
    this->realname=rname;
    this->resp.set_cname(uname);
}

void Client::set_channel(Channel*ptr){
    this->ch=ptr;
}

cstr Client::get_nick()const{
    return this->nick;
}

cstr Client::fixl(cstr&s,const size_t&len)const{
    if(s.size()>len)
        return s;
    return str(len-s.size(),' ')+s;
}

cstr Client::info()const{
    str ret;
    ret+=':'+fixl(this->nick,8)+' '+fixl("-",9)+' '+fixl(this->ip,8);
    return ret+'\n';
}

cstr Client::information()const{
    str ret;
    ret+=to_string(this->id)+'\t';
    ret+="name: "+this->username+'\t';
    ret+="nick: "+this->nick+'\t';
    ret+="host<"+this->ip+':'+this->port+">"+'\t';
    return ret;
}

cstr Client::connected()const{
    str ret("User connected!!\t");
    ret+=this->information();
    return ret;
}

cstr Client::disconnected(){
    str ret("User disconnected!!\t");
    ret+=this->information();
    close(this->fd);
    this->fd=-1;
    return ret;
}

const bool Client::notreg()const{
    return this->nick.empty() or this->username.empty();
}

bool Client::reg()const{
    if(this->notreg()){
        return false;
    }else{
        this->reply(RPL::RPL_MOTDSTART);
        this->reply(RPL::RPL_MOTD);
        this->reply(RPL::RPL_ENDOFMOTD);
        return true;
    }
}



#include "channel.h"

Channel::Channel(cstr&_name):name(_name){};

void Channel::add_user(Client*cliptr){
    this->clis.push_back(cliptr);
}

void Channel::set_topic(cstr&_topic){
    this->topic=_topic;
}

cstr Channel::get_topic()const{
    return this->topic;
}

void Channel::erase(cstr&cname){
    for(auto it=clis.begin();it!=clis.end();++it)
        if((*it)->get_nick()==cname)
            return clis.erase(it),void();
}

cstr Channel::info()const{
    str ret;
    ret+=name+' ';
    ret+=to_string(clis.size())+' ';
    ret+=":"+topic;
    return ret+'\n';
}

cstr Channel::names()const{
    str ret;
    ret+=name+" :";
    for(auto cli:clis)
        ret+=cli->get_nick()+' ';
    return ret+'\n';
}

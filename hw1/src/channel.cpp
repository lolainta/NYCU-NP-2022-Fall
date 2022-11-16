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

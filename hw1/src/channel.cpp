#include "channel.h"

Channel::Channel(cstr&_name):name(_name),topic("No topic is set."){};

void Channel::add_user(Client*cliptr){
    this->clis.push_back(cliptr);
}

cstr Channel::get_topic()const{
    return this->topic;
}

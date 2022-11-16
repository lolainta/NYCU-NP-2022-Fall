#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>

#include "client.h"

using namespace std;
using str=string;
using cstr=const str;

class Channel{
private:
    vector<Client*> clis;
    str topic;
public:
    Channel(cstr&);
    str name;
    void add_user(Client*);
    void set_topic(cstr&);
    cstr get_topic()const;
    void erase(Client*);
};

#endif

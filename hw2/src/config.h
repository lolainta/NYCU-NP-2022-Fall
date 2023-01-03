#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <tuple>

#include "message.h"

using namespace std;

using str=string;
using cstr=const str;

class Config{
    str configFile;
    str forwardIP;
    map<str,str> domains;
    map<tuple<vector<string>,CLASS,TYPE>,ResourceRecord> records;
    void parseMeta();
    void parseZone(cstr&,cstr&);
    vector<str> split(cstr&,char);
    uint16_t getLength(const vector<str>&);
    void showRR(const ResourceRecord&)const;
public:
    Config();
    void load(cstr&);
    const char*getForward()const;
    void showConfig()const;
    bool check(const Question&)const;
    ResourceRecord getConfig(const Question&)const;
};

#endif

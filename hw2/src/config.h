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
    vector<str> split(cstr&,char)const;
    uint16_t getLength(const vector<str>&);
    void showRR(const ResourceRecord&)const;
    ResourceRecord getConfig(const tuple<vector<string>,CLASS,TYPE>&)const;
    bool check(const tuple<vector<string>,CLASS,TYPE> &)const;
public:
    Config();
    void load(cstr&);
    const char*getForward()const;
    void showConfig()const;
    bool inDomain(const vector<string>&name)const;
    vector<ResourceRecord> getAns(const Question&)const;
    vector<ResourceRecord> getAuth(const Question&)const;
    vector<ResourceRecord> getAdd(const vector<ResourceRecord>&)const;
};

#endif

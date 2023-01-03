#include "config.h"

#include <fstream>
#include <iostream>
#include <cassert>
#include <arpa/inet.h>

#define cerr cerr<<__FILE__<<' '<<__LINE__

Config::Config(){}

void Config::parseMeta(){
    ifstream ifs;
    ifs.open(configFile);
    str line;
    getline(ifs,line);
    forwardIP=line;
    while(getline(ifs,line)){
        auto tmp=split(line,',');
        assert(tmp.size()==2);
        domains[tmp[0]]=tmp[1];
    }
    ifs.close();
}

void Config::parseZone(cstr&zone,cstr&zfile){
    ifstream ifs;
    ifs.open(CONFIG_DIR+zfile);
    str line;
    getline(ifs,line);
    assert(line==zone);
    while(getline(ifs,line)){
        auto record=split(line,',');
        assert(record.size()==5);
        assert(record[2]=="IN");
        tuple<vector<string>,CLASS,TYPE> key;
        auto zname=split(zone,'.');
        if(record[0]!="@")
            zname.insert(zname.begin(),record[0]);
        get<0>(key)=zname;
        get<1>(key)=CLASS::IN;
        TYPE type;
        if(record[3]=="CNAME")
            type=TYPE::CNAME;
        else if(record[3]=="MX")
            type=TYPE::MX;
        else if(record[3]=="NS")
            type=TYPE::NS;
        else if(record[3]=="SOA")
            type=TYPE::SOA;
        else if(record[3]=="TXT")
            type=TYPE::TXT;
        else if(record[3]=="A")
            type=TYPE::A;
        else if(record[3]=="AAAA")
            type=TYPE::AAAA;
        else
            cerr<<"Invalid record type"<<endl,exit(1);
        get<2>(key)=type;

        ResourceRecord val(type,stoi(record[1]));
        val.rname=get<0>(key);
        val.rclass=CLASS::IN;
        auto rec=split(record[4],' ');
        switch(val.rtype){
        case TYPE::CNAME:
            assert(rec.size()==1);
            val.rdlength=getLength(split(rec[0],'.'));
            val.cname->cname=split(rec[0],'.');
            break;
        case TYPE::MX:
            assert(rec.size()==2);
            val.rdlength=getLength(split(rec[1],'.'))+2;
            val.mx->preference=stoi(rec[0]);
            val.mx->exchange=split(rec[1],'.');
            break;
        case TYPE::NS:
            assert(rec.size()==1);
            val.rdlength=getLength(split(rec[0],'.'));
            val.ns->nsdname=split(rec[0],'.');
            break;
        case TYPE::SOA:
            assert(rec.size()==7);
            val.rdlength=getLength(split(rec[0],'.'))+getLength(split(rec[1],'.'))+20;
            val.soa->mname=split(rec[0],'.');
            val.soa->rname=split(rec[1],'.');
            val.soa->serial=stoi(rec[2]);
            val.soa->refresh=stoi(rec[3]);
            val.soa->retry=stoi(rec[4]);
            val.soa->expire=stoi(rec[5]);
            val.soa->minumun=stoi(rec[6]);
            break;
        case TYPE::TXT:
            assert(rec.size()==1);
            val.rdlength=rec[0].size();
            val.txt->txt_data=rec[0];
            break;
        case TYPE::A:
            assert(rec.size()==1);
            val.rdlength=4;
            cout<<inet_addr(rec[0].c_str())<<endl;
            break;
        case TYPE::AAAA:
            assert(rec.size()==1);
            val.rdlength=16;
            break;
        default:
            cerr<<"Internal error"<<endl;
            exit(1);
        }
        records[key]=val;
    }
    ifs.close();
}

vector<str> Config::split(cstr&inp,char delim){
    vector<str> ret;
    size_t pos=0;
    size_t nxt;
    while((nxt=inp.find(delim,pos))!=string::npos){
        ret.emplace_back(inp.substr(pos,nxt-pos));
        pos=nxt+1;
    }
    ret.emplace_back(inp.substr(pos));
    if(ret.back().size()==0)
        ret.pop_back();
    return ret;
}

uint16_t Config::getLength(const vector<str>&name){
    uint16_t ret=name.size()+1;
    for(const auto&n:name)
        ret+=n.size();
    return ret;
}

void Config::showRR(const ResourceRecord&rr)const{
    cout<<"RR.rname=";
    for(auto x:rr.rname)
        cout<<x<<'.';
    cout<<endl;
    cout<<"rtype="<<rr.rtype<<",rclass="<<rr.rclass<<",rr.ttl="<<rr.ttl<<",rr.rdlength="<<rr.rdlength<<endl;
}
void Config::load(cstr&file){
    configFile=file;
    parseMeta();
    for(auto&[zone,zfile]:domains)
        parseZone(zone,zfile);
    
}

const char*Config::getForward()const{
    return forwardIP.c_str();
}

void Config::showConfig()const{
    cout<<"Forward IP = "<<forwardIP<<endl;
    cout<<"Find "<<domains.size()<<" domains"<<endl;
    for(auto&[x,y]:domains){
        cout<<x<<' '<<y<<endl;
    }
    for(auto[k,v]:records){
        for(auto n:get<0>(k))
            cout<<n<<'.';
        cout<<' '<<get<1>(k)<<' '<<get<2>(k)<<endl;
        
    }
}

bool Config::check(const Question&query)const{
    tuple<vector<string>,CLASS,TYPE> k(query.qname,query.qclass,query.qtype);
    return records.count(k);
}

ResourceRecord Config::getConfig(const Question&query)const{
    tuple<vector<string>,CLASS,TYPE> k(query.qname,query.qclass,query.qtype);
    assert(records.count(k)==1);
    showRR(records.at(k));
    return records.at(k);
}

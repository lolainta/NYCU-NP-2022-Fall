#include <iostream>
#include <string>
#include <cstring>
#include <cassert>

#include "DNSSocket.h"
#include "config.h"
using namespace std;

ResourceRecord solA(const Question&ques,const Config&conf){
    ResourceRecord ret;
    ret.rtype=A;
    ret.rclass=IN;
    ret.rname=ques.qname;
    ret.ttl=1;
    ret.rdlength=4;
    uint8_t ip[4]={0xff,0xff,0xff,0xff};
    switch(ret.rtype){
      case A:
        ret.a=new A_t;
        memcpy(ret.a,ip,4);
        break;
      default:
        cout<<"Not implemented type!"<<endl;
        exit(1);
    }
    return ret;
}

int main(int argc,char**argv){
    assert(argc==3);
    const int port(stoi(argv[1]));
    const string config(argv[2]);

    DNSSocket sock(port);
    Config conf;
    conf.load(config);
    // conf.showConfig();
    UDPSocket fwdSock(conf.getForward(),53);

    while(Message msg=sock.get()){
        cout<<"====== query =====";
        sock.info(msg);
        cout<<"====== query ====="<<endl;

        Message reply(msg.ques.size(),msg.ques.size(),0,0);
        reply.hdr.id=msg.hdr.id;
        reply.hdr.qr=1;
        for(size_t i=0;i<reply.ques.size();++i){
            reply.ques[i]=msg.ques[i];
            assert(msg.ques[i].qclass==CLASS::IN);
            if(conf.check(msg.ques[i])){
                cout<<"Query found in config file"<<endl;;
                reply.ans[i]=conf.getConfig(msg.ques[i]);
                continue;
            }
            cout<<"Query not found"<<endl;
            auto cur=sock.getLast();
            fwdSock.send(cur.first,cur.second);
            uint8_t ans[1000];
            ssize_t rlen=fwdSock.read(ans,sizeof(ans));
            sock.reply(ans,rlen);
            break;
        }
        sock.put(reply);
//        break;
    }
    return 0;
}

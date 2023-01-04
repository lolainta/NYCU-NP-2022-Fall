#include <iostream>
#include <string>
#include <cstring>
#include <cassert>

#include "DNSSocket.h"
#include "config.h"
using namespace std;

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
        assert(msg.ques.size()==1);
        assert(msg.ques.front().qclass==CLASS::IN);
        if(conf.served(msg.ques.front().qname)){
            cout<<"Served"<<endl;
            Message reply(0,0,0,0);
            reply.hdr.id=msg.hdr.id;
            reply.hdr.qr=1;
            reply.ques.push_back(msg.ques.front());
            reply.ans=conf.getAns(msg.ques.front());
            reply.auth=conf.getAuth(msg.ques.front());
            reply.add=conf.getAdd(reply.ans);
            reply.hdr.qdcount=reply.ques.size();
            reply.hdr.ancount=reply.ans.size();
            reply.hdr.nscount=reply.auth.size();
            reply.hdr.arcount=reply.add.size();
            sock.put(reply);
        }else{
            cout<<"Forward"<<endl;
            auto cur=sock.getLast();
            fwdSock.send(cur.first,cur.second);
            uint8_t ans[1000];
            ssize_t rlen=fwdSock.read(ans,sizeof(ans));
            sock.reply(ans,rlen);
        }
    }
    return 0;
}

#include<iostream>
#include<cassert>
#include<arpa/inet.h>
#include<unistd.h>
#include<vector>
#include<cstring>
#include<signal.h>
#include<thread>

using namespace std;
using str=string;
using cstr=const str;

const int MAXC=12;
bool cancel=false;
int servSock;
vector<int> socks;
vector<thread> threads;

void timeout(int sig){
    cancel=true;
    for(int i=0;i<MAXC;++i)
        threads[i].join();
    str msg("/report\n");
    for(auto sock:socks)
        close(sock);
    write(servSock,msg.c_str(),msg.size());
    char read_buf[2048];
    memset(read_buf,0,2048);
    read(servSock,read_buf,2048);
    cout<<read_buf;
    exit(0);
}

void flood(int sock){
    str msg("asdfghjkl");
    for(;;)
        if(cancel)
            return;
        else
            write(sock,msg.c_str(),msg.size());
}

int main(int argc,char**argv){
    int servPort=stoi(argv[2]),sinkPort=stoi(argv[2])+1;
    cout<<"Server port: "<<servPort<<endl;
    cout<<"Sink port: "<<sinkPort<<endl;

    struct sockaddr_in servAddr,sinkAddr;
    servAddr.sin_family=sinkAddr.sin_family=AF_INET;
    servAddr.sin_port=htons(servPort);
    sinkAddr.sin_port=htons(sinkPort);

    servSock=socket(AF_INET,SOCK_STREAM,0);
    if(inet_pton(AF_INET,argv[1],&servAddr.sin_addr)<=0)
        return perror(to_string(__LINE__).c_str()),1;
    if(connect(servSock,(sockaddr*)&servAddr,sizeof(servAddr))<0)
        return perror(to_string(__LINE__).c_str()),1;

    for(int i=0;i<MAXC;++i){
        socks.push_back(socket(AF_INET,SOCK_STREAM,0));
        if(inet_pton(AF_INET,argv[1],&sinkAddr.sin_addr)<0)
            return perror(to_string(__LINE__).c_str()),1;
        if(connect(socks[i],(struct sockaddr*)&sinkAddr,sizeof(sinkAddr))<0)
            return perror(to_string(__LINE__).c_str()),1;
    }

    signal(SIGTERM,timeout);

    str msg("/reset\n");
    write(servSock,msg.c_str(),msg.size());
    char read_buf[2048];
    memset(read_buf,0,sizeof(read_buf));
    read(servSock,read_buf,2048);
    cout<<read_buf;
    
    for(int i=0;i<MAXC;++i)
        threads.push_back(thread(flood,socks[i]));

    sleep(10000);

    return 0;
}

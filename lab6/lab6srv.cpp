#include<iostream>
#include<vector>
#include<cassert>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include<chrono>

using namespace std;
using str=string;
using cstr=const str;

typedef struct client{
    int id,fd;
    str ip,port;
    bool cmd=true;
    client(int _id,bool _cmd,int lfd):id(_id),cmd(_cmd){
        struct sockaddr_in cliaddr;
        socklen_t clilen=sizeof(cliaddr);
        fd=accept(lfd,(struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
        port=to_string(htons(cliaddr.sin_port));
        ip=(inet_ntoa(cliaddr.sin_addr));
    }
}Client;

cstr disconnect(Client&x){
    return "* client "+x.ip+":"+x.port+" disconnected\n";
}

cstr connect(Client&x){
    return "* client connected from "+x.ip+":"+x.port+" to "+(x.cmd?"control":"sink")+"\n";
}

uint64_t counter=0;

const uint64_t gettime(){
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()).count());
}

cstr stime(uint64_t time){
    auto sec=time/1000;
    auto msec=time%1000;
    return to_string(sec)+'.'+to_string(msec)+'s';
}

cstr reset(){
    str ret=stime(gettime())+" RESET "+to_string(counter)+"\n";
    return ret;
}
cstr ping(){
    return stime(gettime())+" PONG\n";
}

cstr report(uint64_t&last){
    auto cur=gettime();
    auto elaps=cur-last;
    str ret=stime(gettime())+" REPORT "+to_string(counter)+' ';
    ret+=stime(elaps)+' ';
    ret+=to_string((long double)counter/(long double)elaps/1000.0*8.0);
    ret+="Mbps\n";
    return ret;
}

cstr client(const vector<Client> clis){
    int num=0;
    for(auto c:clis)if(!c.cmd)
        num++;
    return stime(gettime())+" CLIENTS "+to_string(num)+"\n";
}

int main(int argc,char**argv){
    int server_port=stoi(argv[1]);
    int sink_port=server_port+1;
    struct sockaddr_in servaddr,sinkaddr;
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    int listenfd_sink=socket(AF_INET,SOCK_STREAM,0);

    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(server_port);
    bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    listen(listenfd,4096);

    sinkaddr.sin_family=AF_INET;
    sinkaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    sinkaddr.sin_port=htons(sink_port);
    bind(listenfd_sink,(struct sockaddr*)&sinkaddr,sizeof(sinkaddr));
    listen(listenfd_sink,4096);

    vector<Client> clients;
    
    uint64_t last=gettime();
    int cnt=0;
    const size_t buf_size=65536;
    char read_buf[buf_size];
//    signal(SIGPIPE,SIG_IGN);
    while(1){
        memset(read_buf,0,buf_size);
        int maxfd=max(listenfd,listenfd_sink);
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(listenfd,&rset);
        FD_SET(listenfd_sink,&rset);
        for(auto x:clients)
            if(x.fd!=-1)
                maxfd=max(maxfd,x.fd),FD_SET(x.fd,&rset);
            
        int nready=select(maxfd+1,&rset,nullptr,nullptr,nullptr);
        assert(nready);
        if(FD_ISSET(listenfd,&rset)){
            Client cur(cnt++,true,listenfd);
            clients.push_back(cur);
            assert(clients.size()<=FD_SETSIZE);
            cout<<connect(cur);
        }
        if(FD_ISSET(listenfd_sink,&rset)){
            Client cur(cnt++,false,listenfd_sink);
            clients.push_back(cur);
            assert(clients.size()<=FD_SETSIZE);
            cout<<connect(cur);
        }
        for(auto&cli:clients){
            if(cli.fd!=-1 and FD_ISSET(cli.fd,&rset)){
                int readn=read(cli.fd,read_buf,buf_size);
                if(readn<=0 or read_buf[0]=='\0'){
                    if(read_buf[0]=='\0')
                        readn=read(cli.fd,read_buf,1);
                    memset(read_buf,0,buf_size);
                    cout<<disconnect(cli);
                    assert(cli.fd!=listenfd);
                    close(cli.fd);
                    cli.fd=-1;
                }else{
                    if(cli.cmd and read_buf[0]=='/'){
                        if(!strncmp(read_buf,"/reset\n",7)){
                            str msg(reset());
                            write(cli.fd,msg.c_str(),msg.size());
                            counter=0;
                            last=gettime();
                        }else if(!strncmp(read_buf,"/ping\n",6)){
                            str msg(ping());
                            write(cli.fd,msg.c_str(),msg.size());
                        }else if(!strncmp(read_buf,"/report\n",8)){
                            str msg(report(last));
                            write(cli.fd,msg.c_str(),msg.size());
                        }else if(!strncmp(read_buf,"/clients\n",9)){
                            str msg(client(clients));
                            write(cli.fd,msg.c_str(),msg.size());
                        }else{
                            assert(0);
                        }
                    }else if(cli.cmd){
                        assert(read_buf[0]!='\0');
                        cout<<"Not a comand!\n";
                    }else{
                        counter+=readn;
                    }
                }
            }
        }

        for(int i=clients.size()-1;i>=0;--i)
            if(clients[i].fd==-1)
                clients.erase(clients.begin()+i);
    }
    return 0;
}

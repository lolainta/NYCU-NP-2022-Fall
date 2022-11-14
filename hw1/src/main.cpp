#include<iostream>
#include<vector>
#include<cassert>
#include<string>
#include<string.h>
#include<signal.h>
#include<memory>
#include<unistd.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
//#include<libexplain/read.h>

#define memset(...)

using namespace std;
using str=string;

#include "client.h"

 
/*
typedef struct client{
    int id;
    int fd;
    str nick;
    str name;
    str ip,port;
    client(int _id):nick(),name(){
        id=_id;
    }
    const str info(){
        str ret;
        ret+=to_string(id)+'\t';
        ret+="name: "+name+'\t';
        ret+="nick: "+nick+'\t';
        ret+="host<"+ip+':'+port+">"+'\t';
        return ret;
    }
}Client;
*/
typedef struct channel{
    int id;
    int cnt;
    str name;
    channel(int _id,str _name){
        id=_id;
        name=_name;
    }
}Channel;

const str connect(const Client&cli){
    str ret("User connected!!\t");
    ret+=cli.info();
    return ret+'\n';
}

const str disconnect(const Client&cli){
    str ret("User disconnected!!\t");
    ret+=cli.info();
    return ret+'\n';
}


int main(int argc,char**argv){
    int server_port=stoi(argv[1]);
    struct sockaddr_in servaddr,cliaddr;
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(server_port);
    bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    listen(listenfd,4096);

    vector<Client> clients;
    
    int cid=0;
    const int buf_size=65536;
    char read_buf[buf_size];
    char write_buf[buf_size];
//    signal(SIGPIPE,SIG_IGN);
    while(1){
        memset(read_buf,0,buf_size);
        memset(write_buf,0,buf_size);
        int maxfd=listenfd;
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(listenfd,&rset);
        for(auto x:clients)
            if(x.fd!=-1)
                maxfd=max(maxfd,x.fd),FD_SET(x.fd,&rset);
            
        int nready=select(maxfd+1,&rset,nullptr,nullptr,nullptr);
        assert(nready);
        if(FD_ISSET(listenfd,&rset)){
            Client cur(cid++);
            struct sockaddr_in cliaddr;
            socklen_t clilen=sizeof(cliaddr);
            cur.fd=accept(listenfd,(struct sockaddr*)&cliaddr,(socklen_t*)&clilen);
            cur.port=to_string(htons(cliaddr.sin_port));
            cur.ip=(inet_ntoa(cliaddr.sin_addr));
            clients.push_back(cur);
            assert(clients.size()<=FD_SETSIZE);

            cout<<connect(cur);
            continue;
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
                    if(read_buf[0]=='/'){
                        if(!strncmp(read_buf,"/who ",5) or !strncmp(read_buf,"/who\n",5)){
                        }else if(!strncmp(read_buf,"/name ",6)){
                        }else{
                        }
                    }else{
                    }
                }
            }
        }

        for(int i=clients.size()-1;i>=0;--i)
            if(clients[i].fd==-1){
                assert(i<clients.size());
                clients.erase(clients.begin()+i);
            }
    }
    return 0;
}

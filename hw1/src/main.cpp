#include<iostream>
#include<vector>
#include<cassert>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
//#include<string.h>
//#include<signal.h>
//#include<memory>
//#include<fcntl.h>
//#include<sys/socket.h>
//#include<netinet/in.h>
//#include<libexplain/read.h>

using namespace std;
using str=string;

#include"client.h"
#include"channel.h"

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
    struct sockaddr_in servaddr;
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(server_port);
    bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    listen(listenfd,4096);

    vector<Client> clients;
    
    const int buf_size=65536;
    char read_buf[buf_size];
//    char write_buf[buf_size];
//    signal(SIGPIPE,SIG_IGN);
    while(1){
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
            Client cur(listenfd,"");
            clients.push_back(cur);
            assert(clients.size()<=FD_SETSIZE);
            cout<<connect(cur);
            continue;
        }
        for(auto&cli:clients)if(cli.fd!=-1 and FD_ISSET(cli.fd,&rset)){
            int readn=read(cli.fd,read_buf,buf_size);
            if(readn<=0 or read_buf[0]=='\0'){
                if(read_buf[0]=='\0')
                    readn=read(cli.fd,read_buf,1);
                cout<<disconnect(cli);
                assert(cli.fd!=listenfd);
                close(cli.fd);
                cli.fd=-1;
            }else{
                cli.reply((const str)"Hi\n");
            }
        }

        for(int i=clients.size()-1;i>=0;--i)
            if(clients[i].fd==-1){
                assert(i<(int)clients.size());
                clients.erase(clients.begin()+i);
            }
    }
    return 0;
}

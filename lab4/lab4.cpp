#include<iostream>
#include<string>
#include<string.h>
#include<signal.h>
#include<memory>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>

using namespace std;

using str=string;

void sig_chld(int n){
    waitpid(-1,NULL,WNOHANG);
}

void child(int connfd,string cmd){
    dup2(connfd,1);
    dup2(connfd,0);
    system(cmd.c_str());
    return;
}

int main(int argc, char**argv){
        int listenfd,connfd;
        pid_t childpid;
        socklen_t clilen;
        struct sockaddr_in cliaddr,servaddr;
        int server_port=stoi(argv[1]);

        string cmd;
        for(int i=2;i<argc;++i)
            cmd+=(str)argv[i]+' ';

        listenfd=socket(AF_INET,SOCK_STREAM,0);
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
        servaddr.sin_port=htons(server_port);

        signal(SIGCHLD,sig_chld);
        bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
        listen(listenfd,3);

        int cnt=0;
        while(1){
            clilen=sizeof(cliaddr);
            connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
            childpid=fork();
            if(childpid==0){
                close(listenfd);
                cout<<"New connection from "<<inet_ntoa(cliaddr.sin_addr)<<":"<<cliaddr.sin_port<<endl;
                child(connfd,cmd);
                exit(0);
            }
            cout<<"Forked: "<<childpid<<endl;
            close(connfd);
        }
        return 0;
}

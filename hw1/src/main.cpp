#include<iostream>
#include<vector>
#include<cassert>
#include<cstring>
#include<sstream>
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

#define inv(x) {x=true;break;}

vector<str> split(const str&inp){
    stringstream ss(inp);
    str tmp;
    vector<str> ret;
    while(ss>>tmp)
        ret.push_back(tmp);
    if(ret.size()==1)
        return ret;
   return ret;
}

vector<str> merge(const vector<str>&org){
    vector<str> ret;
    size_t i;
    for(i=0;i<org.size();++i){
        if(org[i].front()==':'){
            assert(org[i].size()>1);
            ret.push_back(org[i].substr(1));
            break;
        }
        ret.push_back(org[i]); 
    }
    for(;i<org.size();++i)
        ret.push_back(org[i]);
    return ret;
}

int main(int argc,char**argv){
    int server_port=stoi(argv[1]);
    struct sockaddr_in servaddr;
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(server_port);
    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
        perror(to_string(__LINE__).c_str());
    if(listen(listenfd,4096)<0)
        perror(to_string(__LINE__).c_str());

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
            Client cur(listenfd);
            clients.push_back(cur);
            assert(clients.size()<=FD_SETSIZE);
            cout<<cur.connected()<<endl;
            continue;
        }
        for(auto&cli:clients)if(cli.fd!=-1 and FD_ISSET(cli.fd,&rset)){
            memset(read_buf,0,sizeof(read_buf));
            if(read(cli.fd,read_buf,buf_size)<0)
                exit(2);
            str input(read_buf);
            if(input[0]=='\0'){
                cout<<cli.disconnected()<<endl;;
            }else{
                vector<string> inp(split(input));
                for(auto s:inp)
                    cout<<'<'<<s<<'>';
                cout<<endl;
                if(inp.empty())
                    continue;
                bool invalid=false;;
                if(inp[0]=="NICK"){
                    if(inp.size()!=2)
                        cout<<"Too many arguments, only take the first one"<<endl;
                    cli.set_nick(inp[1]);
                    cli.reg();
                }else if(inp[0]=="USER"){
                    if(inp.size()<5){
                        cli.reply(ERR::ERR_NEEDMOREPARAMS,"USER");
                        break;
                    }
                    cli.set_name(inp[1],inp[2],inp[3],inp[4]);
                    cli.reg();
                }else if(inp[0]=="PING"){
                    if(inp.size()<2){
                        cli.reply(ERR::ERR_NOORIGIN);
                        break;
                    }
                    cli.reply("PONG "+inp[1]+'\n');
                }else if(inp[0]=="LIST"){
                    
                }else if(inp[0]=="JOIN"){
                    
                }else if(inp[0]=="TOPIC"){
                    
                }else if(inp[0]=="NAMES"){
                    
                }else if(inp[0]=="PART"){
                    
                }else if(inp[0]=="USERS"){
                    if(!cli.reg()){
                        cli.reply(ERR::ERR_NOTREGISTERED);
                        break;
                    }
                    cli.reply(RPL::RPL_USERSSTART);
                    for(auto cli:clients)
                        cli.reply(RPL::RPL_USERS,cli.info());
                    cli.reply(RPL::RPL_ENDOFUSERS);
                    
                }else if(inp[0]=="PRIVMSG"){
                    
                }else if(inp[0]=="QUIT"){
                    cout<<cli.disconnected()<<endl;
                }else{
                    invalid=true;
                }
                if(invalid){
//                    cli.reply("Invalid Command!");
                }
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

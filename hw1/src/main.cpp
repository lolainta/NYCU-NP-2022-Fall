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
using cstr=const string;

#include"client.h"
#include"channel.h"

const vector<Channel>::iterator nullit;

vector<str> split(cstr&inp){
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
            ret.push_back(org[i++].substr(1));
            break;
        }
        ret.push_back(org[i]); 
    }
    for(;i<org.size();++i)
        ret.back()+=' '+org[i];
    return ret;
}

/*
vector<Channel>::iterator addCh(vector<Channel>&chs,cstr&chname){
    for(auto it=chs.begin();it!=chs.end();++it)
        if(it->name==chname)
            return it;
    chs.emplace_back(chname);
    return prev(chs.end());
}
*/

Channel*getCh(vector<Channel>&chs,cstr&chname){
    for(auto&ch:chs)
        if(ch.name==chname)
            return &ch;
    return nullptr;
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
    vector<Channel> channels;
    
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
            cout<<cur.connected()<<endl<<flush;
            continue;
        }
        for(auto&cli:clients)if(cli.fd!=-1 and FD_ISSET(cli.fd,&rset)){
            memset(read_buf,0,sizeof(read_buf));
            if(read(cli.fd,read_buf,buf_size)<0)
                exit(2);
            str input(read_buf);
            if(input[0]=='\0'){
                cout<<cli.disconnected()<<endl<<flush;
            }else{
                vector<string> inp(split(input));
                for(auto s:inp)
                    cout<<'<'<<s<<'>';
                cout<<endl<<flush;
                if(inp.empty())
                    continue;
                if(inp[0]=="NICK"){
                    if(inp.size()<2){
                        cout<<"Not enough argument, but no return!\n";
                        continue;
                    }
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
                    if(cli.notreg()){
                        cli.reply(ERR::ERR_NOTREGISTERED);
                        break;
                    }
                    cli.reply(RPL::RPL_LISTSTART);
                    if(inp.size()==1){
                        for(auto ch:channels)
                            cli.reply(RPL::RPL_LIST,ch.info());
                    }else{
                        auto ch=getCh(channels,inp[1]);
                        if(!ch){
                            cli.reply(ERR::ERR_NOSUCHCHANNEL,inp[1]);
                            break;
                        }
                        cli.reply(RPL::RPL_LIST,ch->info());
                    }
                    cli.reply(RPL::RPL_LISTEND);
                }else if(inp[0]=="JOIN"){
                    if(cli.notreg()){
                        cli.reply(ERR::ERR_NOTREGISTERED);
                        break;
                    }
                    if(inp.size()<2){
                        cli.reply(ERR::ERR_NEEDMOREPARAMS,"JOIN");
                        break;
                    }
                    if(inp[1].front()!='#'){
                        cli.reply(ERR::ERR_NOSUCHCHANNEL,inp[1]);
                        break;
                    }
                    auto ch=getCh(channels,inp[1]);
                    if(!ch){
                        channels.emplace_back(inp[1]);
                        ch=&channels.back();
                    }
                    cli.set_channel(ch);
                    ch->erase(cli.get_nick());
                    ch->add_user(&cli);
                    if(ch->get_topic()!="")
                        cli.reply(RPL::RPL_TOPIC);
                    else
                        cli.reply(RPL::RPL_NOTOPIC);
                    cli.reply(RPL::RPL_NAMREPLY,ch->names());
                    cli.reply(RPL::RPL_ENDOFNAMES,ch->name);

                }else if(inp[0]=="TOPIC"){
                    if(cli.notreg()){
                        cli.reply(ERR::ERR_NOTREGISTERED);
                        break;
                    }
                    if(inp.size()<2){
                        cli.reply(ERR::ERR_NEEDMOREPARAMS,"TOPIC");
                        break;
                    }
                    if(cli.ch==nullptr or cli.ch->name!=inp[1]){
                        cli.reply(ERR::ERR_NOTONCHANNEL,inp[1]);
                        break;
                    }
                    if(inp.size()>2){
                        inp=merge(inp);
                        cli.ch->set_topic(inp[2]);
                    }
                    if(cli.ch->get_topic()!="")
                        cli.reply(RPL::RPL_TOPIC);
                    else
                        cli.reply(RPL::RPL_NOTOPIC);
                }else if(inp[0]=="NAMES"){
                    if(cli.notreg()){
                        cli.reply(ERR::ERR_NOTREGISTERED);
                        break;
                    }
                    if(inp.size()==1){
                        for(auto ch:channels){
                            cli.reply(RPL::RPL_NAMREPLY,ch.names());
                            cli.reply(RPL::RPL_ENDOFNAMES,ch.name);
                        }
                    }else{
                        auto ch=getCh(channels,inp[1]);
                        if(!ch){
                            cli.reply(ERR::ERR_NOSUCHCHANNEL,inp[1]);
                            break;
                        }
                        cli.reply(RPL::RPL_NAMREPLY,ch->names());
                        cli.reply(RPL::RPL_ENDOFNAMES,ch->name);
                    }
                }else if(inp[0]=="PART"){
                    if(cli.notreg()){
                        cli.reply(ERR::ERR_NOTREGISTERED);
                        break;
                    }
                    if(inp.size()<2){
                        cli.reply(ERR::ERR_NEEDMOREPARAMS,"PART");
                        break;
                    }
                    if(!getCh(channels,inp[1])){
                        cli.reply(ERR::ERR_NOSUCHCHANNEL,inp[1]);
                        break;
                    }
                    if(cli.ch==nullptr or cli.ch->name!=inp[1]){
                        cli.reply(ERR::ERR_NOTONCHANNEL,inp[1]);
                        break;
                    }
                    cli.ch->erase(cli.get_nick());
                    cli.ch=nullptr;
                }else if(inp[0]=="USERS"){
                    if(cli.notreg()){
                        cli.reply(ERR::ERR_NOTREGISTERED);
                        break;
                    }
                    cli.reply(RPL::RPL_USERSSTART);
                    for(auto clients:clients)
                        cli.reply(RPL::RPL_USERS,clients.info());
                    cli.reply(RPL::RPL_ENDOFUSERS);
                    
                }else if(inp[0]=="PRIVMSG"){
                    if(cli.notreg()){
                        cli.reply(ERR::ERR_NOTREGISTERED);
                        break;
                    }
                    if(inp.size()<3){
                        cli.reply(ERR::ERR_NEEDMOREPARAMS,"PRIVMSG");
                        break;
                    }
                    inp=merge(inp);
                    if(!getCh(channels,inp[1])){
                        cli.reply(ERR::ERR_NOSUCHCHANNEL,inp[1]);
                        break;
                    }

                 
                }else if(inp[0]=="QUIT"){
                    cout<<cli.disconnected()<<endl;
                }else{
                    cli.reply(ERR::ERR_UNKNOWNCOMMAND,inp[0]);
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

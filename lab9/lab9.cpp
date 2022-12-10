#include <iostream>
#include <cassert>
#include <vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <cstring>
#include <set>

using namespace std;
static const char*server_path="/sudoku.sock";

vector<vector<int>> ans;


void fill(vector<vector<int>>&grid){
    set<int> all;
    for(int i=1;i<10;++i){
        all.insert(i);
    }
    for(int i=0;i<9;++i)for(int j=0;j<9;++j)if(grid[i][j]==-1){
        set<int> tmp(all);
        for(int k=1;k<9;++k){
            if(tmp.count(grid[i][(j+k)%9]))
                tmp.erase(grid[i][(j+k)%9]);
        }
        for(int k=1;k<9;++k){
            if(tmp.count(grid[(i+k)%9][j]))
                tmp.erase(grid[(i+k)%9][j]);
        }
        for(int k=0;k<9;++k){
            int u=i/3*3+k/3;
            int v=j/3*3+k%3;
            if(tmp.count(grid[u][v]))
                tmp.erase(grid[u][v]);
        }
        if(tmp.size()==1){
            vector<int> cur;
            cur.push_back(i);
            cur.push_back(j);
            cur.push_back(*tmp.begin());
            ans.push_back(cur);
            grid[i][j]=*tmp.begin();
        }
    }
}

void prt(const vector<vector<int>>&grid){
    for(const auto&row:grid){
        for(const auto&x:row)
            cout<<x<<' ';
        cout<<endl;
    }
    cout<<endl;
}
int main(int argc,char**argv){

    int fd;
    socklen_t len;
    struct sockaddr_un srvAddr;

    fd=socket(AF_LOCAL,SOCK_STREAM,0);

    bzero(&srvAddr,sizeof(srvAddr));
    srvAddr.sun_family=AF_LOCAL;
    strcpy(srvAddr.sun_path,server_path);

    connect(fd,(sockaddr*)&srvAddr,sizeof(srvAddr));

    char buff[8192];
    strcpy(buff,"S");
    send(fd,buff,strlen(buff)+1,0);
    len=recv(fd,buff,8192,0);


    vector<vector<int>> grid(9,vector<int>(9));

    assert(strlen(buff)==86);

    int cur=4;
    for(int i=0;i<9;++i)for(int j=0;j<9;++j){
        if(buff[cur]=='.')
            grid[i][j]=-1;
        else
            grid[i][j]=buff[cur]-'0';
        cur++;
    }

    for(int round=0;round<40;++round){
//        prt(grid);
        fill(grid);
    }

    for(auto cur:ans){
        cout<<"send v"<<endl;
        string tmp("V ");
        tmp+=to_string(cur[0])+' ';
        tmp+=to_string(cur[1])+' ';
        tmp+=to_string(cur[2])+'\n';
        memset(buff,0,sizeof(buff));
        strcpy(buff,tmp.c_str());
        send(fd,buff,strlen(buff)+1,0);
        len=recv(fd,buff,8192,0);
    }
    len=recv(fd,buff,8192,0);
    cout<<buff<<endl;

    string tmp("C\n");
    strcpy(buff,tmp.c_str());
    send(fd,buff,strlen(buff)+1,0);
    len=recv(fd,buff,8192,0);
    cout<<buff<<endl;

    tmp="P\n";
    strcpy(buff,tmp.c_str());
    send(fd,buff,strlen(buff)+1,0);
    len=recv(fd,buff,8192,0);
    cout<<buff<<endl;


    return 0;

}

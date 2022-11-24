#include <iostream>

using namespace std;

int main() {
    int oseed;
    cout<<"Input seed: ";
    cin>>hex>>oseed;
    cin.ignore();
    srand(oseed);
    for(int i=0;i<4;++i)
        cout<<hex<<rand();
    cout<<endl;
    rand();
    for(int i=0;i<4;++i)
        cout<<hex<<rand();
    cout<<endl;
    return 0;
}

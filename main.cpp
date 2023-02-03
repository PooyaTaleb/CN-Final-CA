#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <string>
#include <limits>
#include <chrono>
#include <map>

using namespace std;

struct Packet{
    bool ack;
    uint16_t id;
    uint32_t src_address;
    uint32_t dest_address;
    char * data;
};

// queue for host A
queue <Packet> qa;
//queue for host B
queue <Packet> qb;
//queue for router R
queue <Packet> qr;

int packet_size=1000;


class Host{
    public:
        Host(string address): cwnd(1), ssthresh(numeric_limits<int>::max())
        
        {
            ip=address;
        }
        Host() 

        {

        }

        void sendFile(){
            ifstream file(filename, ios::in | ios::binary);



            vector <Packet> packets;
            
            char bytes[packet_size];
            file.read(bytes,packet_size);
            int id=1;

            while(file.gcount()==packet_size){
                Packet p;
                p.ack=false;
                p.id=id;
                p.src_address=100;
                p.dest_address=200;
                p.data=bytes;
                packets.push_back(p);

                file.read(bytes,packet_size);
                id++;
            }

            //filling last packet with null characters.
            for(int i=file.gcount();i<packet_size;i++){
                bytes[i]='\0';
            }

            Packet p;
            p.ack=false;
            p.id=id;
            p.src_address=100;
            p.dest_address=200;
            p.data=bytes;
            packets.push_back(p);

            file.close();       

            cout<<packets.size()<<endl;

            for (auto i=packets.begin(); i!=packets.end() ;i++){
                qr.push(*i);
            }





        }

        void receiveFile(){
            ofstream file("output.dt", ios::out | ios::binary);
            while(true){
                if (qb.empty()){
                    continue;
                }
                else{
                    Packet p=qb.front();
                    cout<<p.id<<endl;
                    qb.pop();
                    file.write(p.data,packet_size);
                }
            }
        }

    private:
        int cwnd;
        int ssthresh;
        string filename;
        string ip;

};

class Router{
    public:
        Router(string address)

        {
            ip=address;
        }
        Router()

        {

        }

        void routePackets(){
            while(true){
                // cout<<"hello"<<endl;
                // this_thread::sleep_for(chrono::seconds(2));
                if (qr.empty()){
                    continue;
                }
                else{
                    Packet p = qr.front();
                    cout<<p.id<<endl;
                    qr.pop();
                    this_thread::sleep_for(chrono::milliseconds(1));

                    qb.push(p);
                }
            }
        }
        string getip(){
            return ip;
        }

    private:
        string ip;
};


vector <string> splitString(string s){
    string temp="";
    vector <string> v;
    for (int i=0;i<s.length();i++){
        if (s[i]==' '){
            v.push_back(temp);
            temp="";
        }
        else if(i==s.length()-1){
            temp+=s[i];
            v.push_back(temp);
        }
        else{
            temp+=s[i];
        }

    }
    return v;
}

vector < pair <Host,string> > hosts;
vector < pair <Router,string> > routers;
vector < pair < pair<string,string>,int > > links;


void dvrp(){
    map<pair<string, string>, int> distances;
    for (auto &host: hosts){
        for (auto &link : links){
            if (link.first.first==host.second){
                distances[make_pair(host.second , link.first.second)]=link.second;
            }
        }
    }
    for (auto &router: routers){
        for (auto &link : links){
            if (link.first.first==router.second){
                distances[make_pair(router.second , link.first.second)]=link.second;
            }
        }
    }

    
    for(int i=0; i<routers.size()+hosts.size();i++){
        for(auto &host:hosts){
            for(auto &link:links){
                int dist=distances[make_pair(host.second,link.first.second)]+link.second;
                if(dist < distances[make_pair(link.first.second,host.second)]){
                    distances[make_pair(link.first.second,host.second)]=dist;
                }
            }
        }

        for(auto &router:routers){
            for(auto &link:links){
                int dist=distances[make_pair(router.second,link.first.second)]+link.second;
                if(dist < distances[make_pair(link.first.second,router.second)]){
                    distances[make_pair(link.first.second,router.second)]=dist;
                }
            }
        }
    }

    for (auto &host:hosts){
        for (auto &host2:hosts){
            if (host.second!=host2.second){
                cout<<host.second<<" "<<host2.second<<" "<<distances[make_pair(host.second,host2.second)]<<endl;
            }
        }
        for (auto &router:routers){
            cout<<host.second<<" "<<router.second<<" "<<distances[make_pair(host.second,router.second)]<<endl;
        }
    }

    for (auto &router: routers){
        for(auto &router2:routers){
            if(router.second!=router2.second){
                cout<<router.second<<" "<<router2.second<<" "<<distances[make_pair(router.second,router2.second)]<<endl;
            }
        }
    }


}


int main(){
    // Host a("dt_files/out-1MB.dt");
    // Router r1;
    // Host b;

    // thread A( &Host::sendFile , &a);
    // thread R1( &Router::routePackets , &r1);
    // thread B( &Host::receiveFile , &b);

    // A.join();
    // R1.join();
    // B.join();

    string input;
    getline(cin,input);
    vector <string> v=splitString(input);
    while (true){
        if (v[0]=="add"){
            if (v[1]=="hosts"){
                for (int i=2;i<v.size();i++){
                    Host a(v[i]);
                    pair<Host,string> p;
                    p.first=a;
                    p.second=v[i];
                    hosts.push_back(p);
                }
            }
        

            else if(v[1]=="routers"){
                for (int i=2;i<v.size();i++){
                    Router r(v[i]);
                    pair<Router,string> p;
                    p.first=r;
                    p.second=v[i];
                    routers.push_back(p);
                }
                // start each router its routing process.
                for (auto i=routers.begin();i!=routers.end();i++){
                    i->first.routePackets();
                }
            }

            else if(v[1]=="link"){
                links.push_back( make_pair(make_pair(v[2],v[3]) , stoi(v[4])));

            }
        }
        else if (v[0]=="update"){

        }
        else if (v[0]=="remove"){

        }
        else if (v[0]=="log"){

        }
        else if (v[0]=="run"){
            dvrp();
        }
        else if (v[0]=="draw"){

        }
        else if (v[0]=="show"){

        }
        else if (v[0]=="exit"){
            break;
        }
        else{
            cout<<"your command is wrong try again"<<endl;
        }
    }
    
    


    

    return 0;
}
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <string>
#include <limits>
#include <chrono>
#include <map>
#include <algorithm>

#include "classes.h"

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


vector < pair <Host,string> > hosts;
vector < pair <Router,string> > routers;
vector < pair < pair<string,string>,int > > links;
vector < pair <Node,string> > nodes;



Node::Node(){

}

void Node::dvrp(string src){
    for(auto &node:nodes){
        routingTable[node.second].first=numeric_limits<int>::max();
    }
    routingTable[src].first=0;
    routingTable[src].second=src;
    
    string temp="";


    vector <string> neighbors;

    for(int i=1; i<=nodes.size(); i++){
        for(int j=0; j<links.size(); j++){
            string u=links[j].first.first;
            string v=links[j].first.second;
            int weight=links[j].second;
            if(routingTable[u].first!=numeric_limits<int>::max() && routingTable[u].first+weight < routingTable[v].first){
                if (src==u){
                    neighbors.push_back(v);
                    temp=v;
                }
                if(find(neighbors.begin(),neighbors.end(),u)!=neighbors.end()){
                    temp=u;
                }
                routingTable[v].first=routingTable[u].first+weight;
                routingTable[v].second=temp;
            }

        }
    }


}

void Node::showTable(){
    cout <<endl;
    cout<<"destination      "<<"cost         "<<"next hop"<<endl;
    for(auto i:routingTable){
        cout<<i.first<<"                "<<i.second.first<<"                  "<<i.second.second<<endl;
    }
}

Host::Host(string address){
    ip==address;
    cwnd=1;
    ssthresh=numeric_limits<int>::max();

}
Host::Host(){}

void Host::sendFile(){
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

void Host::receiveFile(){
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


Router::Router(string address){
    ip==address;
}
Router::Router(){}

void Router::routePackets(){
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

// help functions

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
    vector <string> v;
    while (true){
        getline(cin,input);
        v=splitString(input);
        if (v[0]=="add"){
            if (v[1]=="hosts"){
                for (int i=2;i<v.size();i++){
                    Host a(v[i]);
                    hosts.push_back(make_pair(a,v[i]));
                    nodes.push_back(make_pair(a,v[i]));

                }
            }
        

            else if(v[1]=="routers"){
                for (int i=2;i<v.size();i++){
                    Router r(v[i]);
                    routers.push_back(make_pair(r,v[i]));
                    nodes.push_back(make_pair(r,v[i]));
                }
            }

            else if(v[1]=="link"){
                links.push_back( make_pair(make_pair(v[2],v[3]) , stoi(v[4])));
                links.push_back( make_pair(make_pair(v[3],v[2]) , stoi(v[4])));

            }
        }


        else if (v[0]=="update"){
            
        }


        else if (v[0]=="remove"){
            cout<<links.size()<<endl;
            for(auto i=links.begin();i<links.end();i++){
                if( i->first.first==v[2] && i->first.second==v[3] ){
                    links.erase(i);
                }
            }
            
            for(auto i=links.begin();i<links.end();i++){
                if( i->first.first==v[3] && i->first.second==v[2] ){
                    links.erase(i);
                }
            }

            cout<<links.size()<<endl;
        }


        else if (v[0]=="log"){

        }


        else if (v[0]=="run"){
            for (auto &node:nodes){
                node.first.dvrp(node.second);
            }
        }


        else if (v[0]=="draw"){

        }


        else if (v[0]=="show"){
            for (auto &node:nodes){
                if (node.second==v[2]){
                    node.first.showTable();
                }
            }

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

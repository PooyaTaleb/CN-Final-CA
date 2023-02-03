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



//define size of packet

int packet_size=1000;


// two global variables. used global variables for simplicity. links stores the two node addresses as a pair and the cost of the edge as its second element
// vector nodes stores each node object and its ip address.
vector < pair < pair<string,string>,int > > links;
vector < pair <Node,string> > nodes;


// our constructor to initilize our ip address for the node and to see whether its a host or a router
Node::Node(string address, string type){
    ip=address;
    if (type=="host"){
        ishost=true;
    }
    else{
        isrouter=true;
    }
}

Node::Node(){}

// this algorithm uses both links and nodes vectors to find the shortest paths and next hops for each node
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

string Node::getip(){
    return ip;
}

void Node::showTable(){
    cout <<endl<<endl;
    cout<<"routing table for node "<<ip<<endl;
    cout<<endl;
    cout<<"destination      "<<"cost         "<<"next hop"<<endl;
    for(auto i:routingTable){
        cout<<i.first<<"                "<<i.second.first<<"                  "<<i.second.second<<endl;
    }
}


// this functions sends file from this node to destination address given in function argument. used by hosts
void Node::sendFile(string dest){
    cout<<dest<<endl;
    string filename="dt_files/out-1MB.dt";
    ifstream file(filename, ios::in | ios::binary);

    // reading file and storing each chunk in packets and keeping it in vector packets
    vector <Packet> packets;
    char bytes[packet_size];
    file.read(bytes,packet_size);

    // packet id starts from 1
    int id=1;

    while(file.gcount()==packet_size){
        Packet p;
        p.ack=false;
        p.id=id;
        p.src_address=ip;
        p.dest_address=dest;
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
    p.src_address=ip;
    p.dest_address=dest;
    p.data=bytes;
    packets.push_back(p);

    file.close();       



    // find next hop by using routing table and destination address
    Node *hop;
    for(auto &node:nodes){
        if (node.second==routingTable[dest].second){
            hop=&node.first;
        }
    }

    // send packets to the queue of next hop
    for (auto i=packets.begin(); i!=packets.end() ;i++){
        hop->push(*i);
    }
}

// this function creates a file and writes the packets received in the file. used by hosts
void Node::receiveFile(){
    string filename="output.dt";
    ofstream file(filename, ios::out | ios::binary);
    while(true){
        if (q.empty()){
            continue;
        }
        else{
            Packet p=q.front();
            cout<<p.id<<endl;
            if (p.id==1049){
                return;
            }
            q.pop();
            file.write(p.data,packet_size);
        }
    }
}


// this function routes packets received in its queue to the appropriate next hop depending on its destination
void Node::routePackets(){
    while(true){
        if (q.empty()){
            continue;
        }
        else{
            Packet p = q.front();
            string dest=p.dest_address;
            string hop=routingTable[dest].second;
            for(auto &node:nodes){
                if(node.second==hop){
                    Node *h;
                    h=&node.first;
                    h->push(p);
                }
            }
            q.pop();
            // delay to send each packet is 1 milli second
            this_thread::sleep_for(chrono::milliseconds(1));

        }
    }
}

// pushes packet p to queue of node
void Node::push(Packet p){
    q.push(p);
}



// this funcitons splits the input by space character and stores each string in vector and returns it. used for parsing commands
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
    string input;
    vector <string> v;
    while (true){
        // input command
        getline(cin,input);
        v=splitString(input);

        //parse command
        if (v[0]=="add"){
            // if hosts our routers, creats a Node and stores it and its ip adress given by input in vector nodes
            if (v[1]=="hosts"){
                for (int i=2;i<v.size();i++){
                    Node a(v[i],"host");
                    nodes.push_back(make_pair(a,v[i]));

                }
            }
        

            else if(v[1]=="routers"){
                for (int i=2;i<v.size();i++){
                    Node r(v[i],"router");
                    nodes.push_back(make_pair(r,v[i]));
                }
            }
            // if links, pushes edge and its weight to vector links
            else if(v[1]=="link"){
                links.push_back( make_pair(make_pair(v[2],v[3]) , stoi(v[4])));
                links.push_back( make_pair(make_pair(v[3],v[2]) , stoi(v[4])));

            }
        }

        // updates link weight 
        else if (v[0]=="update"){
            if (v.size()==5){
                for(auto i=links.begin();i<links.end();i++){
                    if( i->first.first==v[2] && i->first.second==v[3] ){
                        i->second=stoi(v[4]);
                    }
                }
                for(auto i=links.begin();i<links.end();i++){
                    if( i->first.first==v[3] && i->first.second==v[2] ){
                        i->second=stoi(v[4]);
                    }
                }
                
            }
            else{

            }
        }

        // removes a link from the topology
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

        // runs the routing algorithm for each node
        else if (v[0]=="run"){
            for (auto &node:nodes){
                node.first.dvrp(node.second);
            }
        }


        else if (v[0]=="draw"){

        }

        // shows routing table for a node given by its ip address
        else if (v[0]=="show"){
            for (auto &node:nodes){
                if (node.second==v[2]){
                    node.first.showTable();
                }
            }

        }
        // this gets two arguments. <send> <ip1> <ip2> .  ip1 sends file to ip2.
        else if(v[0]=="send"){
            // thread vector to start running the routers in different threads
            std::vector<std::thread> threadvector;
            Node *sender;
            Node *receiver;
            // finds the object nodes already created for ip1 and ip2
            for(auto &node:nodes){
                if(node.second==v[1]){
                    sender=&node.first;
                }
                if(node.second==v[2]){
                    receiver=&node.first;
                }
            }
            // creates a thread for each router
            for(auto &node:nodes){
                if(node.first.isrouter){
                    threadvector.emplace_back([&](){node.first.routePackets();});
                }
            }
            // thread s starts sending file to destination reciever->getip(), and thread r starts receiving the file
            thread s(&Node::sendFile,sender,receiver->getip());
            thread r(&Node::receiveFile,receiver);


            // we join our threads in the end
            for(auto& t: threadvector){
                t.join();
            }
            s.join();
            r.join();
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

#include <string>
#include <limits>
#include <map>

using namespace std;

//our packet data structure. we define source and destination address as strings for simplicity but in reality it should be of type 32 bit integer. ipv4

struct Packet{
    bool ack;
    uint16_t id;
    string src_address;
    string dest_address;
    char * data;
};

//our Node data structure. we define hosts and routers as nodes.

class Node{
    public:
        //our countructor takes address which is the ip address of this node and a type which tells us if this node is a host or a router
        Node(string address, string type);

        Node();

        // dvrp algorithm (bellman ford). each node runs this algorithm to create its routing table
        void dvrp(string src);
        // displays our routing table
        void showTable();
        // returns ip address of this node
        string getip();

        // this function is when a node host wants to send a 1 MB file to a destination source
        void sendFile(string dest);

        // this function is a node host receiving a 1 MB file
        void receiveFile();

        // node router runs this functions permenantly (a seperate thread) and routs each packets it receives in its queue
        void routePackets();

        // pushes packet to the queue stored in this node
        void push(Packet p);

        // our routing table. first element of map is our destination address. our second element is a pair, which its first element is the cost
        // and its second element is the next hop
        map <string,pair<int,string>> routingTable;

        // to check if our node is host or router
        bool ishost=false;
        bool isrouter=false;

    private:
        // stores the ip address
        string ip;
        // stores the packets recieved. each host and router has a queue.
        queue <Packet> q;
};




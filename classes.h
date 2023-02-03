#include <string>
#include <limits>
#include <map>

using namespace std;

class Node{
    public:
        Node();

        void dvrp(string src);

        void showTable();

        map <string,pair<int,string>> routingTable;

    private:
};


class Host: public Node{
    public:
        Host(string address);
        Host();

        void sendFile();
        void receiveFile();

    private:
        int cwnd;
        int ssthresh;
        string filename;
        string ip;

};

class Router: public Node{
    public:
        Router(string address);
        Router();

        void routePackets();

    private:
        string ip;
};

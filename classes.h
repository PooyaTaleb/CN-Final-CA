#include <string>
#include <limits>

using namespace std;

class Node{
    public:
        Node();

        void dvrp(string src);

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

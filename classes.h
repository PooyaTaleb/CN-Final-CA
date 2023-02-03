#include <string>
#include <limits>
#include <map>

using namespace std;

class Node{
    public:
        Node(string address, string type);

        Node();


        void dvrp(string src);

        void showTable();

        string getip();

        void sendFile();

        void receiveFile();

        void routePackets();

        map <string,pair<int,string>> routingTable;

    private:
        string ip;
        bool ishost=false;
        bool isrouter=false;
};




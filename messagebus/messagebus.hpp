#include <arpa/inet.h>
#include <netinet/in.h>
// #include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>

#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstring>

struct Node {
    std::string ipAddress;
    int port;
};

class messagebus {
   public:
    messagebus(const std::vector<std::string>& addresses, const std::vector<int>& ports, const std::string& selfAddress, int selfPort);
    ~messagebus();

    void sendMessage(const std::string& message);
    void sendMessage(const std::string& message, const Node& node);

    void receiveMessage();
    void receiveMessage(const Node& node);

   private:
    std::vector<Node> nodes;
    Node selfNode;
};
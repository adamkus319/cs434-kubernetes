#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

struct Node {
    std::string ipAddress;
    std::string port;
};

class messagebus {
   public:
    messagebus(const std::vector<std::string>& addresses, const std::vector<int>& ports);
    ~messagebus();

    void sendMessage(const std::string& message);
    void sendMessage(const std::string& message, const Node& node);

    void receiveMessage();

   private:
    std::vector<Node> nodes;
};
#include "messagebus.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> addresses = {"127.0.0.1", "127.0.0.1"};
    std::vector<int> ports = {11000, 11001};

    std::string selfAddress = "127.0.0.1";
    int selfPort = 8080;

    messagebus mb(addresses, ports, selfAddress, selfPort);

    std::string msg = "Hello World!";

    // create a thread to receive message
    // std::thread t1(&messagebus::receiveMessage, &mb);
    // t1.detach();

    // create a thread to send message
    // std::thread t2(&messagebus::sendMessage, &mb, msg);
    // t2.detach();

    mb.sendMessage(msg);

    return 0;
}

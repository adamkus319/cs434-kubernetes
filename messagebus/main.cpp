#include "messagebus.hpp"

int main(int argc, char *argv[]) {
    // std::vector<std::string> addresses = {"127.0.0.1", "127.0.0.1"};
    std::vector<std::string> addresses;
    addresses.push_back("127.0.0.1");
    addresses.push_back("127.0.0.1");
    // std::vector<int> ports = {11000, 11001};
    std::vector<int> ports;
    ports.push_back(11000);
    ports.push_back(11001);

    messagebus mb(addresses, ports);

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

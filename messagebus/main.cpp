#include "messagebus.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> addresses = {"127.0.0.1", "127.0.0.1"};
    std::vector<int> ports = {11000, 11001};

    messagebus mb(addresses, ports);

    std::thread t1(&messagebus::receiveMessage, &mb);
    std::thread t2(&messagebus::sendMessage, &mb, "Hello World!");

    return 0;
}

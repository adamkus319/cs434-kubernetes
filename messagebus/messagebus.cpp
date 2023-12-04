#include "messagebus.hpp"

messagebus::messagebus(const std::vector<std::string>& addresses, const std::vector<int>& ports, const std::string& selfAddress, int selfPort) {
    for (int i = 0; i < addresses.size(); i++) {
        // skip self node
        if (addresses[i] == selfAddress && ports[i] == selfPort) {
            continue;
        }

        // create a node and add it to the vector
        Node node;
        node.ipAddress = addresses[i];
        node.port = ports[i];
        nodes.push_back(node);
    }

    selfNode.ipAddress = selfAddress;
    selfNode.port = selfPort;

    // call the receiveMessage function
    receiveMessage();
}

messagebus::~messagebus() {}

void messagebus::sendMessage(const std::string& message) {
    for (int i = 0; i < nodes.size(); i++) {
        sendMessage(message, nodes[i]);
    }
}

void messagebus::sendMessage(const std::string& message, const Node& node) {
    // create socket and send message
    int sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(node.port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, node.ipAddress.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address/ Address not supported" << std::endl;
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
        return;
    }

    // // get the local port number for debugging
    // struct sockaddr_in localAddr;
    // socklen_t addrSize = sizeof(localAddr);
    // if (getsockname(sock, (struct sockaddr*)&localAddr, &addrSize) == -1) {
    //     std::cout << "Getsockname failed" << std::endl;
    //     close(sock);
    //     return;
    // }
    // int localPort = ntohs(localAddr.sin_port);
    // std::cout << "Sending from port: " << localPort << std::endl;

    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Message sent to " << node.ipAddress << ":" << node.port << std::endl;

    // get "ACK" back from the receiver
    char buffer[1024] = {0};
    int valread = read(sock, buffer, 1024);
    std::cout << "Response received: " << buffer << std::endl;

    close(sock);
}

void messagebus::receiveMessage() {
    // start a thread to receive message
    std::thread t_rec([this]() {
        this->receiveMessage(selfNode);  // Call the appropriate receiveMessage function
    });
    t_rec.detach();
}

void messagebus::receiveMessage(const Node& node) {
    // create socket and wait for connection
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cout << "socket failed" << std::endl;
        return;
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cout << "setsockopt" << std::endl;
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(node.port);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cout << "bind failed" << std::endl;
        return;
    }

    if (listen(server_fd, 3) < 0) {
        std::cout << "listen" << std::endl;
        return;
    }

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        std::cout << "accept" << std::endl;
        return;
    }

    char buffer[1024] = {0};
    int valread = read(new_socket, buffer, 1024);
    std::cout << "Message received: " << buffer << std::endl;

    // send "ACK" back to the sender
    send(new_socket, "ACK", 3, 0);
    close(new_socket);
}
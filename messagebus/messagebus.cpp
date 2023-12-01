#include "messagebus.hpp"

messagebus::messagebus(const std::vector<std::string>& addresses, const std::vector<int>& ports) {
    for (int i = 0; i < addresses.size(); i++) {
        Node node;
        node.ipAddress = addresses[i];
        node.port = ports[i];
        nodes.push_back(node);
    }

    std::thread t1(&messagebus::receiveMessage, this);
    t1.detach();
}

messagebus::~messagebus() {}

void messagebus::sendMessage(const std::string& message) {
    for (int i = 0; i < nodes.size(); i++) {
        sendMessage(message, nodes[i]);
    }
}

void messagebus::sendMessage(const std::string& message, const Node& node) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(std::stoi(node.port));

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, node.ipAddress.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address/ Address not supported" << std::endl;
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
        return;
    }

    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Message sent to " << node.ipAddress << ":" << node.port << std::endl;
    close(sock);
}

void messagebus::receiveMessage() {
    while (true) {
        // create socket and wait for connections
        int server_fd, new_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cout << "socket failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            std::cout << "setsockopt" << std::endl;
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(std::stoi(nodes[0].port));

        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cout << "bind failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) < 0) {
            std::cout << "listen" << std::endl;
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            std::cout << "accept" << std::endl;
            exit(EXIT_FAILURE);
        }

        char buffer[1024] = {0};
        int valread = read(new_socket, buffer, 1024);
        std::cout << "Message received: " << buffer << std::endl;
        close(new_socket);
        close(server_fd);
    }
}
#include <grpc/grpc.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <iostream>
#include <sstream>

#include "codegen/message.grpc.pb.h"
#include "codegen/message.pb.h"
#include "scheduler.hpp"

using namespace std;
using namespace messagebus;
using namespace grpc;

void sendGlobalConfig(GlobalConfigLocal* config);

GlobalConfigLocal* currGlobalConfig = createGlobal(vector<RegionConfigLocal*>());
vector<string> addresses = vector<string>();

GlobalConfig* convertGlobalConfigLocal(const GlobalConfigLocal* config) {
    GlobalConfig* g = new GlobalConfig;
    for (const auto& region_ptr : config->regions) {
        RegionConfig* r = g->add_regions();
        r->set_regionname(region_ptr->regionName);
        for (const auto& cluster_ptr : region_ptr->clusters) {
            ClusterConfig* c = r->add_clusters();
            c->set_clustername(cluster_ptr->clusterName);
            for (const auto& service_ptr : cluster_ptr->services) {
                ServiceConfig* s = c->add_services();
                s->set_servicename(service_ptr->serviceName);
                s->set_count(service_ptr->count);
            }
        }
    }
    return g;
}

void sendGlobalConfig(GlobalConfigLocal* config) {
    GlobalConfig* grpc_config = convertGlobalConfigLocal(config);

    // send config to all peers
    for (int i = 0; i < addresses.size(); i++) {
        string peerAddress = addresses[i];

        cout << "Sending to " << peerAddress << endl;
        auto channel = grpc::CreateChannel(peerAddress, grpc::InsecureChannelCredentials());
        auto stub = MessageBus::NewStub(channel);

        // send config
        updateConfigResponse response;
        ClientContext context;

        Status status = stub->PeerUpdateConfig(&context, *grpc_config, &response);

        if (!status.ok()) {
            cout << "Error: " << status.error_code() << ": " << status.error_message() << endl;
        } else {
            cout << "Success" << endl;
        }
    }
}

int main(int argc, char** argv) {
    cout << "Starting messagebus" << endl;

    char* addr = std::getenv("SERVER_ADDRESS");
    string selfAddress;

    if (addr == nullptr) {
        cerr << "SERVER_ADDRESS not set" << endl;
        return 1;
    } else {
        selfAddress = addr;
    }

    char* address_str = std::getenv("PEER_ADDRESSES");
    addresses = vector<string>();
    if (address_str == NULL) {
        cerr << "PEER_ADDRESS not set" << endl;
        return 1;
    } else {
        // parse address (comma separated)
        std::istringstream ss(address_str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            addresses.push_back(token);
        }
    }

    // send initial config to peers
    sendGlobalConfig(currGlobalConfig);

    return 0;
}
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

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using messagebus::MessageBus;
using messagebus::updateConfigRequest;
using messagebus::updateConfigResponse;

class MessageBusClient {
   public:
    MessageBusClient(std::shared_ptr<Channel> channel)
        : stub_(MessageBus::NewStub(channel)) {}

    bool UserUpdateConfig(const std::string& regionName, const std::string& serviceName, int count) {
        updateConfigRequest request;
        request.set_regionname(regionName);
        request.set_servicename(serviceName);
        request.set_count(count);

        updateConfigResponse response;
        ClientContext context;

        Status status = stub_->UserUpdateConfig(&context, request, &response);

        if (status.ok()) {
            return response.ok();
        } else {
            std::cout << "RPC failed: " << status.error_message() << std::endl;
            return false;
        }
    }

   private:
    std::unique_ptr<MessageBus::Stub> stub_;
};

int main(int argc, char** argv) {
    MessageBusClient client(grpc::CreateChannel("192.168.64.6:30001", grpc::InsecureChannelCredentials()));
    std::string regionName = "us";
    std::string serviceName = "nginx";
    int count = 5;

    bool response = client.UserUpdateConfig(regionName, serviceName, count);
    std::cout << "Update config was " << (response ? "successful" : "unsuccessful") << std::endl;

    return 0;
}

// void sendGlobalConfig(GlobalConfigLocal* config);

// GlobalConfigLocal* currGlobalConfig = createGlobal(vector<RegionConfigLocal*>());
// vector<string> addresses = vector<string>();

// GlobalConfig* convertGlobalConfigLocal(const GlobalConfigLocal* config) {
//     GlobalConfig* g = new GlobalConfig;
//     for (const auto& region_ptr : config->regions) {
//         RegionConfig* r = g->add_regions();
//         r->set_regionname(region_ptr->regionName);
//         for (const auto& cluster_ptr : region_ptr->clusters) {
//             ClusterConfig* c = r->add_clusters();
//             c->set_clustername(cluster_ptr->clusterName);
//             for (const auto& service_ptr : cluster_ptr->services) {
//                 ServiceConfig* s = c->add_services();
//                 s->set_servicename(service_ptr->serviceName);
//                 s->set_count(service_ptr->count);
//             }
//         }
//     }
//     return g;
// }

// void sendGlobalConfig(GlobalConfigLocal* config) {
//     GlobalConfig* grpc_config = convertGlobalConfigLocal(config);

//     // send config to all peers
//     for (int i = 0; i < addresses.size(); i++) {
//         string peerAddress = addresses[i];

//         cout << "Sending to " << peerAddress << endl;
//         auto channel = grpc::CreateChannel(peerAddress, grpc::InsecureChannelCredentials());
//         auto stub = MessageBus::NewStub(channel);

//         // send config
//         updateConfigResponse response;
//         ClientContext context;

//         Status status = stub->PeerUpdateConfig(&context, *grpc_config, &response);

//         if (!status.ok()) {
//             cout << "Error: " << status.error_code() << ": " << status.error_message() << endl;
//         } else {
//             cout << "Success" << endl;
//         }
//     }
// }

// int main(int argc, char** argv) {
//     cout << "Starting messagebus" << endl;

//     char* addr = std::getenv("SERVER_ADDRESS");
//     string selfAddress;

//     if (addr == nullptr) {
//         cerr << "SERVER_ADDRESS not set" << endl;
//         return 1;
//     } else {
//         selfAddress = addr;
//     }

//     char* address_str = std::getenv("PEER_ADDRESSES");
//     addresses = vector<string>();
//     if (address_str == NULL) {
//         cerr << "PEER_ADDRESS not set" << endl;
//         return 1;
//     } else {
//         // parse address (comma separated)
//         std::istringstream ss(address_str);
//         std::string token;
//         while (std::getline(ss, token, ',')) {
//             addresses.push_back(token);
//         }
//     }

//     // send initial config to peers
//     sendGlobalConfig(currGlobalConfig);

//     return 0;
// }

// #include "scheduler.hpp"

// void test_scheduler(){
// 	ServiceConfigLocal *s1 = createService("nginx", 2);
//     ServiceConfigLocal *s2 = createService("apache", 3);
//     ServiceConfigLocal *s3 = createService("webapp", 1);
//     ServiceConfigLocal *s4 = createService("loadbalancer", 1);

//     vector<ServiceConfigLocal *> ss1 = {s1, s2};
//     vector<ServiceConfigLocal *> ss2 = {s3, s4};
//     ClusterConfigLocal *c1 = createCluster("us-east", ss1);
//     ClusterConfigLocal *c2 = createCluster("us-west", ss2);

//     vector<ClusterConfigLocal *> cs = {c1, c2};
//     RegionConfigLocal *r1 = createRegion("us", cs);

//     vector<RegionConfigLocal *> rs = {r1};
//     GlobalConfigLocal *currGlobalConfigLocal = createGlobal(rs);

//     cout << "Before updates:" << endl;
//     printGlobalConfigLocal(currGlobalConfigLocal);
//     cout << endl;

//     pair<string, unordered_map<string, int>> update;
//     update.first = "us";

//     unordered_map<string, int> service_map;
//     service_map["nnginx"] = 4;
//     update.second = service_map;

//     scheduler(currGlobalConfigLocal, update);

//     cout << "After updates:" << endl;
//     printGlobalConfigLocal(currGlobalConfigLocal);
// }

// void test_updater(){
//     ServiceConfigLocal *s1 = createService("server", 2);
//     ServiceConfigLocal *s2 = createService("client", 1);

//     vector<ServiceConfigLocal *> ss1 = {s1};
//     ClusterConfigLocal *c1 = createCluster("us-east", ss1);

//     vector<ClusterConfigLocal *> cs = {c1};
//     RegionConfigLocal *r1 = createRegion("us", cs);

//     vector<RegionConfigLocal *> rs = {r1};
//     GlobalConfigLocal *currGlobalConfigLocal = createGlobal(rs);

// 	updateLocal("us", "us-east", currGlobalConfigLocal);
// }

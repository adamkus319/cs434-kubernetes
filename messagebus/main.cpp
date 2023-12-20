#include <grpc/grpc.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "codegen/message.grpc.pb.h"
#include "codegen/message.pb.h"
#include "scheduler.hpp"

using namespace std;
using namespace messagebus;
using namespace grpc;

// config globals
string selfAddress;
vector<string> addresses;
string regionName;
string clusterName;

void sendGlobalConfig(GlobalConfigLocal* config);

// INIT STATE
// GlobalConfigLocal* currGlobalConfig = createGlobal(vector<RegionConfigLocal*>());
ServiceConfigLocal* s1 = createService("nginx", 1);
ServiceConfigLocal* s2 = createService("apache", 3);
ServiceConfigLocal* s3 = createService("webapp", 1);
ServiceConfigLocal* s4 = createService("loadbalancer", 1);

vector<ServiceConfigLocal*> ss1 = {s1, s2};
vector<ServiceConfigLocal*> ss2 = {s3, s4};
ClusterConfigLocal* c1 = createCluster("us-east", ss1);
ClusterConfigLocal* c2 = createCluster("us-west", ss2);

vector<ClusterConfigLocal*> cs1 = {c1, c2};
RegionConfigLocal* r1 = createRegion("us", cs1);

// EUR region
ServiceConfigLocal* s5 = createService("jetty", 4);
ServiceConfigLocal* s6 = createService("nginx", 3);

vector<ServiceConfigLocal*> ss3 = {s5, s6};
vector<ServiceConfigLocal*> ss4 = {s4};
ClusterConfigLocal* c3 = createCluster("france", ss3);
ClusterConfigLocal* c4 = createCluster("uk", ss4);

vector<ClusterConfigLocal*> cs2 = {c3, c4};
RegionConfigLocal* r2 = createRegion("eur", cs2);

// global configLocal
vector<RegionConfigLocal*> rs = {r1, r2};
GlobalConfigLocal* currGlobalConfig = createGlobal(rs);

int readEnvironmentVariables() {
    string serverAddress = "SERVER_ADDRESS";
    string peerAddresses = "PEER_ADDRESSES";
    string regionNameConf = "REGION_NAME";
    string clusterNameConf = "CLUSTER_NAME";

    char* serverAddressEnv = std::getenv(serverAddress.c_str());
    char* peerAddressesEnv = std::getenv(peerAddresses.c_str());
    char* regionNameEnv = std::getenv(regionNameConf.c_str());
    char* clusterNameEnv = std::getenv(clusterNameConf.c_str());

    cout << "serverAddress: " << serverAddressEnv << endl;
    cout << "peerAddresses: " << peerAddressesEnv << endl;
    cout << "regionName: " << regionNameEnv << endl;
    cout << "clusterName: " << clusterNameEnv << endl;

    if (serverAddressEnv == NULL || peerAddressesEnv == NULL || regionNameEnv == NULL || clusterNameEnv == NULL) {
        cerr << "Error reading environment variables" << endl;
        return 1;
    }

    selfAddress = serverAddressEnv;
    // parse peer addresses (comma-separated)
    istringstream as(peerAddressesEnv);
    string address;
    while (getline(as, address, ',')) {
        addresses.push_back(address);
    }
    regionName = regionNameEnv;
    clusterName = clusterNameEnv;

    return 0;
}

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

GlobalConfigLocal* convertGlobalConfig(const GlobalConfig* config) {
    GlobalConfigLocal* g = new GlobalConfigLocal;
    for (const auto& region_ptr : config->regions()) {
        RegionConfigLocal* r = createRegion(region_ptr.regionname(), vector<ClusterConfigLocal*>());
        for (const auto& cluster_ptr : region_ptr.clusters()) {
            ClusterConfigLocal* c = createCluster(cluster_ptr.clustername(), vector<ServiceConfigLocal*>());
            for (const auto& service_ptr : cluster_ptr.services()) {
                ServiceConfigLocal* s = createService(service_ptr.servicename(), service_ptr.count());
                c->services.push_back(s);
            }
            r->clusters.push_back(c);
        }
        g->regions.push_back(r);
    }
    return g;
}

class MessageBusServiceImpl : public MessageBus::Service {
    Status UserUpdateConfig(ServerContext* context, const updateConfigRequest* request, updateConfigResponse* response) override {
        // print request data
        cout << "Region: " << request->regionname() << endl;
        cout << "Service: " << request->servicename() << endl;
        cout << "Count: " << request->count() << endl;

        if (request->regionname().size() == 0) {
            response->set_ok(false);
            return Status(StatusCode::INVALID_ARGUMENT, "error: Argument not found");
        }
        if (request->servicename().size() == 0) {
            response->set_ok(false);
            return Status(StatusCode::INVALID_ARGUMENT, "error: Argument not found");
        }
        if (request->count() == 0) {
            response->set_ok(true);
            return Status::OK;
        }

        // print request data
        cout << "Region: " << request->regionname() << endl;
        cout << "Service: " << request->servicename() << endl;
        cout << "Count: " << request->count() << endl;

        // schedule updates
        scheduler(currGlobalConfig, make_pair(request->regionname(), unordered_map<string, int>{{request->servicename(), request->count()}}));

        // update local deployments
        updateLocal(regionName, clusterName, currGlobalConfig);

        sendGlobalConfig(currGlobalConfig);

        // send ack to stream
        response->set_ok(true);

        return Status::OK;
    }

    Status PeerUpdateConfig(ServerContext* context, const GlobalConfig* request, updateConfigResponse* response) override {
        // print request data
        cout << "Peer update config" << endl;
        GlobalConfigLocal* config = convertGlobalConfig(request);
        printGlobalConfigLocal(config);
        cout << endl;

        // send ack to stream
        response->set_ok(true);

        return Status::OK;
    }
};

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

void RunServer(string selfAddress) {
    MessageBusServiceImpl service;
    string server_address(selfAddress);

    EnableDefaultHealthCheckService(true);
    reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Server listening on " << server_address << endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv) {
    cout << "Starting messagebus" << endl;

    if (readEnvironmentVariables()) {
        cerr << "You must set environment variables: [serverAddress, peerAddresses, regionName, clusterName]" << endl;
        return 1;
    }

    cout << "Before updates:" << endl;
    printGlobalConfigLocal(currGlobalConfig);
    cout << endl;

    RunServer(selfAddress);
    return 0;
}

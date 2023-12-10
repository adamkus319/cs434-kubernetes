// #include <grpcpp/grpcpp.h>

#include <grpc/grpc.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <iostream>

#include "codegen/message.grpc.pb.h"
#include "codegen/message.pb.h"

using namespace std;
using namespace messagebus;
using namespace grpc;

class MessageBusServiceImpl : public MessageBus::Service {
    Status UserUpdateConfig(ServerContext* context, const updateConfigRequest* request, updateConfigResponse* response) override {
        if (request->regionname().size() == 0) return Status(StatusCode::INVALID_ARGUMENT, "error: Argument not found");
        if (request->servicename().size() == 0) return Status(StatusCode::INVALID_ARGUMENT, "error: Argument not found");
        if (request->count() == 0) return Status::OK;

        // parse and send data to scheduler

        // print request data
        cout << "Region: " << request->regionname() << endl;
        cout << "Service: " << request->servicename() << endl;
        cout << "Count: " << request->count() << endl;

        // send ack to stream
        response->set_ok(true);

        return Status::OK;
    }
    Status PeerUpdateConfig(ServerContext* context, const GlobalConfig* request, updateConfigResponse* response) override {
        // print request data

        // send ack to stream
        response->set_ok(true);

        return Status::OK;
    }
};

void RunServer() {
    MessageBusServiceImpl service;
    string server_address("0.0.0.0:50051");

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
    RunServer();
    return 0;
}
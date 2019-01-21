#include <iostream>
#include <memory>
#include <string>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "messagedef.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using messagedef::SampleDataMessage;
using messagedef::Response;
using messagedef::SampleData;


class SimpleServerImpl final : public SampleData::Service {

    Status SendData(ServerContext* context, const SampleDataMessage* message, Response* response) override
    {
        response->set_message("test");
        return Status::OK;
    }

};

void RunServer() 
{
    std::string server_address("localhost:50051");
    SimpleServerImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv)
{
    RunServer();
    return 0;
}
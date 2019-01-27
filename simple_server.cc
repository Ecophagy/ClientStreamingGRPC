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
using grpc::ServerReader;
using grpc::Status;
using messagedef::SampleDataMessage;
using messagedef::Response;
using messagedef::SendSampleData;


class SimpleServerImpl final : public SendSampleData::Service {

    Status SendData(ServerContext* context, ServerReader<SampleDataMessage>* reader, Response* response) override
    {
        SampleDataMessage message;
        int chunks = 0;
        // Count the number of chunks received
        while (reader->Read(&message))
        {
            chunks++;
        }
        response->set_message(std::to_string(chunks));
        return Status::OK;
    }

};

void RunServer() 
{
    std::string server_address("localhost:50051");

    ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    SimpleServerImpl service;
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
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

/**
 *  Implentation of the SendSampleData servive defined in messagedef.proto
 */
class SimpleServerImpl final : public SendSampleData::Service {

    Status SendData(ServerContext* context, ServerReader<SampleDataMessage>* reader, Response* response) override
    {
        SampleDataMessage message;
        int chunks = 0;
        // Receive a stream of messages
        while (reader->Read(&message))
        {
            // Each message contains a chunk of the file sent
            chunks++;
        }
        //Return how many chunks we received
        response->set_chunksreceived(chunks);
        return Status::OK;
    }

};

// Set up and run the server
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
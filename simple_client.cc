#include <memory>

#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include "messagedef.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using messagedef::SampleData;
using messagedef::SampleDataMessage;
using messagedef::Response;

class SimpleClient {
    private:
        std::unique_ptr<SampleData::Stub> stub_;

    public:
        SimpleClient(std::shared_ptr<Channel> channel) : stub_(SampleData::NewStub(channel)) {}

        bool SendData(const SampleDataMessage& message, Response* response)
        {
            ClientContext context;
            Status status = stub_->SendData(&context, message, response);
            if (!status.ok()){
                std::cout << "SendData rpc failed" << std::endl;
                return false;
            }
            if (response->message().empty())
            {
                std::cout << "No message in response" << std::endl;
            }
            else 
            {
                std::cout << "message received: " << response->message() << std::endl;
                return true;
            }
        }

};

int main(int argc, char** argv)
{
    SimpleClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    SampleDataMessage message;
    message.set_stringfield("test");
    message.set_numberfield(6);
    message.set_filefield("bytestring");
    Response response;

    client.SendData(message, &response);

    return 0;
}
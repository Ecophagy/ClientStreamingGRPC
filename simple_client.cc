#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include "messagedef.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;
using messagedef::SampleData;
using messagedef::SampleDataMessage;
using messagedef::Response;

class SimpleClient {
    private:
        std::unique_ptr<SampleData::Stub> stub_;

    public:
        SimpleClient(std::shared_ptr<Channel> channel) : stub_(SampleData::NewStub(channel)) {}

        bool SendData(std::string stringField, int numberField, std::string filePath)
        {
            Response response;
            ClientContext context;
            std::unique_ptr<ClientWriter<SampleDataMessage>> writer(stub_->SendData(&context, &response));

            std::ifstream file(filePath, std::ios::binary);
            std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
            int chunkSize = 64 * 1024; // 64 KiB
            int numberOfChunks = std::ceil((float)buffer.size()/(float)chunkSize);

            for(int i=0; i < numberOfChunks; i++)
            {
                SampleDataMessage message;
                message.set_stringfield(stringField);
                message.set_numberfield(numberField);

                auto sliceStart = buffer.begin() + i*chunkSize;
                auto sliceEnd = i==numberOfChunks ? buffer.end() : buffer.begin() + (i+1)*chunkSize;

                std::vector<char> slice(sliceStart, sliceEnd);
                message.set_filefieldchunk(std::string(slice.begin(), slice.end()));
                
                if(!writer->Write(message))
                {
                    //Broken stream
                    break;
                }
            }
            writer->WritesDone();
            Status status = writer->Finish();

            if (!status.ok()){
                std::cout << "SendData rpc failed" << std::endl;
                return false;
            }
            if (response.message().empty())
            {
                std::cout << "No message in response" << std::endl;
            }
            else 
            {
                std::cout << "message received: " << response.message() << std::endl;
                return true;
            }
        }

};

int main(int argc, char** argv)
{
    // Check argument numbers are correct
    if (argc != 4)
    {
        std::cerr << "Please provide a string, integer, and file path" << std::endl;
        return 1;
    } 

    SimpleClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    try 
    {
        client.SendData(argv[1], std::stoi(argv[2]), argv[3]);
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "integer parameter \'" << argv[2] << "\' is not a valid integer" << std::endl;
        return 1;
    }

    return 0;
}
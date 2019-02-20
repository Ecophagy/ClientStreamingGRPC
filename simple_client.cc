#include <memory>
#include <fstream>
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
using messagedef::SendSampleData;
using messagedef::SampleDataMessage;
using messagedef::Response;

/**
 *  Simple client that sends messages to the server
 */
class SimpleClient {
    private:
        std::unique_ptr<SendSampleData::Stub> stub_;
        const int chunkSize = 64 * 1024; // 64 KiB

    public:
        SimpleClient(std::shared_ptr<Channel> channel) : stub_(SendSampleData::NewStub(channel)) {}

        bool SendData(std::string stringField, int numberField, std::string filePath)
        {
            Response response;
            ClientContext context;
            
            // Set up the streaming writer
            std::unique_ptr<ClientWriter<SampleDataMessage>> writer(stub_->SendData(&context, &response));

            // Open the file and read it into memory
            std::ifstream file(filePath, std::ios::binary);
            std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});

            // Calculate how many chunks the file will be broken in to
            int numberOfChunks = std::ceil((float)buffer.size()/(float)chunkSize);

            // Send the message to the server with the file broken into chunks
            for(int i=0; i < numberOfChunks; i++)
            {
                SampleDataMessage message;
                message.set_stringfield(stringField);
                message.set_numberfield(numberField);

                auto sliceStart = buffer.begin() + i*chunkSize;

                // If the last piece of the file is smaller than the chunk size, read to the end of the buffer 
                auto sliceEnd = i==numberOfChunks-1 ? buffer.end() : buffer.begin() + (i+1)*chunkSize;

                std::vector<char> slice(sliceStart, sliceEnd);
                message.set_filefieldchunk(std::string(slice.begin(), slice.end()));
                
                // Send the message
                if(!writer->Write(message))
                {
                    //Broken stream
                    break;
                }
            }
            // All chunks sent, so tell the server we're done
            writer->WritesDone();
            Status status = writer->Finish();

            // Check response status and return accordingly
            if (!status.ok()){
                std::cerr << "SendData rpc failed" << std::endl;
                return false;
            }
            if (response.chunksreceived() == 0)
            {
                std::cerr << "Server received no chunks" << std::endl;
            }
            else 
            {
                std::cout << response.chunksreceived() << " chunks received" << std::endl;
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

    // Create client channel
    SimpleClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    // Call rpc
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
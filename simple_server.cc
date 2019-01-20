#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include "messagedef.grpc.pb.h"

class SimpleServerImpl  final : public SampleData::Service {

};

int main()
{
    //XXX: Temporary
    return 0;
}
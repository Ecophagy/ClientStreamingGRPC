CXX =g++
CPPFLAGS += `pkg-config --clflags protobuf grpc`
CXXFLAGS += -std=c++11

PROTOS_PATH = protos

vpath %.proto $(PROTOS_PATH)


all:

%.grpc.pb.cc: %.proto
	protoc -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

%.pb.cc: %.proto
	protoc -I $(PROTOS_PATH) --cpp_out=. $<
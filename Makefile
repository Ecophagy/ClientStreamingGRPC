CXX =g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11

LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
			-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
			-ldl

PROTOS_PATH = protos

vpath %.proto $(PROTOS_PATH)


all: simple_server #simple_client

simple_server: messagedef.pb.o messagedef.grpc.pb.o simple_server.o
	$(CXX) $^ $(LDFLAGS) -o $@

simple_client: #TODO

%.grpc.pb.cc: %.proto
	protoc -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

%.pb.cc: %.proto
	protoc -I $(PROTOS_PATH) --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h simple_server #simple_client
	
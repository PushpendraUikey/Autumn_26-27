CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: tool generate server_bin client_bin

tool: tool.cpp
	$(CXX) $(CXXFLAGS) -o tool tool.cpp

# Running the tool generates proxy.h/proxy.cpp/stuff.h/stuff.cpp from service.spec
generate: tool service.spec
	./tool service.spec

server_bin: generate server.cpp stuff.cpp
	$(CXX) $(CXXFLAGS) -o server_bin server.cpp stuff.cpp

client_bin: generate client.cpp proxy.cpp
	$(CXX) $(CXXFLAGS) -o client_bin client.cpp proxy.cpp

clean:
	rm -f tool server_bin client_bin proxy.h proxy.cpp stuff.h stuff.cpp *.o server.log

.PHONY: all generate clean

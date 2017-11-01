CC=clang++

all: 
	clang++ -o throughput test_throughput.cpp -std=c++11 -O3 -lpthread -luWS -lssl -lcrypto -lz -luv -DUSE_LIBUV -lmongoc-1.0 -lbson-1.0

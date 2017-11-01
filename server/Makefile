CC=clang++

all: server benchmark

server: server.o db.o
	clang++ -o server server.o db.o -lpthread -luWS -lssl -lcrypto -lz -luv -DUSE_LIBUV -lmongoc-1.0 -lbson-1.0

benchmark: benchmark.cpp
	clang++ -std=c++11 -o benchmark benchmark.cpp -lpthread -luWS -lssl -lcrypto -lz -luv -DUSE_LIBUV

server.o: server.cpp
	clang++ -c -o server.o -std=c++11 server.cpp

db.o: db.c
	clang -c -o db.o -std=c99 db.c -I/usr/local/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0
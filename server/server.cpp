#include <uWS/uWS.h>
#include <array>
#include <iostream>
#include <cstdint> // better int types

extern "C" {
#include "db.h"
}

// clang++ -std=c++11 echo.cpp -lpthread -luWS -lssl -lcrypto -lz -luv -DUSE_LIBUV

static uint64_t uuid = 1;
db_handle_t* handle;

void message_recv(uWS::WebSocket<uWS::SERVER> *ws,
    char *message,
    uint64_t length,
    uWS::OpCode opCode) {

    // Verify message matches what we expect.
    if (opCode != uWS::OpCode::BINARY) {
        std::cerr << "expected binary message but got " << opCode << std::endl;
        return;
    }

    if (length % 4 != 0) {
        std::cerr << "expected 4 byte aligned" << std::endl;
        return;
    }

    uint64_t id = (uint64_t)ws->getUserData();
    std::cout << "got message of length " << length << " from user " << id << std::endl;
    record_data(handle, id, message, length);
}

int main()
{
    uWS::Hub h;
    const int port = 3000;

    handle = init();

    h.onMessage(message_recv);

    h.onConnection([](uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req) {
        std::cout << "Got connection" << std::endl;
        ws->setUserData((void*)uuid++);
    });

    if (h.listen(port)) {
        std::cout << "Listening on port " << port << std::endl;
        h.run();
    }

    destroy (handle);
}

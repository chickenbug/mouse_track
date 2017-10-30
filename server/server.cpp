#include <uWS/uWS.h>
#include <array>
#include <iostream>
#include <cstdint> // better int types

extern "C" {
#include "db.h"
}

// clang++ -std=c++11 echo.cpp -lpthread -luWS -lssl -lcrypto -lz -luv -DUSE_LIBUV

static uint64_t uuid = 1;

void messageRecv(uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
    if (opCode == uWS::OpCode::BINARY) {
        /* TODO: read a chunk of points */
        uint64_t id = (uint64_t)ws->getUserData();
        std::cout << "Got binary message of length " << length << " from user " << id << std::endl;
        // Insert into mongodb
        record_data(uuid, 10, 20);
    }
}

int main()
{
    uWS::Hub h;
    const int port = 3000;

    h.onMessage(messageRecv);

    h.onConnection([](uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req) {
        std::cout << "Got connection" << std::endl;
        ws->setUserData((void*)uuid++);
    });

    if (h.listen(port)) {
        std::cout << "Listening on port " << port << std::endl;
        h.run();
    }
}

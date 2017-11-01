#include <uWS/uWS.h>
#include <array>
#include <iostream>
#include <cstdint> // better int types
#include "test.h"

extern "C" {
#include "db.h"
}

// clang++ -std=c++11 echo.cpp -lpthread -luWS -lssl -lcrypto -lz -luv -DUSE_LIBUV

static uint64_t uuid = 1;
db_handle_t* handle;

static bool in_test = false;

typedef uWS::WebSocket<uWS::SERVER> ServerSocket;
std::vector<ServerSocket*> test_sockets;
int32_t test_bytes_expecting = 0;


void message_recv(uWS::WebSocket<uWS::SERVER> *ws,
    char *message,
    uint64_t length,
    uWS::OpCode opCode) {

    if (length == 0) return;

    if (length == 5 && message[0] == MSG_CLIENT_TEST) {
        // Add sockets.
        test_sockets.push_back (ws);

        // Next four bytes are expected total bytes to receive in the test.
        message++;
        int32_t* as_int32_ptr = (int32_t*)message;
        in_test = true;
        test_bytes_expecting = *as_int32_ptr;
        std::cout << "server test initialized. Expecting " << test_bytes_expecting << " bytes" << std::endl;

        // Acknowledge test starts
        char ack[] = { MSG_SERVER_ACK };
        ws->send (ack, 1, uWS::OpCode::BINARY);

        return;
    }

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

    if (in_test) {
        test_bytes_expecting -= (int32_t)length;

        if (test_bytes_expecting <= 0) {
            std::cout << "end of test, broadcasting ALL_RECV" << std::endl;
            char ack[] = { MSG_SERVER_ALL_RECV };
            for (ServerSocket* ws : test_sockets) {
                ws->send (ack, 1, uWS::OpCode::BINARY);
            }
            test_sockets.clear();
            in_test = false;
            test_bytes_expecting = 0;
        }
    }
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

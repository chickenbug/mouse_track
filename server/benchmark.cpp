#include <iostream>
#include <uWS/uWS.h>
#include <cstdint>
#include <vector>
#include "test.h"

/*
This is a simple websocket server benchmark which does the following:
1. Creates X websocket connections.
2. Sends a message telling the server the test is starting and how many total bytes to expect.
3. Sends Y bytes on each socket
4. Receive a message from the server after finished processing
5. Record timing
*/

typedef uWS::WebSocket<uWS::CLIENT> ClientSocket;

std::vector<ClientSocket*> sockets;
ClientSocket* master = NULL;

const int numConnections = 1;
const int messageSize = 1000; // in bytes (should be divisible by 4).
const int messagesToBroadcast = 30;
int messagesBroadcasted = 0;
char* premadeMessage;

const int numTrials = 3;
int trial = 0;

int numAcks = 0;
int numAllRecv = 0;

std::chrono::high_resolution_clock::time_point start;

void startTrial();
void transmit();

void onRecv (char* msg, size_t len) {
    std::cout << "got message " << std::endl;
    if (len != 1) return;

    if (msg[0] == MSG_SERVER_ACK) {
        numAcks++;
        if (numAcks < numConnections) return;

        // Ready to start test.
        transmit ();
    }

    if (msg[0] == MSG_SERVER_ALL_RECV) {
        numAllRecv++;
        if (numAllRecv < numConnections) return;

        // End of test.
        std::cout << "recv end of test" << std::endl;
        auto now = std::chrono::high_resolution_clock::now();
        auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds> (now - start);
        std::cout << "took " << durationMs.count() << "ms" << std::endl;
        auto durationUs = std::chrono::duration_cast<std::chrono::microseconds> (now - start);

        int totalBytesSent = messageSize * numConnections * messagesToBroadcast;
        double throughput = (double)totalBytesSent / (double)durationMs.count();
        std::cout << "effective throughput is " << throughput << " bytes per ms" << std::endl;
        std::cout << "per connection " << (throughput / numConnections) << " bytes per ms" << std::endl;
        trial++;
        startTrial ();
    }
}

void afterBroadcast (ClientSocket* ws, void *data, bool cancelled, void *reserved) {
    messagesBroadcasted++;
    transmit();
}

void transmit () {
    if (messagesBroadcasted >= messagesToBroadcast) return;
    // std::cout << "broadcasting round " << messagesBroadcasted << std::endl;

    for (int i = 0; i < sockets.size(); i++) {
        if (i == sockets.size() - 1) {
            // The last message calls the next transmit after sending.
            sockets[i]->send (premadeMessage, messageSize, uWS::OpCode::BINARY, afterBroadcast);
        } else {
            sockets[i]->send (premadeMessage, messageSize, uWS::OpCode::BINARY);
        }
    }
}

void cleanup() {
    for (ClientSocket* ws : sockets) {
        ws->close();
    }
}

void startTrial() {
    if (trial >= numTrials) return cleanup();

    char msg[5] = { MSG_CLIENT_TEST, 0, 0, 0, 0 };
    int32_t* as_int32_ptr = (int32_t*) ((char*)msg + 1);
    *as_int32_ptr = messageSize * numConnections * messagesToBroadcast;

    // Reset state.
    numAcks = 0;
    numAllRecv = 0;
    messagesBroadcasted = 0;

    // Start test.
    std::cout << "Sending MSG_CLIENT_TEST" << std::endl;
    start = std::chrono::high_resolution_clock::now();

    // Send on every socket and get an ack for each to start the test.
    for (ClientSocket* ws : sockets) {
        ws->send (msg, 5, uWS::OpCode::BINARY);
    }
}

int main () {
    uWS::Hub h;

    if (messageSize % 4 != 0) {
        std::cerr << "messageSize is not divisible by 4" << std::endl;
        return 1;
    }

    const char* uri = "ws://127.0.0.1:3000";

    premadeMessage = (char*)malloc(messageSize);
    for (int i = 0; i < messageSize; i++) premadeMessage[i] = i % 128;

    h.onConnection([&h, uri](ClientSocket* ws, uWS::HttpRequest req) {
        std::cout << "established connection" << std::endl;
        sockets.push_back (ws);

        if (sockets.size() == 1) master = sockets[0];

        if (sockets.size() >= numConnections) {
            startTrial ();
        } else {
            h.connect(uri);
        }
    });

    h.onError([](void* err) {
        std::cerr << "error occurred" << std::endl;
    });

    h.onMessage([](ClientSocket* ws, char* msg, size_t len, uWS::OpCode opCode){
        onRecv (msg, len);
    });

    h.connect(uri);
    h.run();
}
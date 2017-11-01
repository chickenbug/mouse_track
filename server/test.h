#pragma once

// Special bytes sent between server and testing client to signal a test is occurring.
#define MSG_MASTER 0x01
#define MSG_CLIENT_TEST 0x02
#define MSG_SERVER_ACK 0x03
#define MSG_SERVER_ALL_RECV 0x04
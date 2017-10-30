#include "db.h"

#include "bson.h"
#include "mongoc.h"

// TODO: link with MongoDB.

struct db_handle_t_ {
    mongoc_client_t* client;
};

void record_data (uint64_t uuid, uint16_t x, uint16_t y) {
    bson_t bson;
}

void init (db_handle_t* handle) {

}

void destroy (db_handle_t* handle) {

}
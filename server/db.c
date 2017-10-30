#include "db.h"

#include "bson.h"
#include "mongoc.h"

// TODO: link with MongoDB.

struct db_handle_t_ {
    mongoc_client_t* client_pool;
    mongoc_collection_t* coll;
};

void record_data (uint64_t uuid, uint16_t x, uint16_t y) {
    bson_t bson = BSON_INITIALIZER;
    bson_append_int32(&bson, "x", 1, x);
    bson_append_int32(&bson, "y", 1, y);
    mongoc_collection_insert(coll, MONGOC_INSERT_NONE, &bson, NULL, NULL);
    printf ("inserted\n");
}

db_handle_t* init () {
    mongoc_init ();
    db_handle_t* handle = malloc (sizeof(db_handle_t));
    mongoc_uri_t* uri = mongoc_uri_new (MONGO_URI);
    if (!uri) {
        printf("Could not create uri\n");
        return NULL;
    }
    handle->client_pool = mongoc_client_pool_new (uri);
    mongoc_client_pool_set_error_api (handle->client_pool, 2);
    handle->coll = mongoc_client_get_collection (client, "db", "cursor_positions");
}

void destroy (db_handle_t* handle) {
    mongoc_client_pool_destroy (handle->client_pool);
    free (handle);
    mongoc_cleanup ();
}
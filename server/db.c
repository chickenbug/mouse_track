#include "db.h"

#include "bson.h"
#include "mongoc.h"

struct db_handle_t_ {
    mongoc_client_pool_t* client_pool;
    mongoc_client_t* client;
    mongoc_collection_t* coll;
};

void record_data (db_handle_t* handle,
    uint64_t uuid,
    char* message,
    uint64_t length) {

    bson_t bson = BSON_INITIALIZER;
    bson_t array;
    char buffer[10];
    const char* key;
    uint16_t* as_int16_ptr = (uint16_t*) message;

    // Construct document: { uuid; d: [ {x: <num>, y: <num>}, {x: <num>, y: <num>}... ]}
    bson_append_int64 (&bson, "uuid", 4, uuid);
    bson_append_array_begin (&bson, "d", 1, &array);
    for (uint32_t i = 0; i < (length >> 1); i += 2) {
        uint16_t x = *(as_int16_ptr + i);
        uint16_t y = *(as_int16_ptr + i + 1);
        bson_t child;
        size_t key_length = bson_uint32_to_string(i, &key, buffer, 10);
        bson_append_document_begin(&array, key, key_length, &child);
        bson_append_int32(&child, "x", 1, (uint32_t) x);
        bson_append_int32(&child, "y", 1, (uint32_t) y);
        bson_append_document_end(&array, &child);
    }
    bson_append_array_end (&bson, &array);

    char* str = bson_as_json(&bson, NULL);
    printf ("Constructed document %s\n", str);
    bson_free (str);

    mongoc_collection_insert(handle->coll, MONGOC_INSERT_NONE, &bson, NULL, NULL);

    bson_destroy (&bson);
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
    handle->client = mongoc_client_pool_pop (handle->client_pool);
    mongoc_client_pool_set_error_api (handle->client_pool, 2);
    handle->coll = mongoc_client_get_collection (handle->client, "db", "cursor_positions");

    // Set unacknowledged write concern.
    mongoc_write_concern_t* wc = mongoc_write_concern_new();
    mongoc_write_concern_set_w (wc, MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED);
    mongoc_collection_set_write_concern (handle->coll, (const mongoc_write_concern_t*) wc);
    mongoc_write_concern_destroy (wc);

    return handle;
}

void destroy (db_handle_t* handle) {
    mongoc_client_pool_push (handle->client_pool, handle->client);
    mongoc_client_pool_destroy (handle->client_pool);
    free (handle);
    mongoc_cleanup ();
}
#include "db.h"

#include "bson.h"
#include "mongoc.h"

struct db_handle_t_ {
    mongoc_client_pool_t* client_pool;
    mongoc_client_t* client;
    mongoc_collection_t* coll;
};

#define OPTIMIZE_W0
//#define OPTIMIZE_PREALLOC

bson_t* tmpl_bson;

void record_data (db_handle_t* handle,
    uint64_t uuid,
    char* message,
    uint64_t length) {

    bson_t bson = BSON_INITIALIZER;
    bson_t array;
    char buffer[10];
    const char* key;
    uint16_t* as_int16_ptr = (uint16_t*) message;

    #ifdef OPTIMIZE_PREALLOC
    // Do an "in-place" update.
    char* str = bson_as_json(tmpl_bson, NULL);
    if (strlen(str) > 30) str[30] = '\0';
    printf ("Constructed document %s\n", str);
    bson_free (str);

    mongoc_collection_insert(handle->coll, MONGOC_INSERT_NONE, tmpl_bson, NULL, NULL);

    #else
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
    if (strlen(str) > 30) str[30] = '\0';
    printf ("Constructed document %s\n", str);
    bson_free (str);

    mongoc_collection_insert(handle->coll, MONGOC_INSERT_NONE, &bson, NULL, NULL);

    bson_destroy (&bson);
    // printf ("inserted\n");

    #endif
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
    #ifdef OPTIMIZE_W0
    mongoc_write_concern_t* wc = mongoc_write_concern_new();
    mongoc_write_concern_set_w (wc, MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED);
    mongoc_collection_set_write_concern (handle->coll, (const mongoc_write_concern_t*) wc);
    mongoc_write_concern_destroy (wc);
    #endif

    // Preallocate a bson_t which we will re-use.
    #ifdef OPTIMIZE_PREALLOC
    tmpl_bson = bson_new();
    char buffer[10];
    bson_t array, child;
    const char* key;
    bson_append_int64 (tmpl_bson, "uuid", 4, 1);
    bson_append_array_begin (tmpl_bson, "d", 1, &array);
    // TODO: THIS ASSUMES CHUNK SIZE OF 50 (100 pairs, 200 bytes total).
    for (uint32_t i = 0; i < 50; i += 2) {
        bson_t child;
        size_t key_length = bson_uint32_to_string(i, &key, buffer, 10);
        bson_append_document_begin(&array, key, key_length, &child);
        bson_append_int32(&child, "x", 1, 0);
        bson_append_int32(&child, "y", 1, 0);
        bson_append_document_end(&array, &child);
    }
    bson_append_array_end (tmpl_bson, &array);
    #endif


    return handle;
}

void test_clear_state (db_handle_t* handle) {
    printf ("dropping collection\n");
    bson_error_t err;
    if (!mongoc_collection_drop (handle->coll, &err)) {
        fprintf (stderr, "could not drop %s", err.message);
    }

}

void destroy (db_handle_t* handle) {
    mongoc_client_pool_push (handle->client_pool, handle->client);
    mongoc_client_pool_destroy (handle->client_pool);
    free (handle);
    mongoc_cleanup ();
}
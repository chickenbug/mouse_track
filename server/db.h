#ifndef DB_H
#define DB_H
#include <stdint.h>

#define MONGO_URI "mongodb://127.0.0.1/?appname=pool-example"

typedef struct db_handle_t_ db_handle_t;
db_handle_t* init();
void record_data (db_handle_t*, uint64_t uuid, char* message, uint64_t length);
void destroy(db_handle_t*);
void test_clear_state (db_handle_t*);

#endif
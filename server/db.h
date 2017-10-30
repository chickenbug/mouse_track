#ifndef DB_H
#define DB_H
#include <stdint.h> // better int types

#define MONGO_URI "mongodb://127.0.0.1/?appname=pool-example"

typedef struct db_handle_t_ db_handle_t;
db_handle_t* init();
void record_data (uint64_t uuid, uint16_t x, uint16_t y);
void destroy(db_handle_t*);

#endif
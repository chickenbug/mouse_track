#ifndef DB_H
#define DB_H
#include <stdint.h> // better int types

typedef struct db_handle_t_ db_handle_t;
void init(db_handle_t*);
void record_data (uint64_t uuid, uint16_t x, uint16_t y);
void destroy(db_handle_t*);

#endif
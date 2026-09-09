#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdint.h>

#include "hashtable.h"

struct symbol {
	struct hashitem ht;
	uint16_t val;
	bool undefined;
	char name[];
};

struct hashtable *symbol_table_init(void);
void symbol_table_destroy(struct hashtable *symbol_table);
bool symbol_table_insert(struct hashtable *symbol_table,
			 const char *name, uint16_t val);
bool symbol_table_insert_undef(struct hashtable *symbol_table,
			 const char *name);
struct symbol *symbol_table_lookup(struct hashtable *symbol_table,
				   const char *name);

#endif

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

#include "list.h"

#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

#define HASHTABLE_FOREACH(var, table)           \
	for (size_t i = 0; i < (table)->size; i++) \
		LIST_FOREACH(var, (table)->buckets[i], next)

#define HASHTABLE_FOREACH_SAFE(var, table, tvar) \
	for (size_t i = 0; i < (table)->size; i++)  \
		LIST_FOREACH_SAFE(var, (table)->buckets[i], tvar)

struct hashitem {
	struct hashitem *next;
	void *key;
};

struct hashtable_ops {
	size_t (*hash)(const void *item);
	bool (*comp)(const void *a, const void *b);
	void (*free_item)(void *item);
};

struct hashtable {
	size_t size;
	struct hashtable_ops ops;
	struct hashitem *buckets[];
};

struct hashtable *hashtable_create(size_t size, const struct hashtable_ops *ops);
void hashtable_insert(struct hashtable *h, void *key, void *item);
void *hashtable_lookup(struct hashtable *h, const void *key);
void hashtable_remove(struct hashtable *h, void *item);
void hashtable_destroy(struct hashtable *h);

#endif

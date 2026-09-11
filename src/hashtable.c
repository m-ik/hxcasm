#include <stdlib.h>

#include "hashtable.h"

struct hashtable *hashtable_create(size_t size, const struct hashtable_ops *ops)
{
	struct hashtable *h = calloc(
		1, sizeof(struct hashtable) + size * sizeof(struct hashitem *));
	if (!h)
		return NULL;

	h->size = size;
	h->ops.hash = ops->hash;
	h->ops.comp = ops->comp;
	h->ops.free_item = ops->free_item;

	return h;
}

void hashtable_insert(struct hashtable *h, void *key, void *item)
{
	size_t hash = h->ops.hash(key) % h->size;
	struct hashitem **bucket = &h->buckets[hash];
	struct hashitem *new_item = item;
	new_item->next = *bucket;
	new_item->key = key;
	*bucket = new_item;
}

void *hashtable_lookup(struct hashtable *h, const void *key)
{
	size_t hash = h->ops.hash(key) % h->size;
	struct hashitem *item = h->buckets[hash];
	LIST_FOREACH(item, h->buckets[hash]) {
		if (h->ops.comp(key, item->key))
			return item;
	}

	return NULL;
}

void hashtable_remove(struct hashtable *h, void *item)
{
	size_t hash = h->ops.hash(item) % h->size;
	struct hashitem **bucket = &h->buckets[hash];
	struct hashitem *prv = NULL;
	for (struct hashitem *cur = *bucket; cur; prv = cur, cur = cur->next) {
		if (h->ops.comp(item, cur)) {
			if (prv)
				prv->next = cur->next;
			else
				*bucket = cur->next;
			break;
		}
	}
}

void hashtable_destroy(struct hashtable *h)
{
	struct hashitem *item = NULL;
	struct hashitem *tmp = NULL;
	HASHTABLE_FOREACH_SAFE(item, h, tmp) {
		hashtable_remove(h, item);
		if (h->ops.free_item)
			h->ops.free_item(item);
		item = NULL;
	}
	free(h);
}

#include <nmmintrin.h>
#include <string.h>
#include <stdlib.h>

#include "symbols.h"
#include "hack_defs.h"
#include "hashtable.h"

static struct symbol *symbol_new(const char *name, size_t len, uint16_t val)
{
	struct symbol *sym = malloc(sizeof(struct symbol) + len + 1);
	if (!sym)
		return NULL;

	memcpy(sym->name, name, len + 1);
	sym->val = val;
	sym->undefined = false;
	sym->ht.next = NULL;
	sym->ht.key = sym->name;

	return sym;
}

static void symbol_free(void *sym)
{
	free(sym);
}

static size_t symbol_hash(const void *s)
{
	const char *key = s;
	size_t hash = 42;
	while (*key != '\0') {
		hash = _mm_crc32_u8(hash, *key);
		key++;
	}
	return hash;
}

static bool symbol_comp(const void *s1, const void *s2)
{
	const char *sym1 = s1;
	const char *sym2 = s2;
	return !strcmp(sym1, sym2);
}

struct hashtable *symbol_table_init(void)
{
	static const struct hashtable_ops symbol_ops = {
		.hash = symbol_hash,
		.comp = symbol_comp,
		.free_item = symbol_free,
	};
	struct hashtable *symbol_table = hashtable_create(256, &symbol_ops);
	if (!symbol_table)
		goto error;

	for (int i = 0; predef_table[i].name; i++) {
		const struct hack_table_entry *e = &predef_table[i];
		if (!symbol_table_insert(symbol_table, e->name, e->val))
			goto error;
	}

	return symbol_table;

error:
	hashtable_destroy(symbol_table);
	return NULL;
}

void symbol_table_destroy(struct hashtable *symbol_table)
{
	hashtable_destroy(symbol_table);
}

bool symbol_table_insert(struct hashtable *symbol_table, const char *name,
			 uint16_t val)
{
	struct symbol *sym = symbol_new(name, strlen(name), val);
	if (!sym)
		return false;

	hashtable_insert(symbol_table, sym->name, sym);

	return true;
}

bool symbol_table_insert_undef(struct hashtable *symbol_table, const char *name)
{
	struct symbol *sym = symbol_new(name, strlen(name), 0);
	if (!sym)
		return false;
	sym->undefined = true;

	hashtable_insert(symbol_table, sym->name, sym);

	return true;
}

struct symbol *symbol_table_lookup(struct hashtable *symbol_table,
				   const char *name)
{
	struct symbol *sym = NULL;

	struct hashitem *found = hashtable_lookup(symbol_table, name);
	if (found)
		sym = container_of(found, struct symbol, ht);

	return sym;
}

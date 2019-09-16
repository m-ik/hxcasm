#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "dict.h"

bool
lut_lookup(char *key, symbol_t *lut, int sz, uint16_t *code)
{
	for (int i = 0 ; i < sz ; i++) {
		if (!strcmp(key, lut->str)) {
			*code = lut->val;
			return true;
		}
		lut++;
	}

	return false;
}

dict_t *
init_dict(void)
{
	dict_t *dict = malloc(sizeof(dict_t));
	dict->len = 0;
	dict->cap = 1;
	dict->entries = malloc(dict->cap * GROWTH_SIZE * sizeof(symbol_t));

	return dict;
}

void
dict_insert(dict_t *d, symbol_t e)
{
	if (d->len == d->cap * GROWTH_SIZE) {
		d->cap++;
		d->entries = realloc(d->entries,
				     d->cap * GROWTH_SIZE * sizeof(symbol_t));
	}

	d->entries[d->len].str = malloc((strlen(e.str)+1) * sizeof(char));
	strcpy(d->entries[d->len].str, e.str);
	d->entries[d->len].val = e.val;
	d->len++;
}

bool
dict_lookup(char *key, dict_t *d, uint16_t *val)
{
	if (d->len == 0) {
		return false;
	}

	for (int i = 0 ; i < d->len ; i++) {
		if (!strcmp(key, d->entries[i].str)) {
			*val = d->entries[i].val;
			return true;
		}
	}

	return false;
}

void
free_dict(dict_t *d)
{
	for (int i = 0 ; i < d->len ; i++) {
		free(d->entries[i].str);
	}
	free(d->entries);
	free(d);
}

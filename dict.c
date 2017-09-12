#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "dict.h"

#define GROWTH_FACTOR	32

dict_t *
init_dict(void)
{
	dict_t *dict;
	dict = malloc(sizeof(dict_t));
	dict->len = 0;
	dict->cap = 1;
	dict->entries = malloc(dict->cap * GROWTH_FACTOR * sizeof(symbol_t));

	return dict;
}

void
dict_insert(dict_t *d, symbol_t e)
{
	if(d->len == d->cap * GROWTH_FACTOR)
	{
		d->cap++;
		d->entries = realloc(d->entries, 
						d->cap * GROWTH_FACTOR * sizeof(symbol_t));
	}

	d->entries[d->len].str = malloc((strlen(e.str)+1) * sizeof(char));
	strcpy(d->entries[d->len].str, e.str);
	d->entries[d->len].val = e.val;
	d->len++;
}

/*
	function:		dict_lookup
	description:	finds if input string exists in a dictionary
					of type "symbol_t" and returns its corresponding
					code
	return:			code of the symbol (uint16_t)
					NULL if string was not found in the dictionary
 */

bool
dict_lookup(char *key, dict_t *d, uint16_t *val)
{
	int i;

	if(d->len == 0)
		return false;

	for(i = 0 ; i < d->len ; i++)
	{
		if(!strcmp(key, d->entries[i].str))
		{
			*val = d->entries[i].val;
			return true;
		}
	}

	return false;
}

void
free_dict(dict_t *d)
{
	free(d->entries);
	free(d);
}

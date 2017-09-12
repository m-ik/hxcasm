#ifndef __DICT_H__
#define __DICT_H__

#include <stdint.h>

typedef struct symbol_t
{
	char *str;
	uint16_t val;
} symbol_t;

typedef struct dict_t
{
	int len;
	int cap;
	symbol_t *entries;
} dict_t;

dict_t *init_dict(void);
void dict_insert(dict_t *d, symbol_t e);
bool dict_lookup(char *key, dict_t *d, uint16_t *val);
void free_dict(dict_t *d);

#endif

#ifndef __DICT_H__
#define __DICT_H__

#include <stdint.h>

#define GROWTH_SIZE		32

typedef struct symbol_t {
	char *str;
	uint16_t val;
} symbol_t;

typedef struct dict_t {
	int len;
	int cap;
	symbol_t *entries;
} dict_t;

/*
	function:		lut_lookup
	description:	finds if input string exists in a lookup table
					of type "symbol_t" and writes it to *code
	return:			true - if symbol was found
					false - if symbol was not found
 */

bool lut_lookup(char *key, symbol_t *lut, int sz, uint16_t *code);

dict_t *dict_new(void);

/*
	function:		dict_insert
	description:	inserts a new symbol in the dictionary
	return:			-
 */

void dict_insert(dict_t *d, symbol_t e);

/*
	function:		dict_lookup
	description:	finds if input string exists in a dictionary
					of type "symbol_t" and writes it to *code
	return:			true - if key was found
					false - if key was not found
 */

bool dict_lookup(char *key, dict_t *d, uint16_t *val);
void dict_free(dict_t *d);

#endif

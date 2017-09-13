#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdint.h>

#ifdef DEBUG
#ifdef __GNUC__
#define DEBUG_PRINT(fmt, var) printf("%s:%d:%s(): "#var" = " fmt"\n", \
									__FILE__, __LINE__, __func__, var)
#else
#define DEBUG_PRINT(fmt, var) printf(#var" = " fmt"\n", var)
#endif
#else
#define DEBUG_PRINT(fmt, var) do {} while(0)
#endif

/*
	function:		isnum
	description:	checks if a string is a representation
					of a 16-bit unsigned integer and writes this 
					integer to num argument
	return:			true - if the string represents a 16-bit unsigned integer
					false - if not
 */

bool isnum(char *str, uint16_t *num);

/*  
	function:		fgetl
	description:	returns a line from a file
	return:			line as a string
*/

char *fgetl(FILE *fp);

/*
	function:		trim_whitespace
	description:	removes white space from a string
	return:			-
 */

void trim_whitespace(char *str);

/*
	function:		remove_comments
	description:	removes comment lines and inline comments
	return:			0 - valid comment
					1 - invalid comment
 */

int remove_comments(char *str);

/*
	function:		print_binary16
	description:	prints an 16-bit unsigned integer in binary format
	return:			-
 */

void print_binary16(uint16_t num);

/*
	function:		fprint_binary16
	description:	writes a 16-bit unsigned integer in 
					binary format (ASCII) to a file
	return:			-
 */

void fprint_binary16(FILE *fp, uint16_t num);

#endif

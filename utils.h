#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdint.h>

char *fgetl(FILE *fp);
void trim_whitespace(char *str);
void remove_comments(char *str);
void print_binary16(uint16_t num);
void fprint_binary16(FILE *fp, uint16_t num);

#endif

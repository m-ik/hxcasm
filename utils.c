#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"

bool
isnum(char *str, uint16_t *num)
{
	int mul = strlen(str) - 1;
	*num = 0;

	while (*str != '\0') {
		if (*str < 48 || *str > 57) {
			return false;
		}

		if (mul) {
			*num += ((*str - 48) * mul * 10);
		} else {
			*num += (*str - 48);
		}
		str++;
	}

	return true;
}

char
*fgetl(FILE *fp)
{
	char *line = (char *)malloc(512 * sizeof(char));
	int size = 0;
	int resize = 0;
	char curr = '\0';
	while (!feof(fp)) {
		fread(&curr, sizeof(char), 1, fp);
		line[size] = curr;
		size++;

		if (size/512 > resize) {
			resize++;
			line = (char *)realloc(line, resize * 512 * sizeof(char));
		}

		if (curr == '\n' || curr == 13) {
			line[size-1] = '\0';
			break;
		}
	}

	if (feof(fp)) {
		free(line);
		return NULL;
	}

	return line;
}

void
trim_whitespace(char *str)
{
	char *i = str;
	char *j = str;

	while (*j != 0) {
		*i = *j++;
		if ((*i != ' ') && (*i != '\t')) {
			i++;
		}
	}
	*i=0;
}

int
remove_comments(char *str)
{
	while (*str != '\0') {
		if (*str == '/')	{
			if (*(str+1) == '/') {
				*str = '\0';
				return 0;
			} else {
				return 1;
			}
		}
		str++;
	}
	return 0;
}

void
print_binary16(uint16_t num)
{
	printf("0b");
	printf("%1d", (num >> 15) & 1);
	printf("%1d", (num >> 14) & 1);
	printf("%1d", (num >> 13) & 1);
	printf("%1d", (num >> 12) & 1);
	printf("%1d", (num >> 11) & 1);
	printf("%1d", (num >> 10) & 1);
	printf("%1d", (num >> 9) & 1);
	printf("%1d", (num >> 8) & 1);
	printf("%1d", (num >> 7) & 1);
	printf("%1d", (num >> 6) & 1);
	printf("%1d", (num >> 5) & 1);
	printf("%1d", (num >> 4) & 1);
	printf("%1d", (num >> 3) & 1);
	printf("%1d", (num >> 2) & 1);
	printf("%1d", (num >> 1) & 1);
	printf("%1d", num & 1);
	printf("\n");
}

void
fprint_binary16(FILE *fp, uint16_t num)
{
	for (int i = 15 ; i != 0 ; i--) {
		char bit = ((num >> i) & 1) + 48;
		fwrite(&bit, sizeof(char), 1, fp);
	}
}

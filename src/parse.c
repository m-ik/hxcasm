#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"
#include "hack_defs.h"

static bool is_valid_symbol(char *s)
{
	if (*s >= '0' && *s <= '9')
		return false;

	while (*s) {
		if (!(isalnum(*s) || *s == '_' || *s == '$' || *s == '.'))
			return false;
		s++;
	}

	return true;
}

static bool is_valid_constant(char *s)
{
	while (*s) {
		if (!isdigit(*s))
			return false;
		s++;
	}

	return true;
}

static void parse_label(struct parsed_line *parsed, char *s, size_t len)
{
	if (*s != '(' && *(s + len - 1) != ')')
		return;

	*(s + len - 1) = '\0';

	if (!is_valid_symbol(s + 1))
		return;

	parsed->type = LINE_TYPE_LABEL;
	parsed->sym.is_constant = false;
	parsed->sym.name = (s + 1);
}

static void parse_a_instr(struct parsed_line *parsed, char *s)
{
	if (*s != '@')
		return;

	bool is_constant = is_valid_constant(s + 1);
	bool is_symbol = is_valid_symbol(s + 1);

	if (!is_symbol && !is_constant)
		return;

	parsed->type = LINE_TYPE_A_INSTR;
	parsed->sym.is_constant = is_constant;
	parsed->sym.name = (s + 1);
}

static void parse_c_instr(struct parsed_line *parsed, char *s, size_t len)
{
	char *start = s;
	char *equal = strchr(s, '=');
	char *semicolon = strchr(s, ';');

	if ((semicolon && semicolon < equal) ||
	   equal == start ||
	   equal >= (start + len - 1) ||
	   semicolon == start ||
	   semicolon >= (start + len - 1))
		return;

	char *dst = NULL;
	char *cmp = start;
	char *jmp = NULL;
	if (equal) {
		dst = start;
		cmp = equal + 1;
		*equal = '\0';
	}

	if (semicolon) {
		jmp = semicolon + 1;
		*semicolon = '\0';
	}

	if (lookup_table(dst_table, dst ? dst : "") == -1)
		return;
	if (lookup_table(cmp_table, cmp ? cmp : "") == -1)
		return;
	if (lookup_table(jmp_table, jmp ? jmp : "") == -1)
		return;

	parsed->type = LINE_TYPE_C_INSTR;
	parsed->c.dst = dst;
	parsed->c.cmp = cmp;
	parsed->c.jmp = jmp;
}

void parse(struct parsed_line *parsed, char *line, size_t len)
{
	parsed->type = LINE_TYPE_INVALID;

	char *c = NULL;
	size_t new_len = len;
	for (c = line; *c == ' ' || *c == '\t'; c++)
		new_len--;
	char *new_line = c;
	for (c = line + len - 1; *c == ' ' || *c == '\t' || *c == '\n'; c--)
		new_len--;
	*(new_line + new_len) = '\0';

	switch (*new_line) {
	case '\0':
		parsed->type = LINE_TYPE_SKIP;
		break;
	case '/':
		if (*(new_line + 1) == '/')
			parsed->type = LINE_TYPE_SKIP;
		break;
	case '(':
		parse_label(parsed, new_line, new_len);
		break;
	case '@':
		parse_a_instr(parsed, new_line);
		break;
	default:
		parse_c_instr(parsed, new_line, new_len);
		break;
	}
}

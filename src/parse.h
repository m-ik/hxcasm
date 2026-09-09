#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>

enum line_type {
	LINE_TYPE_INVALID,
	LINE_TYPE_SKIP,
	LINE_TYPE_LABEL,
	LINE_TYPE_A_INSTR,
	LINE_TYPE_C_INSTR,
};

struct parsed_line {
	enum line_type type;
	union {
		struct {
			const char *name;
			bool is_constant;
		} sym;
		struct {
			const char *dst;
			const char *cmp;
			const char *jmp;
		} c;
	};
};

void parse(struct parsed_line *parsed, char *line, size_t len);

#endif

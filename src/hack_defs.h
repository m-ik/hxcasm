#ifndef HACK_DEFS_H
#define HACK_DEFS_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define HACK_C_INSTRUCTION_BASE 0xe000
#define HACK_VAR_START_ADDR 0x10

struct hack_table_entry {
	const char *name;
	uint16_t val;
};

// clang-format off
static const struct hack_table_entry predef_table[] = {
	{ "R0",     0x0000 },
	{ "R1",     0x0001 },
	{ "R2",     0x0002 },
	{ "R3",     0x0003 },
	{ "R4",     0x0004 },
	{ "R5",     0x0005 },
	{ "R6",     0x0006 },
	{ "R7",     0x0007 },
	{ "R8",     0x0008 },
	{ "R9",     0x0009 },
	{ "R10",    0x000a },
	{ "R11",    0x000b },
	{ "R12",    0x000c },
	{ "R13",    0x000d },
	{ "R14",    0x000e },
	{ "R15",    0x000f },
	{ "SCREEN", 0x4000 },
	{ "KBD",    0x6000 },
	{ "SP",     0x0000 },
	{ "LCL",    0x0001 },
	{ "ARG",    0x0002 },
	{ "THIS",   0x0003 },
	{ "THAT",   0x0004 },
	{ NULL,     -1     },
};

static const struct hack_table_entry cmp_table[] = {
	{ "0",   0x2a },
	{ "1",   0x3f },
	{ "-1",  0x3a },
	{ "D",   0x0c },
	{ "A",   0x30 },
	{ "!D",  0x0d },
	{ "!A",  0x31 },
	{ "-D",  0x0f },
	{ "-A",  0x33 },
	{ "D+1", 0x1f },
	{ "A+1", 0x37 },
	{ "D-1", 0x0e },
	{ "A-1", 0x32 },
	{ "D+A", 0x02 },
	{ "D-A", 0x13 },
	{ "A-D", 0x07 },
	{ "D&A", 0x00 },
	{ "D|A", 0x15 },
	{ "M",   0x70 },
	{ "!M",  0x71 },
	{ "-M",  0x73 },
	{ "M+1", 0x77 },
	{ "M-1", 0x72 },
	{ "D+M", 0x42 },
	{ "D-M", 0x53 },
	{ "M-D", 0x47 },
	{ "D&M", 0x40 },
	{ "D|M", 0x55 },
	{ NULL,  -1 },
};

static const struct hack_table_entry dst_table[] = {
	{ "",  0x00 },
	{ "M",   0x01 },
	{ "D",   0x02 },
	{ "DM",  0x03 },
	{ "A",   0x04 },
	{ "AM",  0x05 },
	{ "AD",  0x06 },
	{ "ADM", 0x07 },
	{ NULL,  -1 },
};

static const struct hack_table_entry jmp_table[] = {
	{ "",  0x00 },
	{ "JGT", 0x01 },
	{ "JEQ", 0x02 },
	{ "JGE", 0x03 },
	{ "JLT", 0x04 },
	{ "JNE", 0x05 },
	{ "JLE", 0x06 },
	{ "JMP", 0x07 },
	{ NULL,  -1 },
};
// clang-format on

static inline int lookup_table(const struct hack_table_entry table[],
			       const char *name)
{
	for (int i = 0; table[i].name; i++) {
		if (!strcmp(name, table[i].name))
			return table[i].val;
	}
	return -1;
}

#endif

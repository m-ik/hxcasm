#include "encode.h"
#include "hack_defs.h"

static uint16_t encode_c_instr(const char *dst, const char *cmp, const char *jmp)
{
	uint16_t code = HACK_C_INSTRUCTION_BASE;

	int ret;
	if ((ret = lookup_table(cmp_table, cmp ? cmp : "")) != -1)
		code |= (uint16_t)ret << 6;
	if ((ret = lookup_table(dst_table, dst ? dst : "")) != -1)
		code |= (uint16_t)ret << 3;
	if ((ret = lookup_table(jmp_table, jmp ? jmp : "")) != -1)
		code |= (uint16_t)ret;

	return code;
}

uint16_t encode(struct parsed_line *line)
{
	return encode_c_instr(line->c.dst, line->c.cmp, line->c.jmp);
}

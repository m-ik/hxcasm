#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#include "parse.h"
#include "symbols.h"
#include "encode.h"
#include "list.h"
#include "hack_defs.h"

#define DEFAULT_OUTPUT "out.hack"

enum res_status {
	RESOLVE_OK,
	RESOLVE_FAIL_DUP_LABEL,
	RESOLVE_FAIL_ADD_SYMBOL,
	RESOLVE_FAIL_UNDEFINED,
};

struct patch_list {
	long stream_pos;
	struct symbol *sym;
	struct patch_list *next;
};

static void print_usage(void)
{
	printf("Usage: hxcasm file [options]\n");
	printf("Options:\n");
	printf("  -h         Display this information.\n");
	printf("  -o <file>  Write the outputinto <file>.\n");
}

static void die(const char *fmt, ...)
{
	if (fmt[0] != '\0')
		exit(1);

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	exit(1);
}

static void fprint_bin16(FILE *stream, uint16_t num)
{
	uint16_t mask = 0x8000;
	for (size_t i = 0; i < sizeof(uint16_t) * 8; i++, mask >>= 1)
		fputc(num & mask ? '1' : '0', stream);
}

static enum res_status resolve(uint16_t *address, struct parsed_line *line,
			struct hashtable *symbol_table, uint16_t pc,
			struct patch_list **patch_list_tail)
{
	if (line->sym.is_constant) {
		*address = (uint16_t)strtol(line->sym.name, NULL, 10);
		return RESOLVE_OK;
	}

	struct symbol *sym = symbol_table_lookup(symbol_table, line->sym.name);
	if (line->type == LINE_TYPE_A_INSTR) {
		if (!sym) {
			if (!symbol_table_insert_undef(symbol_table,
						line->sym.name))
				return RESOLVE_FAIL_ADD_SYMBOL;

			sym = symbol_table_lookup(symbol_table, line->sym.name);
		}

		if (sym->undefined) {
			struct patch_list *new = malloc(sizeof(struct patch_list));
			new->sym = symbol_table_lookup(symbol_table,
						       line->sym.name);
			new->next = NULL;
			if (*patch_list_tail)
				(*patch_list_tail)->next = new;
			*patch_list_tail = new;
			return RESOLVE_FAIL_UNDEFINED;
		}

		*address = sym->val;
	} else {
		if (!sym) {
			if (!symbol_table_insert(symbol_table, line->sym.name, pc))
				return RESOLVE_FAIL_ADD_SYMBOL;
		} else {
			if (!sym->undefined)
				return RESOLVE_FAIL_DUP_LABEL;

			sym->val = pc;
			sym->undefined = false;
		}

		*address = pc;
	}

	return RESOLVE_OK;
}

static void backpatch(FILE *stream, struct patch_list *patch_list)
{
	uint16_t var_avail_addr = HACK_VAR_START_ADDR;
	struct patch_list *cur = NULL;
	LIST_FOREACH(cur, patch_list) {
		if (cur->sym->undefined) {
			cur->sym->undefined = false;
			cur->sym->val = var_avail_addr++;
		}
		fseek(stream, cur->stream_pos, SEEK_SET);
		fprint_bin16(stream, cur->sym->val);
	}
}

int main(int argc, char *argv[])
{
	char *input = NULL;
	char *output = NULL;

	int opt;
	while ((opt = getopt(argc, argv, "ho:")) != -1) {
		switch (opt) {
		case 'h':
			print_usage();
			die("");
			break;
		case 'o':
			output = optarg;
			break;
		default:
			print_usage();
			die("");
			break;
		}
	}

	if (!(input = argv[optind])) {
		print_usage();
		die("");
	}

	if (!output)
		output = DEFAULT_OUTPUT;

	FILE *input_stream = fopen(input, "r");
	if (!input_stream)
		die("error: cannot open file: %s", input);

	FILE *output_stream = fopen(output, "w+");
	if (!output_stream)
		die("error: cannot open file: %s", output);

	/*
	 * Parse each line, handle symbols and write machine code to the output
	 * in a single pass. Keep a list of unresolved symbols and use it for
	 * backpatching.
	 */

	struct hashtable *symbol_table =
		symbol_table_init();

	char *line = NULL;
	size_t linebufsize = 0;
	ssize_t linesize = 0;
	size_t count = 1;
	uint16_t pc = 0;
	struct patch_list *patch_list_head = NULL;
	struct patch_list *patch_list = NULL;
	while ((linesize = getline(&line, &linebufsize, input_stream)) != -1) {
		struct parsed_line parsed;
		parse(&parsed, line, linesize);

		uint16_t machine_code;
		enum res_status res;
		switch (parsed.type) {
		case LINE_TYPE_C_INSTR:
			pc++;
			machine_code = encode(&parsed);
			fprint_bin16(output_stream, machine_code);
			fputc('\n', output_stream);
			break;
		case LINE_TYPE_A_INSTR:
			pc++;
			res = resolve(&machine_code, &parsed, symbol_table, pc,
				      &patch_list);
			if (!patch_list_head)
				patch_list_head = patch_list;
			switch (res) {
			case RESOLVE_OK:
				fprint_bin16(output_stream, machine_code);
				fputc('\n', output_stream);
				break;
			case RESOLVE_FAIL_UNDEFINED:
				patch_list->stream_pos = ftell(output_stream);
				fputs("0000000000000000\n", output_stream);
				break;
			case RESOLVE_FAIL_ADD_SYMBOL:
				printf("error: line %lu: failed to add symbol\n", count);
				goto exit;
			default:
				break;
			}
			break;
		case LINE_TYPE_LABEL:
			res = resolve(&machine_code, &parsed, symbol_table, pc,
				      &patch_list);
			switch (res) {
			case RESOLVE_FAIL_DUP_LABEL:
				printf("error: line %lu: duplicate label\n", count);
				goto exit;
			case RESOLVE_FAIL_ADD_SYMBOL:
				printf("error: line %lu: failed to add symbol\n", count);
				goto exit;
			default:
				break;
			}
			break;
		case LINE_TYPE_INVALID:
			printf("error: line %lu: failed to parse\n", count);
			goto exit;
		case LINE_TYPE_SKIP:
			break;
		}

		count++;
	}
	backpatch(output_stream, patch_list_head);

exit:
	struct patch_list *tmp = NULL;
	struct patch_list *cur = NULL;
	LIST_FOREACH_SAFE(cur, patch_list_head, tmp)
		free(cur);
	free(line);
	symbol_table_destroy(symbol_table);
	fclose(output_stream);
	fclose(input_stream);

	return 0;
}

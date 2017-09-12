#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "lut.h"
#include "utils.h"

#define GROWTH_FACTOR	16

void
suicide(const char *error_str)
{
	printf("%s\n", error_str);
	exit(1);
}

/*
	function:		dict_lookup
	description:	finds if input string exists in a dictionary
					of type "symbol_t" and returns its corresponding
					code
	return:			code of the symbol (uint16_t)
					NULL if string was not found in the dictionary
 */

bool
lut_lookup(char *key, symbol_t *lut, int sz, uint16_t *code)
{
	int i;

	for(i = 0 ; i < sz ; i++)
	{
		if(!strcmp(key, lut->str))
		{
			*code = lut->val;
			return true;
		}
		lut++;
	}

	return false;
}

bool
is_num(char *str, uint16_t *num)
{
	int mul = strlen(str) - 1;
	*num = 0;

	while(*str != '\0')
	{
		if(*str < 48 || *str > 57)
		{
			return false;
		}

		if(mul)
			*num += ((*str - 48) * mul * 10);
		else
			*num += (*str - 48);
		str++;
	}

	return true;
}

uint16_t
ainstr2code(char *instr, dict_t *l, dict_t *v)
{
	bool ret;
	uint16_t code;

	ret = is_num(instr, &code);
	if(ret)
	{
		return code;
	}

	ret = lut_lookup(instr, sym_lut, sizeof(sym_lut)/sizeof(symbol_t), &code);
	if(ret)
	{
		return code;
	}

	ret = dict_lookup(instr, l, &code);
	if(ret)
	{
		return code;
	}

	ret = dict_lookup(instr, v, &code);
	if(!ret)
	{
		symbol_t e;
		e.str = malloc(sizeof(char) * (strlen(instr) + 1));
		strcpy(e.str, instr);
		code = 16 + v->len;
		e.val = code;
		dict_insert(v, e);
		free(e.str);
	}
	
	return code;
}

// if c instruction is invalid function returns 0
/*
	function:		cinstr2code
	description:	takes a C hack instruction as an input string
					and returns its corresponding code
	return:			code - if the C-instruction is valid
					1 - if destination is invalid
					2 - if comparison is invalid
					3 - if jump is invalid
 */

uint16_t
cinstr2code(char *str)
{
	// flag: 1 -> if instuction is dest=comp
	//       1 -> if instruction is comp;jump
	//       2 -> if instuction is dest=comp;jump
	// jmp_flag: 0 -> no jump
	//			 1 -> jump 

	uint16_t code = 0, _dst, _cmp, _jmp;
	int c=0, flag=0, jmp_flag=0;
	char fields[3][4];
	bool ret;

	_dst = _cmp = _jmp = 0;

	while(*str != '\0')
	{
		if(*str == '=')
		{
			fields[flag][c] = 0;
			flag=1;
			c=0;
		}
		else if(*str == ';')
		{
			fields[flag][c] = 0;
			flag+=1;
			c=0;
			jmp_flag = 1;
		}
		else
		{
			fields[flag][c] = *str;
			c++;
		}
		str++;
	}

	fields[flag][c] = 0;

	switch(flag)
	{
		case 0:
			// Invalid command
			code = 0;
			suicide("[Error] Invalid command in line");
			break;
		case 1:
			if(jmp_flag)
			{
				// fields[0] -> cmp
				// fields[1] -> jmp
				_dst = 0;
				_cmp = 1;
				_jmp = 2;
			}
			else
			{
				// fields[0] -> dst
				// fields[1] -> cmp
				_dst = 1;
				_cmp = 2;
				_jmp = 0;
			}
			break;
		case 2:
			_dst = 1;
			_cmp = 2;
			_jmp = 3;
			break;
	}

	if(_dst)
	{
		ret = lut_lookup(fields[_dst-1], dst_lut, 
				sizeof(dst_lut)/sizeof(symbol_t), &_dst);
		if(!ret)
		{
			suicide("[Error] Invalid destination in line");
			return 2;
		}
	}

	if(_cmp)
	{
		ret = lut_lookup(fields[_cmp-1], cmp_lut,
				sizeof(cmp_lut)/sizeof(symbol_t), &_cmp);
		if(!ret)
		{
			suicide("[Error] Invalid comparison in line");
			return 3;
		}
	}

	if(_jmp)
	{
		ret = lut_lookup(fields[_jmp-1], jmp_lut,
				sizeof(jmp_lut)/sizeof(symbol_t), &_dst);
		if(!ret)
		{
			suicide("[Error] Invalid jump on line");
			return 4;
		}
	}

	// construct the machine code
	code = 0xe000; // 1110 0000 0000 0000
	code |= (_cmp << 6);
	code |= (_dst << 3);
	code |= _jmp;

	return code;
}

bool
is_ignored(char *line)
{
	return ( (line[0] == '\0') 
			|| (line[0] == '/' && line[1] == '/')  
			|| (line[0] == 13) ) ?
			true : false;
}

int
is_label(char *line, uint16_t address, dict_t *labels)
{
	int i, j, sz;
	symbol_t entry;
	bool ret = false;

	if(line[0] != '(' )
	{
		return 1;
	}

	sz = strlen(line);
	entry.str = malloc((sz - 1) * sizeof(char));
	
	if(line[sz - 1] != ')')
	{
		suicide("[Error] Missing parentheses");
	}

	for(i = 1, j = 0 ; i < sz-1 ; i++, j++)
	{
		ret = ((line[i] >= 48) && (line[i] <= 57)); //0-9
		ret |= ((line[i] >= 65) && (line[i] <= 90)); //A-Z
		ret |= ((line[i] >= 97) && (line[i] <= 122)); //a-z
		ret |= line[i] == 36; //$
		ret |= line[i] == 46; //.
		ret |= line[i] == 95; //_

		if(!ret)
		{
			printf("%s\n", line);
			suicide("[Error] Labels have to contain characters "
					"0-9, A-Z, _");
		}
		
		entry.str[j] = line[i];
	}
	
	entry.str[j] = '\0';

	ret = dict_lookup(entry.str, labels, &(entry.val));
	
	if(ret)
	{
		suicide("[Error] Duplicate labels");
	}

	entry.val = address;
	dict_insert(labels, entry);

	free(entry.str);
	return 0;
}

int
main(int argc, char *argv[])
{
	char *l, *filename;
	uint16_t code, addr = 0;
	dict_t *labels, *variables;
	int ret, lines = 0;
	FILE *ifp, *tfp, *ofp;

	if(argc != 2)
	{
		suicide("\nWrong usage\nUsage:\n\t hxcasm <filename>\n\n");
	}

	filename = argv[1];
	if((ifp = fopen(filename, "r")) == NULL)
	{
		suicide("Cannot open file");
	}
	
	if((tfp = fopen("out.s", "w+")) == NULL)
	{
		suicide("Cannot open file");
	}

	labels = init_dict();
	variables = init_dict();

	/* first pass: labels */
	while((l = fgetl(ifp)) != NULL)
	{
		trim_whitespace(l);

		if(!is_ignored(l))
		{
			remove_comments(l);

			ret = is_label(l, addr, labels);
			if(ret > 0)
			{
				// Not label
				fwrite(l, sizeof(char), strlen(l), tfp);
				fwrite("\n", sizeof(char), 1, tfp);
				addr++;
			}
			else if(ret < 0)
			{
				// Label syntax error
				return 1;
			}
		}

		free(l);
		lines++;
	}

	fseek(tfp, 0, SEEK_SET);
	fclose(ifp);


	if((ofp = fopen("out.hack", "w")) == NULL)
	{
		suicide("Cannot open file out.hack");
	}

	lines = 0;
	while((l = fgetl(tfp)) != NULL)
	{
		// handle code
		if(l[0] == '@')
		{
			code = ainstr2code(l+1, labels, variables);
		}
		else
		{
			code = cinstr2code(l);
		}

		fprint_binary16(ofp, code);
		fwrite("\n", sizeof(char), 1, ofp);
	
		free(l);
		lines++;
	}

	fclose(tfp);
	fclose(ofp);

	return 0;
}

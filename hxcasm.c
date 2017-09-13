#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "lut.h"
#include "utils.h"

void
suicide(const char *error_str)
{
	printf("%s\n", error_str);
	exit(1);
}


/*
	function:		ainstr2code
	description:	takes an A hack instruction as an input string
					and returns its corresponding code
	return:			code - if the A-instruction is valid
					0x8001 - missing closing parentheses
					0x8002 - duplicate label
					0x8003 - invalid characters in the label name
	
	(Note: C-instruction codes are greater or equal
	to 0xe000, so the aforementioned error codes are
	valid) 
 */

uint16_t
ainstr2code(char *instr, dict_t *l, dict_t *v)
{
	bool ret;
	uint16_t code;

	ret = isnum(instr, &code);
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

/*
	function:		cinstr2code
	description:	takes a C hack instruction as an input string
					and returns its corresponding code
	return:			code - if the C-instruction is valid
					1 - if invalid command
					2 - if destination is invalid
					3 - if comparison is invalid
					4 - if jump is invalid
	
	(Note: C-instruction codes are greater or equal
	to 0xe000, so the aforementioned error codes are
	valid) 
 */

uint16_t
cinstr2code(char *str)
{
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
			return 1;
			break;
		case 1:
			if(jmp_flag)
			{
				_dst = 0;
				_cmp = 1;
				_jmp = 2;
			}
			else
			{
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
			return 2;
		}
	}

	if(_cmp)
	{
		ret = lut_lookup(fields[_cmp-1], cmp_lut,
				sizeof(cmp_lut)/sizeof(symbol_t), &_cmp);
		if(!ret)
		{
			return 3;
		}
	}

	if(_jmp)
	{
		ret = lut_lookup(fields[_jmp-1], jmp_lut,
				sizeof(jmp_lut)/sizeof(symbol_t), &_dst);
		if(!ret)
		{
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

void
cerror(int err, char *filename, int line)
{
	char str[128];

	str[0] = '\0';

	switch(err)
	{
		case 1:
			sprintf(str, "%s:%d [error]: Invalid C-instruction", 
					filename, line);
			break;
		case 2:
			sprintf(str, "%s:%d [error]: Invalid destination",
					filename, line);
			break;
		case 3:
			sprintf(str, "%s:%d [error]: Invalid comparison",
					filename, line);
			break;
		case 4:
			sprintf(str, "%s:%d [error]: Invalid jump",
					filename, line);
			break;
	}

	if(str[0] != '\0')
		suicide(str);
}

void
lerror(int err, char *filename, int line)
{
	char str[128];

	str[0] = '\0';

	switch(err)
	{
		case 0x8001:
			sprintf(str, "%s:%d [error]: Missing parentheses",
					filename, line);
			break;
		case 0x8002:
			sprintf(str, "%s:%d [error]: Duplicate label",
					filename, line);
			break;
		case 0x8003:
			sprintf(str, "%s:%d [error]: Labels have to contain characters "
					"(A-Z, a-z, 0-9, _, ., $)",
					filename, line);
			break;
		default:
			break;
	}
	
	if(str[0] != '\0')
		suicide(str);
}

bool
isignored(char *line)
{
	return ( (line[0] == '\0') 
			|| (line[0] == '/' && line[1] == '/')  
			|| (line[0] == 13) ) ?
			true : false;
}

int
islabel(char *line, uint16_t address, dict_t *labels)
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
		return 0x8001;
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
			return 0x8003;
		}
		
		entry.str[j] = line[i];
	}
	
	entry.str[j] = '\0';

	ret = dict_lookup(entry.str, labels, &(entry.val));
	
	if(ret)
	{
		return 0x8002;
	}

	entry.val = address;
	dict_insert(labels, entry);

	free(entry.str);
	return 0;
}

int
main(int argc, char *argv[])
{
	char *l, *filename, err_str[128];
	uint16_t code, addr = 0;
	dict_t *labels, *variables;
	int ret, lines = 1;
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

		if(!isignored(l))
		{
			ret = remove_comments(l);
			if(ret)
			{
				sprintf(err_str, "%s:%d [error]: invalid comment syntax", 
						filename, lines);
				suicide(err_str);
			}

			ret = islabel(l, addr, labels);
			lerror(ret, filename, lines);
			
			if(!ret)
			{
				fwrite(l, sizeof(char), strlen(l), tfp);
				fwrite("\n", sizeof(char), 1, tfp);
				addr++;
			}
			
			if(l[0] != '@')
			{
				ret = cinstr2code(l);
				cerror(ret, filename, lines);
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

	lines = 1;

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

	free_dict(labels);
	free_dict(variables);

	return 0;
}

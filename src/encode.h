#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>

#include "parse.h"
#include "hashtable.h"

uint16_t encode(struct parsed_line *line);

#endif

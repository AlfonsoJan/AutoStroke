#ifndef PARSE_H_
#define PARSE_H_

#include "action.h"

#define MAX_TOKENS 256
#define MAX_ACTIONS 256

int parse_macro_text(const char *macro_path, ActionString *out_actions, size_t *out_count);

#endif // PARSE_H_
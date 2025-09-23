#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <ctype.h>

#include "utils.h"

int parse_long_strict(const char *tok, long *out) {
    if (!tok || !*tok) return 0;
    errno = 0;
    char *end = NULL;
    long v = strtol(tok, &end, 0);
    if (errno == ERANGE) return 0;
    if (end == tok) return 0;
    while (*end && isspace((unsigned char) * end)) end++;
    if (*end != '\0') return 0;
    *out = v;
    return 1;
}

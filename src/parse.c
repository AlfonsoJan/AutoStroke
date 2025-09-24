#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "parse.h"

static void rstrip(char *s) {
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ' || s[n - 1] == '\t'))
        s[--n] = '\0';
}

static char *dupstr(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = (char*)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

static void tokenize_line(char *line, char *out_tokens[], int *out_count) {
    enum { S_SKIPWS, S_TOKEN, S_QUOTED, S_COMMENT } state = S_SKIPWS;
    char *p = line;
    char *w = NULL;
    char quote = 0;
    int ntok = 0;

    for (;;) {
        char c = *p;

        switch (state) {
        case S_SKIPWS:
            // skip ASCII spaces/tabs
            while (c && (c == ' ' || c == '\t')) c = *++p;
            if (c == '\0') {
                *out_count = ntok;
                return;
            }
            // comment start
            if (c == '#') {
                *out_count = ntok;
                return;
            }
            if (c == '/' && p[1] == '/') {
                *out_count = ntok;
                return;
            }

            if (ntok >= MAX_TOKENS) {
                *out_count = ntok;
                return;
            }
            // start token
            if (c == '"' || c == '\'') {
                quote = c;
                p++;
                out_tokens[ntok++] = w = p;
                state = S_QUOTED;
            } else {
                out_tokens[ntok++] = w = p;
                state = S_TOKEN;
            }
            break;

        case S_TOKEN:
            if (c == '\0') {
                *w = '\0';
                *out_count = ntok;
                return;
            }
            if (c == ' ' || c == '\t') {
                *w = '\0';
                p++;
                state = S_SKIPWS;
                break;
            }
            if (c == '#') {
                *w = '\0';
                *out_count = ntok;
                return;
            }
            if (c == '/' && p[1] == '/') {
                *w = '\0';
                *out_count = ntok;
                return;
            }
            // normal char
            *w++ = *p++;
            break;

        case S_QUOTED:
            if (c == '\0') {
                *w = '\0';
                *out_count = ntok;
                return;
            }
            if (c == quote) {
                *w = '\0';
                p++;
                state = S_SKIPWS;
                break;
            }
            if (c == '\\') {
                p++;
                char e = *p;
                if (e == 'n')      {
                    *w++ = '\n';
                    p++;
                } else if (e == 'r') {
                    *w++ = '\r';
                    p++;
                } else if (e == 't') {
                    *w++ = '\t';
                    p++;
                } else if (e == '\\' || e == '"' || e == '\'') {
                    *w++ = e;
                    p++;
                } else if (e == 'x' || e == 'X') {
                    p++;
                    int hi = -1, lo = -1;
                    if (isxdigit((unsigned char)p[0]))
                        hi = isdigit((unsigned char)p[0]) ? p[0] - '0' : 10 + (tolower((unsigned char)p[0]) - 'a');
                    if (isxdigit((unsigned char)p[1]))
                        lo = isdigit((unsigned char)p[1]) ? p[1] - '0' : 10 + (tolower((unsigned char)p[1]) - 'a');
                    if (hi >= 0 && lo >= 0) {
                        *w++ = (char)((hi << 4) | lo);
                        p += 2;
                    }
                } else if (e != '\0') {
                    *w++ = e;
                    p++;
                }
            } else {
                *w++ = *p++;
            }
            break;

        default:
            *out_count = ntok;
            return;
        }
    }
}

int parse_macro_text(const char *macro_path, ActionString *out_actions, size_t *out_count) {
    *out_count = 0;

    FILE *fp = NULL;
    if (fopen_s(&fp, macro_path, "rb") != 0) fp = NULL;
    if (!fp) {
        fprintf(stderr, "Could not open file: %s\n", macro_path);
        return 1;
    }

    size_t cap = 4096;
    char *buf = (char*)malloc(cap);
    if (!buf) {
        fclose(fp);
        fprintf(stderr, "Out of memory\n");
        return 1;
    }

    int line_no = 0;
    while (fgets(buf, (int)cap, fp)) {
        line_no++;
        while (!strchr(buf, '\n') && !feof(fp)) {
            size_t len = strlen(buf);
            cap *= 2;
            char *nbuf = (char*)realloc(buf, cap);
            if (!nbuf) {
                free(buf);
                fclose(fp);
                fprintf(stderr, "Out of memory\n");
                return 1;
            }
            buf = nbuf;
            if (!fgets(buf + len, (int)(cap - len), fp)) break;
        }
        rstrip(buf);

        char *tokens[MAX_TOKENS];
        int count = 0;
        tokenize_line(buf, tokens, &count);
        if (count == 0) continue;

        if (*out_count >= MAX_ACTIONS) {
            fprintf(stderr, "Too many actions (max %lu)\n", (unsigned long)MAX_ACTIONS);
            free(buf);
            fclose(fp);
            return 1;
        }

        ActionType actionType = stringToActionType(tokens[0]);
        if (actionType == UNKNOWN_ACTION || actionType == TOTAL_ACTION) {
            fprintf(stderr, "Line %d: Skipping '%s': unknown command\n", line_no, tokens[0]);
            continue;
        }

        ActionString *as = &out_actions[*out_count];
        as->action = actionType;
        as->argcount = count - 1;
        as->args = NULL;
        as->line_no = line_no;
        as->filename = macro_path;

        if (as->argcount > 0) {
            as->args = (char**)calloc((size_t)as->argcount, sizeof(char*));
            if (!as->args) {
                free(buf);
                fclose(fp);
                fprintf(stderr, "Out of memory\n");
                return 1;
            }
            for (int i = 0; i < as->argcount; ++i) {
                as->args[i] = dupstr(tokens[i + 1]);
                if (!as->args[i]) {
                    for (int j = 0; j < i; ++j) {
                        free(as->args[j]);
                    }
                    free(as->args);
                    free(buf);
                    fclose(fp);
                    fprintf(stderr, "Out of memory\n");
                    return 1;
                }
            }
        }

        (*out_count)++;
    }

    free(buf);
    fclose(fp);
    return 0;
}

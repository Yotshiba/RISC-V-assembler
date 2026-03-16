#include "parser.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static void trim_trailing(char *s) {
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
}

static const char *skip_ws(const char *s) {
    while (isspace((unsigned char)*s)) s++;
    return s;
}

int parse_line(const char *line, ParsedLine *out) {
    memset(out, 0, sizeof(*out));
    if (!line) return 0;

    /* Copy and strip comment */
    char buf[MAX_LINE_LEN];
    strncpy(buf, line, MAX_LINE_LEN - 1);
    buf[MAX_LINE_LEN - 1] = '\0';

    char *comment = strchr(buf, '#');
    if (comment) *comment = '\0';
    trim_trailing(buf);

    const char *p = skip_ws(buf);
    if (*p == '\0') return 0; /* empty / comment-only line */

    /* Detect optional label (ends with ':') */
    const char *colon = strchr(p, ':');
    if (colon) {
        int llen = (int)(colon - p);
        if (llen > 0 && llen < MAX_TOKEN_LEN) {
            strncpy(out->label, p, (size_t)llen);
            out->label[llen] = '\0';
            trim_trailing(out->label);
        }
        p = skip_ws(colon + 1);
    }

    if (*p == '\0') return 0; /* label-only line */

    /* Extract mnemonic */
    int i = 0;
    while (*p && !isspace((unsigned char)*p) && *p != ',' && i < MAX_TOKEN_LEN - 1)
        out->mnemonic[i++] = *p++;
    out->mnemonic[i] = '\0';

    if (out->mnemonic[0] == '\0') return 0;

    /* Extract operands (comma- or space-separated; parentheses kept intact) */
    p = skip_ws(p);
    while (*p && out->num_operands < MAX_OPERANDS) {
        /* Skip leading commas and whitespace between operands */
        while (*p && (isspace((unsigned char)*p) || *p == ',')) p++;
        if (!*p) break;

        int depth = 0;
        i = 0;
        while (*p && i < MAX_TOKEN_LEN - 1) {
            if      (*p == '(') depth++;
            else if (*p == ')') depth--;
            /* Stop at comma or whitespace only when not inside parentheses */
            if (depth == 0 && (*p == ',' || isspace((unsigned char)*p))) break;
            out->operands[out->num_operands][i++] = *p++;
        }
        out->operands[out->num_operands][i] = '\0';
        if (i > 0) out->num_operands++;
    }

    return 0;
}

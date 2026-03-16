#include "symbols.h"
#include <string.h>
#include <stdio.h>

#define SYM_NAME_LEN 64   /* max label length */

typedef struct {
    char     name[SYM_NAME_LEN];
    uint32_t address;
} Symbol;

static Symbol sym_table[MAX_SYMBOLS];
static int    sym_count = 0;

void symbols_init(void) {
    sym_count = 0;
    memset(sym_table, 0, sizeof(sym_table));
}

int symbols_add(const char *name, uint32_t address) {
    if (!name || name[0] == '\0') return -1;

    /* Duplicate check */
    uint32_t dummy;
    if (symbols_find(name, &dummy) == 0) {
        fprintf(stderr, "Error: duplicate label '%s'\n", name);
        return -1;
    }

    if (sym_count >= MAX_SYMBOLS) {
        fprintf(stderr, "Error: symbol table full\n");
        return -1;
    }

    strncpy(sym_table[sym_count].name, name, SYM_NAME_LEN - 1);
    sym_table[sym_count].name[SYM_NAME_LEN - 1] = '\0';
    sym_table[sym_count].address = address;
    sym_count++;
    return 0;
}

int symbols_find(const char *name, uint32_t *address) {
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(sym_table[i].name, name) == 0) {
            if (address) *address = sym_table[i].address;
            return 0;
        }
    }
    return -1;
}

int symbols_count(void) {
    return sym_count;
}

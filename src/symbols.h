#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdint.h>

/*
 * symbols.h - Label symbol table (array-based)
 */

#define MAX_SYMBOLS 256

/* Initialise / clear the symbol table */
void symbols_init(void);

/*
 * Add a label at the given address.
 * Returns  0 on success.
 * Returns -1 if the label already exists or the table is full.
 */
int symbols_add(const char *name, uint32_t address);

/*
 * Look up a label.
 * Returns  0 and writes *address if found.
 * Returns -1 if not found (*address unchanged).
 */
int symbols_find(const char *name, uint32_t *address);

/* Number of symbols currently stored */
int symbols_count(void);

#endif /* SYMBOLS_H */

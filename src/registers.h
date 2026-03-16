#ifndef REGISTERS_H
#define REGISTERS_H

/*
 * registers.h - Register name to number conversion
 * Supports x0-x31 and ABI names (zero, ra, sp, ..., t6)
 */

/* Returns register number 0-31, or -1 for invalid name */
int parse_register(const char *name);

#endif /* REGISTERS_H */

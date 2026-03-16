#ifndef INSTR_TABLE_H
#define INSTR_TABLE_H

#include <stdint.h>

/*
 * instr_table.h - Instruction specification table
 * Data taken directly from the CS61C RISC-V reference card.
 */

typedef enum {
    FMT_R,   /* rd, rs1, rs2              */
    FMT_I,   /* rd, rs1, imm12            */
    FMT_S,   /* rs2, imm12(rs1)           */
    FMT_B,   /* rs1, rs2, label/imm13     */
    FMT_U,   /* rd, imm20                 */
    FMT_J    /* rd, label/imm21           */
} InstrFormat;

typedef struct {
    const char  *mnemonic;
    InstrFormat  format;
    uint32_t     opcode;
    uint32_t     funct3;
    uint32_t     funct7;   /* 0 where not applicable */
} InstrSpec;

/* Returns pointer to matching entry, or NULL if not found */
const InstrSpec *lookup_instr(const char *mnemonic);

#endif /* INSTR_TABLE_H */

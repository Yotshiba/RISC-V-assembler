#include "instr_table.h"
#include <string.h>
#include <stddef.h>

/*
 * Complete instruction table for the required RV32I subset.
 * Opcode / funct3 / funct7 values from the CS61C reference card.
 */
static const InstrSpec table[] = {
    /* ── R-type (opcode 0x33) ─────────────────────────────────── */
    { "add",   FMT_R, 0x33, 0x0, 0x00 },
    { "sub",   FMT_R, 0x33, 0x0, 0x20 },
    { "sll",   FMT_R, 0x33, 0x1, 0x00 },
    { "slt",   FMT_R, 0x33, 0x2, 0x00 },
    { "sltu",  FMT_R, 0x33, 0x3, 0x00 },
    { "xor",   FMT_R, 0x33, 0x4, 0x00 },
    { "srl",   FMT_R, 0x33, 0x5, 0x00 },
    { "sra",   FMT_R, 0x33, 0x5, 0x20 },
    { "or",    FMT_R, 0x33, 0x6, 0x00 },
    { "and",   FMT_R, 0x33, 0x7, 0x00 },

    /* ── I-type arithmetic (opcode 0x13) ──────────────────────── */
    { "addi",  FMT_I, 0x13, 0x0, 0x00 },
    { "slti",  FMT_I, 0x13, 0x2, 0x00 },
    { "sltiu", FMT_I, 0x13, 0x3, 0x00 },
    { "xori",  FMT_I, 0x13, 0x4, 0x00 },
    { "ori",   FMT_I, 0x13, 0x6, 0x00 },
    { "andi",  FMT_I, 0x13, 0x7, 0x00 },

    /* ── I-type shifts (funct7 packed into upper imm bits) ──────  */
    { "slli",  FMT_I, 0x13, 0x1, 0x00 },
    { "srli",  FMT_I, 0x13, 0x5, 0x00 },
    { "srai",  FMT_I, 0x13, 0x5, 0x20 },

    /* ── I-type loads (opcode 0x03) ───────────────────────────── */
    { "lb",    FMT_I, 0x03, 0x0, 0x00 },
    { "lh",    FMT_I, 0x03, 0x1, 0x00 },
    { "lw",    FMT_I, 0x03, 0x2, 0x00 },
    { "lbu",   FMT_I, 0x03, 0x4, 0x00 },
    { "lhu",   FMT_I, 0x03, 0x5, 0x00 },

    /* ── S-type stores (opcode 0x23) ──────────────────────────── */
    { "sb",    FMT_S, 0x23, 0x0, 0x00 },
    { "sh",    FMT_S, 0x23, 0x1, 0x00 },
    { "sw",    FMT_S, 0x23, 0x2, 0x00 },

    /* ── B-type branches (opcode 0x63) ────────────────────────── */
    { "beq",   FMT_B, 0x63, 0x0, 0x00 },
    { "bne",   FMT_B, 0x63, 0x1, 0x00 },
    { "blt",   FMT_B, 0x63, 0x4, 0x00 },
    { "bge",   FMT_B, 0x63, 0x5, 0x00 },
    { "bltu",  FMT_B, 0x63, 0x6, 0x00 },
    { "bgeu",  FMT_B, 0x63, 0x7, 0x00 },

    /* ── J-type (opcode 0x6F) ─────────────────────────────────── */
    { "jal",   FMT_J, 0x6F, 0x0, 0x00 },

    /* ── jalr: I-type (opcode 0x67) ──────────────────────────── */
    { "jalr",  FMT_I, 0x67, 0x0, 0x00 },

    /* ── U-type extras (bonus; not required by spec) ─────────── */
    { "lui",   FMT_U, 0x37, 0x0, 0x00 },
    { "auipc", FMT_U, 0x17, 0x0, 0x00 },
};

#define TABLE_SIZE ((int)(sizeof(table) / sizeof(table[0])))

const InstrSpec *lookup_instr(const char *mnemonic) {
    if (!mnemonic) return NULL;
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(table[i].mnemonic, mnemonic) == 0)
            return &table[i];
    }
    return NULL;
}

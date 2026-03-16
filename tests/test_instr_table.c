#include <stdio.h>
#include "instr_table.h"

static int pass = 0, fail = 0;

#define ASSERT_NOT_NULL(ptr, msg) do { \
    if ((ptr) != NULL) { pass++; printf("PASS: %s\n", msg); } \
    else { fail++; printf("FAIL: %s  (got NULL)\n", msg); } \
} while(0)

#define ASSERT_NULL(ptr, msg) do { \
    if ((ptr) == NULL) { pass++; printf("PASS: %s\n", msg); } \
    else { fail++; printf("FAIL: %s  (expected NULL)\n", msg); } \
} while(0)

#define ASSERT_EQ(got, expected, msg) do { \
    if ((uint32_t)(got) == (uint32_t)(expected)) { pass++; printf("PASS: %s\n", msg); } \
    else { fail++; printf("FAIL: %s  (got 0x%X, expected 0x%X)\n", msg, (unsigned)(got), (unsigned)(expected)); } \
} while(0)

int main(void) {
    printf("=== test_instr_table ===\n");

    const InstrSpec *s;

    /* R-type instructions */
    s = lookup_instr("add");
    ASSERT_NOT_NULL(s,           "add found");
    ASSERT_EQ(s->format, FMT_R,  "add format");
    ASSERT_EQ(s->opcode, 0x33,   "add opcode");
    ASSERT_EQ(s->funct3, 0x0,    "add funct3");
    ASSERT_EQ(s->funct7, 0x00,   "add funct7");

    s = lookup_instr("sub");
    ASSERT_NOT_NULL(s,           "sub found");
    ASSERT_EQ(s->funct7, 0x20,   "sub funct7");

    s = lookup_instr("sra");
    ASSERT_NOT_NULL(s,           "sra found");
    ASSERT_EQ(s->funct3, 0x5,    "sra funct3");
    ASSERT_EQ(s->funct7, 0x20,   "sra funct7");

    /* I-type arithmetic */
    s = lookup_instr("addi");
    ASSERT_NOT_NULL(s,           "addi found");
    ASSERT_EQ(s->format, FMT_I,  "addi format");
    ASSERT_EQ(s->opcode, 0x13,   "addi opcode");

    s = lookup_instr("slli");
    ASSERT_NOT_NULL(s,           "slli found");
    ASSERT_EQ(s->funct3, 0x1,    "slli funct3");
    ASSERT_EQ(s->funct7, 0x00,   "slli funct7 (no arithmetic bit)");

    s = lookup_instr("srai");
    ASSERT_NOT_NULL(s,           "srai found");
    ASSERT_EQ(s->funct7, 0x20,   "srai funct7");

    /* Loads */
    s = lookup_instr("lw");
    ASSERT_NOT_NULL(s,           "lw found");
    ASSERT_EQ(s->opcode, 0x03,   "lw opcode");
    ASSERT_EQ(s->funct3, 0x2,    "lw funct3");

    s = lookup_instr("lbu");
    ASSERT_NOT_NULL(s,           "lbu found");
    ASSERT_EQ(s->funct3, 0x4,    "lbu funct3");

    /* Stores */
    s = lookup_instr("sw");
    ASSERT_NOT_NULL(s,           "sw found");
    ASSERT_EQ(s->format, FMT_S,  "sw format");
    ASSERT_EQ(s->opcode, 0x23,   "sw opcode");

    /* Branches */
    s = lookup_instr("beq");
    ASSERT_NOT_NULL(s,           "beq found");
    ASSERT_EQ(s->format, FMT_B,  "beq format");
    ASSERT_EQ(s->opcode, 0x63,   "beq opcode");
    ASSERT_EQ(s->funct3, 0x0,    "beq funct3");

    s = lookup_instr("blt");
    ASSERT_NOT_NULL(s,           "blt found");
    ASSERT_EQ(s->funct3, 0x4,    "blt funct3");

    s = lookup_instr("bgeu");
    ASSERT_NOT_NULL(s,           "bgeu found");
    ASSERT_EQ(s->funct3, 0x7,    "bgeu funct3");

    /* jal / jalr */
    s = lookup_instr("jal");
    ASSERT_NOT_NULL(s,           "jal found");
    ASSERT_EQ(s->format, FMT_J,  "jal format");
    ASSERT_EQ(s->opcode, 0x6F,   "jal opcode");

    s = lookup_instr("jalr");
    ASSERT_NOT_NULL(s,           "jalr found");
    ASSERT_EQ(s->format, FMT_I,  "jalr format");
    ASSERT_EQ(s->opcode, 0x67,   "jalr opcode");

    /* Unknown */
    ASSERT_NULL(lookup_instr("nop"),   "nop not in table");
    ASSERT_NULL(lookup_instr(""),      "empty mnemonic");
    ASSERT_NULL(lookup_instr(NULL),    "NULL mnemonic");

    printf("\nResults: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}

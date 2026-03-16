#include <stdio.h>
#include <string.h>
#include "parser.h"

static int pass = 0, fail = 0;

#define ASSERT_STR(got, expected, msg) do { \
    if (strcmp((got), (expected)) == 0) { pass++; printf("PASS: %s\n", msg); } \
    else { fail++; printf("FAIL: %s  (got \"%s\", expected \"%s\")\n", msg, got, expected); } \
} while(0)

#define ASSERT_INT(got, expected, msg) do { \
    if ((got) == (expected)) { pass++; printf("PASS: %s\n", msg); } \
    else { fail++; printf("FAIL: %s  (got %d, expected %d)\n", msg, (int)(got), (int)(expected)); } \
} while(0)

int main(void) {
    printf("=== test_parser ===\n");

    ParsedLine pl;

    /* Basic R-type */
    parse_line("add x1, x2, x3", &pl);
    ASSERT_STR(pl.mnemonic,    "add",   "R-type mnemonic");
    ASSERT_STR(pl.operands[0], "x1",    "R-type rd");
    ASSERT_STR(pl.operands[1], "x2",    "R-type rs1");
    ASSERT_STR(pl.operands[2], "x3",    "R-type rs2");
    ASSERT_INT(pl.num_operands, 3,      "R-type operand count");

    /* I-type with immediate */
    parse_line("addi x5, x0, 10", &pl);
    ASSERT_STR(pl.mnemonic,    "addi",  "I-type mnemonic");
    ASSERT_STR(pl.operands[0], "x5",    "I-type rd");
    ASSERT_STR(pl.operands[2], "10",    "I-type imm");

    /* Memory operand stays intact */
    parse_line("lw x14, 4(x13)", &pl);
    ASSERT_STR(pl.mnemonic,    "lw",    "load mnemonic");
    ASSERT_STR(pl.operands[0], "x14",   "load rd");
    ASSERT_STR(pl.operands[1], "4(x13)","load mem operand");
    ASSERT_INT(pl.num_operands, 2,      "load operand count");

    /* Store */
    parse_line("sw x1, 8(x2)", &pl);
    ASSERT_STR(pl.mnemonic,    "sw",    "store mnemonic");
    ASSERT_STR(pl.operands[0], "x1",    "store rs2");
    ASSERT_STR(pl.operands[1], "8(x2)", "store mem operand");

    /* Label-only line */
    parse_line("loop:", &pl);
    ASSERT_STR(pl.label,    "loop", "label-only label");
    ASSERT_INT(pl.mnemonic[0], 0,   "label-only has no mnemonic");

    /* Label + instruction on same line */
    parse_line("start: addi x1, x0, 5", &pl);
    ASSERT_STR(pl.label,       "start", "inline label");
    ASSERT_STR(pl.mnemonic,    "addi",  "inline mnemonic");

    /* Comment stripping */
    parse_line("add x1, x2, x3  # sum", &pl);
    ASSERT_STR(pl.mnemonic, "add", "comment stripped mnemonic");
    ASSERT_INT(pl.num_operands, 3, "comment stripped operands");

    /* Empty / comment-only lines */
    parse_line("# just a comment", &pl);
    ASSERT_INT(pl.mnemonic[0], 0, "comment-only mnemonic empty");

    parse_line("   ", &pl);
    ASSERT_INT(pl.mnemonic[0], 0, "whitespace-only mnemonic empty");

    /* Branch with label */
    parse_line("beq x1, x2, loop", &pl);
    ASSERT_STR(pl.mnemonic,    "beq",  "branch mnemonic");
    ASSERT_STR(pl.operands[2], "loop", "branch target");

    /* Leading/trailing whitespace */
    parse_line("    sub  x3, x1, x2    ", &pl);
    ASSERT_STR(pl.mnemonic,    "sub",  "indented mnemonic");
    ASSERT_INT(pl.num_operands, 3,     "indented operand count");

    printf("\nResults: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}

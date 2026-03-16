#include <stdio.h>
#include "registers.h"

static int pass = 0, fail = 0;

#define ASSERT_EQ(got, expected, msg) do { \
    if ((got) == (expected)) { pass++; printf("PASS: %s\n", msg); } \
    else { fail++; printf("FAIL: %s  (got %d, expected %d)\n", msg, (int)(got), (int)(expected)); } \
} while(0)

int main(void) {
    printf("=== test_registers ===\n");

    /* x-notation */
    ASSERT_EQ(parse_register("x0"),  0,  "x0");
    ASSERT_EQ(parse_register("x1"),  1,  "x1");
    ASSERT_EQ(parse_register("x15"), 15, "x15");
    ASSERT_EQ(parse_register("x31"), 31, "x31");

    /* ABI names */
    ASSERT_EQ(parse_register("zero"), 0,  "zero");
    ASSERT_EQ(parse_register("ra"),   1,  "ra");
    ASSERT_EQ(parse_register("sp"),   2,  "sp");
    ASSERT_EQ(parse_register("gp"),   3,  "gp");
    ASSERT_EQ(parse_register("tp"),   4,  "tp");
    ASSERT_EQ(parse_register("t0"),   5,  "t0");
    ASSERT_EQ(parse_register("t1"),   6,  "t1");
    ASSERT_EQ(parse_register("t2"),   7,  "t2");
    ASSERT_EQ(parse_register("s0"),   8,  "s0");
    ASSERT_EQ(parse_register("fp"),   8,  "fp == s0");
    ASSERT_EQ(parse_register("s1"),   9,  "s1");
    ASSERT_EQ(parse_register("a0"),   10, "a0");
    ASSERT_EQ(parse_register("a7"),   17, "a7");
    ASSERT_EQ(parse_register("t6"),   31, "t6");

    /* Invalid inputs */
    ASSERT_EQ(parse_register("x32"),   -1, "x32 invalid");
    ASSERT_EQ(parse_register("x-1"),   -1, "x-1 invalid");
    ASSERT_EQ(parse_register(""),      -1, "empty string");
    ASSERT_EQ(parse_register("hello"), -1, "unknown name");
    ASSERT_EQ(parse_register(NULL),    -1, "NULL");

    printf("\nResults: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}

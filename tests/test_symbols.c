#include <stdio.h>
#include "symbols.h"

static int pass = 0, fail = 0;

#define ASSERT_EQ(got, expected, msg) do { \
    if ((got) == (expected)) { pass++; printf("PASS: %s\n", msg); } \
    else { fail++; printf("FAIL: %s  (got %d, expected %d)\n", msg, (int)(got), (int)(expected)); } \
} while(0)

int main(void) {
    printf("=== test_symbols ===\n");

    uint32_t addr;

    symbols_init();
    ASSERT_EQ(symbols_count(), 0, "initial count is 0");

    /* Add labels */
    ASSERT_EQ(symbols_add("start", 0),   0, "add 'start'");
    ASSERT_EQ(symbols_add("loop",  12),  0, "add 'loop'");
    ASSERT_EQ(symbols_add("done",  24),  0, "add 'done'");
    ASSERT_EQ(symbols_count(), 3,           "count after 3 adds");

    /* Find labels */
    ASSERT_EQ(symbols_find("start", &addr), 0,  "find 'start' returns 0");
    ASSERT_EQ((int)addr,  0,                     "'start' address == 0");

    ASSERT_EQ(symbols_find("loop", &addr),  0,  "find 'loop' returns 0");
    ASSERT_EQ((int)addr,  12,                    "'loop' address == 12");

    ASSERT_EQ(symbols_find("done", &addr),  0,  "find 'done' returns 0");
    ASSERT_EQ((int)addr,  24,                    "'done' address == 24");

    /* Label not found */
    ASSERT_EQ(symbols_find("missing", &addr), -1, "find missing returns -1");

    /* Duplicate detection */
    ASSERT_EQ(symbols_add("loop", 99), -1, "duplicate label rejected");
    ASSERT_EQ(symbols_count(), 3,          "count unchanged after duplicate");

    /* Address is NULL-safe */
    ASSERT_EQ(symbols_find("start", NULL), 0, "find with NULL addr pointer");

    /* Re-initialise */
    symbols_init();
    ASSERT_EQ(symbols_count(), 0, "count reset after init");
    ASSERT_EQ(symbols_find("start", &addr), -1, "cleared after init");

    printf("\nResults: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}

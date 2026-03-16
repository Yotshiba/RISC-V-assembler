#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

static int pass = 0, fail = 0;

#define ASSERT_EQ(got, expected, msg) do { \
    if ((got) == (expected)) { pass++; printf("PASS: %s\n", msg); } \
    else { fail++; printf("FAIL: %s  (got %d, expected %d)\n", msg, (int)(got), (int)(expected)); } \
} while(0)

/* Write a small assembly snippet to a temp file and assemble it.
   Returns 0 on successful assembly, -1 on error. */
static int assemble_string(const char *asm_src, char *out_buf, size_t buf_sz) {
    /* Write to temp input file */
    FILE *f = fopen("_test_input.s", "w");
    if (!f) return -1;
    fputs(asm_src, f);
    fclose(f);

    /* Assemble */
    int rc = assemble_file("_test_input.s", "_test_output.hex");
    if (rc != 0) {
        remove("_test_input.s");
        return -1;
    }

    /* Read output */
    if (out_buf && buf_sz > 0) {
        f = fopen("_test_output.hex", "r");
        if (f) {
            size_t n = fread(out_buf, 1, buf_sz - 1, f);
            out_buf[n] = '\0';
            fclose(f);
        }
    }

    remove("_test_input.s");
    remove("_test_output.hex");
    return 0;
}

/* Extract the Nth instruction hex from output (0-based) */
static unsigned long nth_instr(const char *output, int n) {
    const char *p = output;
    for (int i = 0; i <= n; i++) {
        /* Each line: "0xXXXXXXXX: 0xYYYYYYYY\n" */
        const char *colon2 = strstr(p, ": 0x");
        if (!colon2) return 0xFFFFFFFF;
        unsigned long val = strtoul(colon2 + 4, NULL, 16);
        if (i == n) return val;
        p = strchr(colon2, '\n');
        if (!p) return 0xFFFFFFFF;
        p++;
    }
    return 0xFFFFFFFF;
}

int main(void) {
    printf("=== test_assembler ===\n");

    char buf[4096];
    int rc;

    /* ── addi x1, x0, 5  →  0x00500093 ──────────────────────── */
    rc = assemble_string("addi x1, x0, 5\n", buf, sizeof(buf));
    ASSERT_EQ(rc, 0, "addi assembles ok");
    ASSERT_EQ((int)nth_instr(buf, 0), (int)0x00500093, "addi encoding");

    /* ── add x3, x1, x2  →  0x002081B3 ──────────────────────── */
    rc = assemble_string("add x3, x1, x2\n", buf, sizeof(buf));
    ASSERT_EQ(rc, 0, "add assembles ok");
    ASSERT_EQ((int)nth_instr(buf, 0), (int)0x002081B3, "add encoding");

    /* ── lw x14, 4(x13)  →  0x0046A703 ──────────────────────── */
    rc = assemble_string("lw x14, 4(x13)\n", buf, sizeof(buf));
    ASSERT_EQ(rc, 0, "lw assembles ok");
    ASSERT_EQ((int)nth_instr(buf, 0), (int)0x0046A703, "lw encoding");

    /* ── sw x5, 0(x10)   →  0x00552023 ──────────────────────── */
    rc = assemble_string("sw x5, 0(x10)\n", buf, sizeof(buf));
    ASSERT_EQ(rc, 0, "sw assembles ok");
    ASSERT_EQ((int)nth_instr(buf, 0), (int)0x00552023, "sw encoding");

    /* ── Forward label resolution ─────────────────────────────
       addi x1, x0, 0   (pc=0)
       beq  x1, x0, done (pc=4, target=8 → offset=+4)
       done:
       addi x0, x0, 0   (pc=8, nop-ish)
    */
    rc = assemble_string(
        "addi x1, x0, 0\n"
        "beq  x1, x0, done\n"
        "done:\n"
        "addi x0, x0, 0\n",
        buf, sizeof(buf));
    ASSERT_EQ(rc, 0, "forward label assembles ok");
    /* beq x1, x0, +4  →  encode_b(0x63,0,rs1=1,rs2=0,imm=4)
       imm_4_1=2, rs1=1, rs2=0, funct3=0 → 0x00008263  */
    ASSERT_EQ((int)nth_instr(buf, 1), (int)0x00008263, "beq forward offset");

    /* ── Backward label (loop) ───────────────────────────────── */
    rc = assemble_string(
        "addi x1, x0, 5\n"   /* pc=0 */
        "loop:\n"
        "addi x1, x1, -1\n"  /* pc=4 */
        "bne  x1, x0, loop\n",/* pc=8, target=4, offset=-4 */
        buf, sizeof(buf));
    ASSERT_EQ(rc, 0, "backward label assembles ok");
    /* bne x1, x0, -4 → 0xFE009EE3 */
    ASSERT_EQ((int)nth_instr(buf, 2), (int)0xFE009EE3, "bne backward offset");

    /* ── jal x0, 0 → 0x0000006F ─────────────────────────────── */
    rc = assemble_string("jal x0, done\ndone:\n", buf, sizeof(buf));
    ASSERT_EQ(rc, 0, "jal forward assembles ok");
    ASSERT_EQ((int)nth_instr(buf, 0), (int)0x0040006F, "jal forward encoding");

    /* ── Unknown instruction should fail ─────────────────────── */
    rc = assemble_string("foobar x1, x2\n", buf, sizeof(buf));
    ASSERT_EQ(rc, -1, "unknown instruction fails");

    printf("\nResults: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}

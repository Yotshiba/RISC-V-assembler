#include <stdio.h>
#include <stdint.h>
#include "encode.h"

static int pass = 0, fail = 0;

#define ASSERT_HEX(got, expected, msg) do { \
    if ((uint32_t)(got) == (uint32_t)(expected)) { \
        pass++; printf("PASS: %s\n", msg); \
    } else { \
        fail++; printf("FAIL: %s  (got 0x%08X, expected 0x%08X)\n", \
                       msg, (unsigned)(got), (unsigned)(expected)); \
    } \
} while(0)

#define ASSERT_EQ(got, expected, msg) do { \
    if ((got) == (expected)) { pass++; printf("PASS: %s\n", msg); } \
    else { fail++; printf("FAIL: %s  (got %d, expected %d)\n", msg, (int)(got), (int)(expected)); } \
} while(0)

int main(void) {
    printf("=== test_encode ===\n");

    /* ── R-type ────────────────────────────────────────────────── */
    /* add x3, x1, x2  →  0x002081B3 */
    ASSERT_HEX(encode_r(0x33, 0x0, 0x00, 3, 1, 2), 0x002081B3, "add x3,x1,x2");

    /* sub x4, x1, x2  →  0x40208233 */
    ASSERT_HEX(encode_r(0x33, 0x0, 0x20, 4, 1, 2), 0x40208233, "sub x4,x1,x2");

    /* and x5, x6, x7  →  0x007372B3  (rs2=7,rs1=6,f3=7,rd=5) */
    ASSERT_HEX(encode_r(0x33, 0x7, 0x00, 5, 6, 7), 0x007372B3, "and x5,x6,x7");

    /* sra x1, x2, x3  →  0x403150B3  (f7=0x20,rs2=3,rs1=2,f3=5,rd=1) */
    ASSERT_HEX(encode_r(0x33, 0x5, 0x20, 1, 2, 3), 0x403150B3, "sra x1,x2,x3");

    /* ── I-type ────────────────────────────────────────────────── */
    /* addi x1, x0, 5  →  0x00500093 */
    ASSERT_HEX(encode_i(0x13, 0x0, 1, 0, 5),   0x00500093, "addi x1,x0,5");

    /* addi x2, x0, 10 →  0x00A00113 */
    ASSERT_HEX(encode_i(0x13, 0x0, 2, 0, 10),  0x00A00113, "addi x2,x0,10");

    /* addi x1, x1, -1  (sign-extended) */
    ASSERT_HEX(encode_i(0x13, 0x0, 1, 1, -1),  0xFFF08093, "addi x1,x1,-1");

    /* lw x14, 4(x13)  →  0x0046A703  (rd=14, rs1=13, f3=2, imm=4) */
    ASSERT_HEX(encode_i(0x03, 0x2, 14, 13, 4), 0x0046A703, "lw x14,4(x13)");

    /* slli x1, x1, 3  →  imm = (0x00<<5)|3 = 3  →  0x00309093 */
    ASSERT_HEX(encode_i(0x13, 0x1, 1, 1, 3),   0x00309093, "slli x1,x1,3");

    /* srai x1, x1, 3  →  imm = (0x20<<5)|3 = 0x403  →  0x4030D093 */
    ASSERT_HEX(encode_i(0x13, 0x5, 1, 1, 0x403), 0x4030D093, "srai x1,x1,3");

    /* ── S-type ────────────────────────────────────────────────── */
    /* sw x5, 0(x10)  →  0x00552023  (rs2=5, rs1=10, imm=0) */
    ASSERT_HEX(encode_s(0x23, 0x2, 10, 5, 0),  0x00552023, "sw x5,0(x10)");

    /* sw x6, 4(x10)  →  0x00652223  (rs2=6, rs1=10, imm=4) */
    ASSERT_HEX(encode_s(0x23, 0x2, 10, 6, 4),  0x00652223, "sw x6,4(x10)");

    /* sb x1, 8(x2)   →  0x00110423  (rs2=1, rs1=2, imm=8) */
    ASSERT_HEX(encode_s(0x23, 0x0, 2, 1, 8),   0x00110423, "sb x1,8(x2)");

    /* ── B-type ────────────────────────────────────────────────── */
    /* beq x1, x2, +8  →  0x00208463 */
    ASSERT_HEX(encode_b(0x63, 0x0, 1, 2, 8),   0x00208463, "beq x1,x2,+8");

    /* bne x1, x2, -4  (signed offset) */
    /* imm=-4: 0x1FFC, imm_12=1,imm_11=1,imm_10_5=0b111111=0x3F,imm_4_1=0b1110=0xE
       = (1<<31)|(0x3F<<25)|(2<<20)|(1<<15)|(1<<12)|(0xE<<8)|(1<<7)|0x63
       = 0x80000000|0x07E00000|0x00200000|0x00008000|0x00001000|0x00000E00|0x00000080|0x63
       = 0xFE209EE3 */
    ASSERT_HEX(encode_b(0x63, 0x1, 1, 2, -4),  0xFE209EE3, "bne x1,x2,-4");

    /* ── J-type ────────────────────────────────────────────────── */
    /* jal x0, 0  →  0x0000006F */
    ASSERT_HEX(encode_j(0x6F, 0, 0),           0x0000006F, "jal x0,0");

    /* jal x1, 4  →  0x004000EF */
    ASSERT_HEX(encode_j(0x6F, 1, 4),           0x004000EF, "jal x1,+4");

    /* ── U-type ────────────────────────────────────────────────── */
    /* lui x1, 1  →  0x000010B7 */
    ASSERT_HEX(encode_u(0x37, 1, 1),           0x000010B7, "lui x1,1");

    /* ── parse_mem_operand ────────────────────────────────────── */
    int reg; int32_t offset;

    ASSERT_EQ(parse_mem_operand("4(x13)", &reg, &offset),  0,  "parse_mem ok");
    ASSERT_EQ(reg,    13,  "parse_mem reg");
    ASSERT_EQ(offset,  4,  "parse_mem offset");

    ASSERT_EQ(parse_mem_operand("0(x0)", &reg, &offset),   0,  "parse_mem zero");
    ASSERT_EQ(reg,    0,   "zero reg");
    ASSERT_EQ(offset, 0,   "zero offset");

    ASSERT_EQ(parse_mem_operand("-8(x2)", &reg, &offset),  0,  "parse_mem negative");
    ASSERT_EQ(offset, -8,  "negative offset");

    /* ── parse_imm ───────────────────────────────────────────── */
    int ok;
    ASSERT_EQ(parse_imm("10",   &ok), 10,   "decimal imm");
    ASSERT_EQ(ok, 1, "decimal ok");
    ASSERT_EQ(parse_imm("0xFF", &ok), 0xFF, "hex imm");
    ASSERT_EQ(parse_imm("-5",   &ok), -5,   "negative imm");

    printf("\nResults: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}

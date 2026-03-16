#include "encode.h"
#include "registers.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

/* ── R-type ──────────────────────────────────────────────────────── */
uint32_t encode_r(uint32_t opcode, uint32_t funct3, uint32_t funct7,
                  uint32_t rd, uint32_t rs1, uint32_t rs2) {
    return (funct7 << 25)
         | (rs2    << 20)
         | (rs1    << 15)
         | (funct3 << 12)
         | (rd     <<  7)
         |  opcode;
}

/* ── I-type ──────────────────────────────────────────────────────── */
uint32_t encode_i(uint32_t opcode, uint32_t funct3,
                  uint32_t rd, uint32_t rs1, int32_t imm) {
    uint32_t imm12 = (uint32_t)(imm & 0xFFF);
    return (imm12  << 20)
         | (rs1    << 15)
         | (funct3 << 12)
         | (rd     <<  7)
         |  opcode;
}

/* ── S-type ──────────────────────────────────────────────────────── */
uint32_t encode_s(uint32_t opcode, uint32_t funct3,
                  uint32_t rs1, uint32_t rs2, int32_t imm) {
    uint32_t imm12    = (uint32_t)(imm & 0xFFF);
    uint32_t imm_4_0  =  imm12        & 0x01F;
    uint32_t imm_11_5 = (imm12 >>  5) & 0x07F;
    return (imm_11_5 << 25)
         | (rs2      << 20)
         | (rs1      << 15)
         | (funct3   << 12)
         | (imm_4_0  <<  7)
         |  opcode;
}

/* ── B-type ──────────────────────────────────────────────────────── */
/*
 * B-type immediate layout (13 bits, bit 0 always 0):
 *   inst[31]    = imm[12]
 *   inst[30:25] = imm[10:5]
 *   inst[11:8]  = imm[4:1]
 *   inst[7]     = imm[11]
 */
uint32_t encode_b(uint32_t opcode, uint32_t funct3,
                  uint32_t rs1, uint32_t rs2, int32_t imm) {
    uint32_t u       = (uint32_t)(imm & 0x1FFF);
    uint32_t imm_12   = (u >> 12) & 0x1;
    uint32_t imm_11   = (u >> 11) & 0x1;
    uint32_t imm_10_5 = (u >>  5) & 0x3F;
    uint32_t imm_4_1  = (u >>  1) & 0x0F;
    return (imm_12   << 31)
         | (imm_10_5 << 25)
         | (rs2      << 20)
         | (rs1      << 15)
         | (funct3   << 12)
         | (imm_4_1  <<  8)
         | (imm_11   <<  7)
         |  opcode;
}

/* ── U-type ──────────────────────────────────────────────────────── */
/* imm is the 20-bit field value (upper 20 bits of the result) */
uint32_t encode_u(uint32_t opcode, uint32_t rd, int32_t imm) {
    return ((uint32_t)(imm & 0xFFFFF) << 12)
         | (rd << 7)
         |  opcode;
}

/* ── J-type ──────────────────────────────────────────────────────── */
/*
 * J-type immediate layout (21 bits, bit 0 always 0):
 *   inst[31]    = imm[20]
 *   inst[30:21] = imm[10:1]
 *   inst[20]    = imm[11]
 *   inst[19:12] = imm[19:12]
 */
uint32_t encode_j(uint32_t opcode, uint32_t rd, int32_t imm) {
    uint32_t u         = (uint32_t)(imm & 0x1FFFFF);
    uint32_t imm_20    = (u >> 20) & 0x001;
    uint32_t imm_10_1  = (u >>  1) & 0x3FF;
    uint32_t imm_11    = (u >> 11) & 0x001;
    uint32_t imm_19_12 = (u >> 12) & 0x0FF;
    return (imm_20    << 31)
         | (imm_10_1  << 21)
         | (imm_11    << 20)
         | (imm_19_12 << 12)
         | (rd        <<  7)
         |  opcode;
}

/* ── Helpers ─────────────────────────────────────────────────────── */

int32_t parse_imm(const char *s, int *ok) {
    if (!s || !*s) { *ok = 0; return 0; }
    /* skip whitespace */
    while (isspace((unsigned char)*s)) s++;
    char *end;
    long val = strtol(s, &end, 0); /* base-0 handles 0x prefix */
    while (isspace((unsigned char)*end)) end++;
    *ok = (*end == '\0');
    return (int32_t)val;
}

int parse_mem_operand(const char *operand, int *reg, int32_t *offset) {
    char buf[64];
    strncpy(buf, operand, 63);
    buf[63] = '\0';

    char *paren = strchr(buf, '(');
    if (paren) {
        *paren = '\0';
        char *close = strchr(paren + 1, ')');
        if (close) *close = '\0';

        /* offset part (may be empty → 0) */
        if (buf[0] == '\0') {
            *offset = 0;
        } else {
            int ok = 0;
            *offset = parse_imm(buf, &ok);
            if (!ok) return -1;
        }

        /* register part */
        *reg = parse_register(paren + 1);
        if (*reg < 0) return -1;
    } else {
        /* bare register, no offset */
        *offset = 0;
        *reg = parse_register(buf);
        if (*reg < 0) return -1;
    }
    return 0;
}

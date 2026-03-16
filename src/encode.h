#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>

/*
 * encode.h - RISC-V instruction bit-field packing
 * Bit layouts taken from the CS61C reference card.
 *
 *  R:  [31:25]=funct7  [24:20]=rs2  [19:15]=rs1  [14:12]=funct3  [11:7]=rd   [6:0]=opcode
 *  I:  [31:20]=imm12   [19:15]=rs1  [14:12]=funct3  [11:7]=rd   [6:0]=opcode
 *  S:  [31:25]=imm[11:5]  [24:20]=rs2  [19:15]=rs1  [14:12]=f3  [11:7]=imm[4:0]  [6:0]=op
 *  B:  [31]=imm[12]  [30:25]=imm[10:5]  [24:20]=rs2  [19:15]=rs1
 *      [14:12]=f3  [11:8]=imm[4:1]  [7]=imm[11]  [6:0]=op
 *  U:  [31:12]=imm[31:12]  [11:7]=rd  [6:0]=opcode
 *  J:  [31]=imm[20]  [30:21]=imm[10:1]  [20]=imm[11]  [19:12]=imm[19:12]
 *      [11:7]=rd  [6:0]=opcode
 */

uint32_t encode_r(uint32_t opcode, uint32_t funct3, uint32_t funct7,
                  uint32_t rd, uint32_t rs1, uint32_t rs2);

uint32_t encode_i(uint32_t opcode, uint32_t funct3,
                  uint32_t rd, uint32_t rs1, int32_t imm);

uint32_t encode_s(uint32_t opcode, uint32_t funct3,
                  uint32_t rs1, uint32_t rs2, int32_t imm);

uint32_t encode_b(uint32_t opcode, uint32_t funct3,
                  uint32_t rs1, uint32_t rs2, int32_t imm);

uint32_t encode_u(uint32_t opcode, uint32_t rd, int32_t imm);

uint32_t encode_j(uint32_t opcode, uint32_t rd, int32_t imm);

/*
 * Parse "offset(reg)" or "(reg)" memory operand.
 * Returns 0 on success, -1 on error.
 */
int parse_mem_operand(const char *operand, int *reg, int32_t *offset);

/*
 * Parse an integer literal (decimal or 0x hex).
 * Sets *ok = 1 on success, 0 on failure.
 */
int32_t parse_imm(const char *s, int *ok);

#endif /* ENCODE_H */

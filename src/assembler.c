#include "assembler.h"
#include "parser.h"
#include "registers.h"
#include "instr_table.h"
#include "symbols.h"
#include "encode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 8192

/* ── helpers ─────────────────────────────────────────────────────── */

static int is_shift_imm(const char *mnemonic) {
    return strcmp(mnemonic, "slli") == 0
        || strcmp(mnemonic, "srli") == 0
        || strcmp(mnemonic, "srai") == 0;
}

/* ── pass-2 instruction encoder ──────────────────────────────────── */

static int encode_instruction(const ParsedLine *pl, const InstrSpec *spec,
                               uint32_t pc, uint32_t *out_instr, int lineno) {
    uint32_t instr = 0;

    switch (spec->format) {

    /* ── R-type: rd, rs1, rs2 ──────────────────────────────────── */
    case FMT_R: {
        int rd  = parse_register(pl->operands[0]);
        int rs1 = parse_register(pl->operands[1]);
        int rs2 = parse_register(pl->operands[2]);
        if (rd < 0 || rs1 < 0 || rs2 < 0) {
            fprintf(stderr, "line %d: bad register in '%s'\n", lineno, pl->mnemonic);
            return -1;
        }
        instr = encode_r(spec->opcode, spec->funct3, spec->funct7,
                         (uint32_t)rd, (uint32_t)rs1, (uint32_t)rs2);
        break;
    }

    /* ── I-type ────────────────────────────────────────────────── */
    case FMT_I: {
        int rd = parse_register(pl->operands[0]);
        if (rd < 0) {
            fprintf(stderr, "line %d: bad rd in '%s'\n", lineno, pl->mnemonic);
            return -1;
        }

        if (spec->opcode == 0x03) {
            /* loads: lw rd, offset(rs1) */
            int rs1; int32_t offset;
            if (parse_mem_operand(pl->operands[1], &rs1, &offset) != 0) {
                fprintf(stderr, "line %d: bad memory operand '%s'\n",
                        lineno, pl->operands[1]);
                return -1;
            }
            instr = encode_i(spec->opcode, spec->funct3,
                             (uint32_t)rd, (uint32_t)rs1, offset);

        } else if (spec->opcode == 0x67) {
            /* jalr: rd, rs1, imm  OR  rd, offset(rs1) */
            if (strchr(pl->operands[1], '(')) {
                int rs1; int32_t offset;
                if (parse_mem_operand(pl->operands[1], &rs1, &offset) != 0) {
                    fprintf(stderr, "line %d: bad jalr operand\n", lineno);
                    return -1;
                }
                instr = encode_i(spec->opcode, spec->funct3,
                                 (uint32_t)rd, (uint32_t)rs1, offset);
            } else {
                int rs1 = parse_register(pl->operands[1]);
                int ok = 0;
                int32_t imm = parse_imm(pl->operands[2], &ok);
                if (rs1 < 0 || !ok) {
                    fprintf(stderr, "line %d: bad jalr operands\n", lineno);
                    return -1;
                }
                instr = encode_i(spec->opcode, spec->funct3,
                                 (uint32_t)rd, (uint32_t)rs1, imm);
            }

        } else if (is_shift_imm(spec->mnemonic)) {
            /* slli/srli/srai: rd, rs1, shamt */
            int rs1 = parse_register(pl->operands[1]);
            int ok = 0;
            int32_t shamt = parse_imm(pl->operands[2], &ok);
            if (rs1 < 0 || !ok || shamt < 0 || shamt > 31) {
                fprintf(stderr, "line %d: bad shift operands\n", lineno);
                return -1;
            }
            /* Pack funct7 into upper 7 bits of 12-bit imm field */
            int32_t imm = (int32_t)((spec->funct7 << 5) | (shamt & 0x1F));
            instr = encode_i(spec->opcode, spec->funct3,
                             (uint32_t)rd, (uint32_t)parse_register(pl->operands[1]),
                             imm);
        } else {
            /* regular I-type arithmetic: rd, rs1, imm */
            int rs1 = parse_register(pl->operands[1]);
            int ok = 0;
            int32_t imm = parse_imm(pl->operands[2], &ok);
            if (rs1 < 0 || !ok) {
                fprintf(stderr, "line %d: bad I-type operands for '%s'\n",
                        lineno, pl->mnemonic);
                return -1;
            }
            instr = encode_i(spec->opcode, spec->funct3,
                             (uint32_t)rd, (uint32_t)rs1, imm);
        }
        break;
    }

    /* ── S-type: rs2, offset(rs1) ──────────────────────────────── */
    case FMT_S: {
        int rs2 = parse_register(pl->operands[0]);
        int rs1; int32_t offset;
        if (rs2 < 0) {
            fprintf(stderr, "line %d: bad rs2 in '%s'\n", lineno, pl->mnemonic);
            return -1;
        }
        if (parse_mem_operand(pl->operands[1], &rs1, &offset) != 0) {
            fprintf(stderr, "line %d: bad memory operand '%s'\n",
                    lineno, pl->operands[1]);
            return -1;
        }
        instr = encode_s(spec->opcode, spec->funct3,
                         (uint32_t)rs1, (uint32_t)rs2, offset);
        break;
    }

    /* ── B-type: rs1, rs2, label/imm ───────────────────────────── */
    case FMT_B: {
        int rs1 = parse_register(pl->operands[0]);
        int rs2 = parse_register(pl->operands[1]);
        if (rs1 < 0 || rs2 < 0) {
            fprintf(stderr, "line %d: bad register in '%s'\n", lineno, pl->mnemonic);
            return -1;
        }
        int32_t offset;
        uint32_t label_addr;
        if (symbols_find(pl->operands[2], &label_addr) == 0) {
            offset = (int32_t)(label_addr - pc);
        } else {
            int ok = 0;
            offset = parse_imm(pl->operands[2], &ok);
            if (!ok) {
                fprintf(stderr, "line %d: undefined label '%s'\n",
                        lineno, pl->operands[2]);
                return -1;
            }
        }
        instr = encode_b(spec->opcode, spec->funct3,
                         (uint32_t)rs1, (uint32_t)rs2, offset);
        break;
    }

    /* ── U-type: rd, imm ───────────────────────────────────────── */
    case FMT_U: {
        int rd = parse_register(pl->operands[0]);
        int ok = 0;
        int32_t imm = parse_imm(pl->operands[1], &ok);
        if (rd < 0 || !ok) {
            fprintf(stderr, "line %d: bad U-type operands\n", lineno);
            return -1;
        }
        instr = encode_u(spec->opcode, (uint32_t)rd, imm);
        break;
    }

    /* ── J-type: rd, label/imm ─────────────────────────────────── */
    case FMT_J: {
        int rd = parse_register(pl->operands[0]);
        if (rd < 0) {
            fprintf(stderr, "line %d: bad rd in '%s'\n", lineno, pl->mnemonic);
            return -1;
        }
        int32_t offset;
        uint32_t label_addr;
        if (symbols_find(pl->operands[1], &label_addr) == 0) {
            offset = (int32_t)(label_addr - pc);
        } else {
            int ok = 0;
            offset = parse_imm(pl->operands[1], &ok);
            if (!ok) {
                fprintf(stderr, "line %d: undefined label '%s'\n",
                        lineno, pl->operands[1]);
                return -1;
            }
        }
        instr = encode_j(spec->opcode, (uint32_t)rd, offset);
        break;
    }

    default:
        fprintf(stderr, "line %d: unknown format\n", lineno);
        return -1;
    }

    *out_instr = instr;
    return 0;
}

/* ── public API ──────────────────────────────────────────────────── */

int assemble_file(const char *input_path, const char *output_path) {
    FILE *fin = fopen(input_path, "r");
    if (!fin) {
        fprintf(stderr, "Error: cannot open '%s'\n", input_path);
        return -1;
    }

    /* Read all lines into memory */
    static char lines[MAX_LINES][MAX_LINE_LEN];
    int num_lines = 0;
    while (num_lines < MAX_LINES && fgets(lines[num_lines], MAX_LINE_LEN, fin))
        num_lines++;
    fclose(fin);

    symbols_init();

    /* ── PASS 1: build symbol table ─────────────────────────────── */
    uint32_t pc = 0;
    for (int i = 0; i < num_lines; i++) {
        ParsedLine pl;
        parse_line(lines[i], &pl);

        if (pl.label[0])
            if (symbols_add(pl.label, pc) != 0) return -1;

        if (pl.mnemonic[0])
            pc += 4;
    }

    /* ── PASS 2: encode instructions ────────────────────────────── */
    FILE *fout = output_path ? fopen(output_path, "w") : stdout;
    if (!fout) {
        fprintf(stderr, "Error: cannot open output '%s'\n", output_path);
        return -1;
    }

    int errors = 0;
    pc = 0;
    for (int i = 0; i < num_lines; i++) {
        ParsedLine pl;
        parse_line(lines[i], &pl);

        if (!pl.mnemonic[0]) continue;   /* blank / comment / label-only */

        /* Skip assembler directives */
        if (pl.mnemonic[0] == '.') { pc += 4; continue; }

        const InstrSpec *spec = lookup_instr(pl.mnemonic);
        if (!spec) {
            fprintf(stderr, "line %d: unknown instruction '%s'\n",
                    i + 1, pl.mnemonic);
            errors++;
            pc += 4;
            continue;
        }

        uint32_t instr = 0;
        if (encode_instruction(&pl, spec, pc, &instr, i + 1) != 0) {
            errors++;
        } else {
            fprintf(fout, "0x%08X: 0x%08X\n", pc, instr);
        }
        pc += 4;
    }

    if (output_path) fclose(fout);

    if (errors) {
        fprintf(stderr, "%d error(s) — assembly failed.\n", errors);
        return -1;
    }
    return 0;
}

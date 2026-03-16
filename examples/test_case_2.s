# test_case_2.s  –  57 instructions
# Exercises: all R-type, all I-type arithmetic, all I-type shifts,
#            lb/lbu/lh/lhu/lw, sb/sh/sw, all branches, jal, jalr

        # ── Initialise test values ──────────────────────────────
        addi    x1,  x0,  42        # x1 = 42
        addi    x2,  x0,  7         # x2 = 7
        addi    x3,  x0,  -1        # x3 = -1 (all ones)
        addi    x4,  x0,  100       # x4 = memory base
        addi    x5,  x0,  0         # x5 = 0

        # ── R-type arithmetic ───────────────────────────────────
        add     x6,  x1,  x2        # x6  = 49
        sub     x7,  x1,  x2        # x7  = 35
        and     x8,  x1,  x2        # x8  = 42 & 7 = 2
        or      x9,  x1,  x2        # x9  = 42 | 7 = 47
        xor     x10, x1,  x2        # x10 = 42 ^ 7 = 45
        sll     x11, x1,  x2        # x11 = 42 << 7 = 5376
        srl     x12, x11, x2        # x12 = 5376 >> 7 = 42
        sra     x13, x3,  x2        # x13 = -1 >>> 7 = -1 (arithmetic)
        slt     x14, x2,  x1        # x14 = 1 (7 < 42)
        sltu    x15, x2,  x1        # x15 = 1

        # ── I-type arithmetic ───────────────────────────────────
        addi    x16, x1,  -10       # x16 = 32
        slti    x17, x1,  50        # x17 = 1 (42 < 50)
        sltiu   x18, x1,  50        # x18 = 1
        xori    x19, x1,  0xFF      # x19 = 42 ^ 255 = 213
        ori     x20, x1,  0x80      # x20 = 42 | 128 = 170
        andi    x21, x1,  0x3F      # x21 = 42 & 63  = 42

        # ── I-type shifts ───────────────────────────────────────
        slli    x22, x1,  2         # x22 = 42 << 2 = 168
        srli    x23, x22, 2         # x23 = 168 >> 2 = 42
        srai    x24, x3,  4         # x24 = -1 >>> 4 = -1

        # ── Byte/halfword stores then loads ─────────────────────
        sb      x1,  0(x4)          # mem[100]   = 42 (byte)
        sh      x1,  2(x4)          # mem[102]   = 42 (half)
        sw      x1,  4(x4)          # mem[104]   = 42 (word)

        lb      x25, 0(x4)          # x25 = sign_ext(42)  = 42
        lbu     x26, 0(x4)          # x26 = zero_ext(42)  = 42
        lh      x27, 2(x4)          # x27 = sign_ext(42)  = 42
        lhu     x28, 2(x4)          # x28 = zero_ext(42)  = 42
        lw      x29, 4(x4)          # x29 = 42

        # ── Branch instructions (all six) ────────────────────────
        beq     x1,  x1,  beq_ok    # should branch (42 == 42)
        jal     x0,  fail
beq_ok:
        bne     x1,  x2,  bne_ok    # should branch (42 != 7)
        jal     x0,  fail
bne_ok:
        blt     x2,  x1,  blt_ok    # should branch (7 < 42)
        jal     x0,  fail
blt_ok:
        bge     x1,  x2,  bge_ok    # should branch (42 >= 7)
        jal     x0,  fail
bge_ok:
        bltu    x2,  x1,  bltu_ok   # should branch (7 < 42 unsigned)
        jal     x0,  fail
bltu_ok:
        bgeu    x1,  x2,  bgeu_ok   # should branch (42 >= 7 unsigned)
        jal     x0,  fail
bgeu_ok:

        # ── extra arithmetic to reach 50+ instructions ──────────
        add     x5,  x6,  x7        # x5  = 49 + 35 = 84
        sub     x5,  x5,  x2        # x5  = 84 - 7 = 77
        slli    x5,  x5,  1         # x5  = 154
        srli    x5,  x5,  1         # x5  = 77 (restored)
        andi    x5,  x5,  0x7F      # x5  = 77 & 127 = 77
        ori     x5,  x5,  0x80      # x5  = 77 | 128 = 205
        xori    x5,  x5,  0x80      # x5  = 205 ^ 128 = 77 (restored)
        slt     x5,  x5,  x1        # x5  = 1 (77 < 42? no → 0)
        addi    x5,  x0,  0         # x5  = 0 (reset)

        # ── jal / jalr round-trip ────────────────────────────────
        jal     x30, after_jal      # x30 = return address (pc+4)
after_jal:
        jalr    x0,  x30, 0         # jump to x30+0 (return address)
                                    # (creates a loop; just tests encoding)

        # ── Done ─────────────────────────────────────────────────
done:
        jal     x0,  done           # halt

fail:
        jal     x0,  fail           # halt on test failure

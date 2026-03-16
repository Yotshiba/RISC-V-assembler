# test_case_3.s  –  120+ instructions
# Comprehensive RV32I subset workout:
#   • every required mnemonic appears at least once
#   • several loops, forward and backward branches
#   • byte / halfword / word memory round-trips
#   • jal / jalr subroutine call convention

# ────────────────────────────────────────────────────────────────────
# Section 1: register initialisation
# ────────────────────────────────────────────────────────────────────
        addi    x1,  x0,  1         # x1  = 1
        addi    x2,  x0,  2         # x2  = 2
        addi    x3,  x0,  3         # x3  = 3
        addi    x4,  x0,  4         # x4  = 4
        addi    x5,  x0,  5         # x5  = 5
        addi    x6,  x0,  10        # x6  = 10
        addi    x7,  x0,  -1        # x7  = -1 (all ones)
        addi    x8,  x0,  100       # x8  = memory base
        addi    x9,  x0,  0         # x9  = 0  (accumulator)

# ────────────────────────────────────────────────────────────────────
# Section 2: R-type – all 10 opcodes
# ────────────────────────────────────────────────────────────────────
        add     x10, x1,  x2        # x10 = 3
        sub     x10, x6,  x5        # x10 = 5
        and     x11, x6,  x5        # x11 = 10 & 5  = 0
        or      x11, x6,  x5        # x11 = 10 | 5  = 15
        xor     x11, x6,  x5        # x11 = 10 ^ 5  = 15
        sll     x12, x1,  x3        # x12 = 1 << 3  = 8
        srl     x13, x12, x3        # x13 = 8 >> 3  = 1
        sra     x14, x7,  x3        # x14 = -1 >> 3 = -1
        slt     x15, x5,  x6        # x15 = 1 (5 < 10)
        sltu    x16, x5,  x6        # x16 = 1

# ────────────────────────────────────────────────────────────────────
# Section 3: I-type arithmetic – all 6 opcodes
# ────────────────────────────────────────────────────────────────────
        addi    x17, x6,  -3        # x17 = 7
        slti    x18, x17, 8         # x18 = 1 (7 < 8)
        sltiu   x19, x17, 8         # x19 = 1
        xori    x20, x6,  0x0F      # x20 = 10 ^ 15 = 5
        ori     x21, x6,  0x0F      # x21 = 10 | 15 = 15
        andi    x22, x6,  0x0F      # x22 = 10 & 15 = 10

# ────────────────────────────────────────────────────────────────────
# Section 4: I-type shifts – slli / srli / srai
# ────────────────────────────────────────────────────────────────────
        slli    x23, x1,  4         # x23 = 1  << 4 = 16
        srli    x24, x23, 2         # x24 = 16 >> 2 = 4
        srai    x25, x7,  1         # x25 = -1 >> 1 = -1 (arith)
        slli    x26, x5,  3         # x26 = 5  << 3 = 40
        srli    x27, x26, 3         # x27 = 40 >> 3 = 5
        srai    x28, x26, 1         # x28 = 40 >> 1 = 20

# ────────────────────────────────────────────────────────────────────
# Section 5: stores (sb / sh / sw)
# ────────────────────────────────────────────────────────────────────
        addi    x29, x0,  0x5A      # x29 = 0x5A (test byte)
        sb      x29, 0(x8)          # mem[100]   = 0x5A
        addi    x29, x0,  0x1234    # x29 = 0x1234 (test half)
        sh      x29, 2(x8)          # mem[102]   = 0x1234
        addi    x29, x0,  0x7FF     # x29 = 0x7FF (test word lower)
        sw      x29, 4(x8)          # mem[104]   = 0x7FF

# ────────────────────────────────────────────────────────────────────
# Section 6: loads (lb / lbu / lh / lhu / lw)
# ────────────────────────────────────────────────────────────────────
        lb      x1,  0(x8)          # x1  = sign_ext(0x5A) = 90
        lbu     x2,  0(x8)          # x2  = zero_ext(0x5A) = 90
        lh      x3,  2(x8)          # x3  = sign_ext(0x1234) = 0x1234
        lhu     x4,  2(x8)          # x4  = 0x1234
        lw      x5,  4(x8)          # x5  = 0x7FF

# ────────────────────────────────────────────────────────────────────
# Section 7: counter loop using beq
# ────────────────────────────────────────────────────────────────────
        addi    x6,  x0,  0         # x6  = 0  (counter)
        addi    x7,  x0,  8         # x7  = 8  (limit)
        addi    x9,  x0,  0         # x9  = 0  (sum)
loop1:
        beq     x6,  x7,  loop1_end # if counter == 8 → done
        add     x9,  x9,  x6        # sum += counter
        addi    x6,  x6,  1         # counter++
        jal     x0,  loop1          # repeat
loop1_end:

# ────────────────────────────────────────────────────────────────────
# Section 8: countdown loop using bne
# ────────────────────────────────────────────────────────────────────
        addi    x10, x0,  5         # x10 = 5  (countdown)
        addi    x11, x0,  0         # x11 = 0  (reference)
loop2:
        addi    x10, x10, -1        # x10--
        bne     x10, x11, loop2     # loop until x10 == 0

# ────────────────────────────────────────────────────────────────────
# Section 9: signed-compare loop (blt / bge)
# ────────────────────────────────────────────────────────────────────
        addi    x12, x0,  -4        # x12 = -4
        addi    x13, x0,  0         # x13 = 0
loop3:
        bge     x12, x13, loop3_end # if x12 >= 0 → done
        addi    x12, x12, 1         # x12++
        jal     x0,  loop3
loop3_end:

# ────────────────────────────────────────────────────────────────────
# Section 10: unsigned-compare loop (bltu / bgeu)
# ────────────────────────────────────────────────────────────────────
        addi    x14, x0,  0         # x14 = 0
        addi    x15, x0,  4         # x15 = 4
loop4:
        bltu    x14, x15, loop4_body# if x14 < 4 (unsigned) → body
        jal     x0,  loop4_end
loop4_body:
        addi    x14, x14, 1
        jal     x0,  loop4
loop4_end:

# ────────────────────────────────────────────────────────────────────
# Section 11: subroutine call via jal / jalr
# ────────────────────────────────────────────────────────────────────
        jal     x31, square         # call square(x1)  — x31 = ret addr
        jal     x0,  after_square   # skip over subroutine body

        # subroutine: square(x1) → x1 = x1 * x1 using addition loop
square:
        addi    x16, x0,  0         # result = 0
        addi    x17, x0,  0         # i = 0
sq_loop:
        beq     x17, x1,  sq_done  # if i == x1 → done
        add     x16, x16, x1        # result += x1
        addi    x17, x17, 1         # i++
        jal     x0,  sq_loop
sq_done:
        add     x1,  x16, x0        # x1 = result (return value in x1)
        jalr    x0,  x31, 0         # return

after_square:

# ────────────────────────────────────────────────────────────────────
# Section 12: memory array fill and sum
# ────────────────────────────────────────────────────────────────────
        addi    x18, x0,  200       # base of array
        addi    x19, x0,  0         # index i = 0
        addi    x20, x0,  6         # length = 6
fill_loop:
        beq     x19, x20, fill_done # if i == 6 → done
        slli    x21, x19, 2         # byte offset = i * 4
        add     x22, x18, x21       # addr = base + offset
        sw      x19, 0(x22)         # mem[base + i*4] = i
        addi    x19, x19, 1         # i++
        jal     x0,  fill_loop
fill_done:

        # sum the array back
        addi    x19, x0,  0         # i = 0
        addi    x23, x0,  0         # total = 0
sum_loop:
        beq     x19, x20, sum_done  # if i == 6 → done
        slli    x21, x19, 2
        add     x22, x18, x21
        lw      x24, 0(x22)         # load element
        add     x23, x23, x24       # total += element
        addi    x19, x19, 1
        jal     x0,  sum_loop
sum_done:

# ────────────────────────────────────────────────────────────────────
# Section 13: bgeu boundary test
# ────────────────────────────────────────────────────────────────────
        addi    x25, x0,  10
        addi    x26, x0,  10
        bgeu    x25, x26, bgeu_ok   # 10 >= 10 unsigned → branch
        jal     x0,  error
bgeu_ok:
        addi    x27, x0,  1         # flag = 1 (bgeu worked)

# ────────────────────────────────────────────────────────────────────
# End
# ────────────────────────────────────────────────────────────────────
# ────────────────────────────────────────────────────────────────────
# Section 14: additional R-type and I-type to reach 120+ instructions
# ────────────────────────────────────────────────────────────────────
        addi    x1,  x0,  15        # x1  = 15
        addi    x2,  x0,  9         # x2  = 9
        add     x3,  x1,  x2        # x3  = 24
        sub     x4,  x3,  x2        # x4  = 15
        and     x5,  x1,  x2        # x5  = 15 & 9 = 9
        or      x6,  x1,  x2        # x6  = 15 | 9 = 15
        xor     x7,  x1,  x2        # x7  = 15 ^ 9 = 6
        sll     x8,  x1,  x2        # x8  = 15 << 9 = 7680
        srl     x9,  x8,  x2        # x9  = 7680 >> 9 = 15
        sra     x10, x8,  x2        # x10 = 7680 >> 9 = 15
        slt     x11, x2,  x1        # x11 = 1 (9 < 15)
        sltu    x12, x2,  x1        # x12 = 1
        addi    x13, x1,  -7        # x13 = 8
        slti    x14, x13, 10        # x14 = 1 (8 < 10)
        sltiu   x15, x13, 10        # x15 = 1
        xori    x16, x1,  0x0A      # x16 = 15 ^ 10 = 5
        ori     x17, x1,  0x10      # x17 = 15 | 16 = 31
        andi    x18, x1,  0x07      # x18 = 15 & 7  = 7
        slli    x19, x1,  2         # x19 = 60
        srli    x20, x19, 2         # x20 = 15
        srai    x21, x19, 2         # x21 = 15

# Section 15: nested memory stores/loads
        addi    x29, x0,  400       # base
        sw      x1,   0(x29)
        sw      x2,   4(x29)
        sw      x3,   8(x29)
        sw      x4,  12(x29)
        lw      x22,  0(x29)        # x22 = 15
        lw      x23,  4(x29)        # x23 = 9
        lw      x24,  8(x29)        # x24 = 24
        lw      x25, 12(x29)        # x25 = 15
        add     x26, x22, x23       # x26 = 24
        beq     x26, x24, sec15_ok  # 24 == 24 → branch
        jal     x0,  error
sec15_ok:

# Section 16: byte store-load pairs
        addi    x1,  x0,  0x7F      # x1 = 127
        addi    x29, x0,  500
        sb      x1,  0(x29)
        lb      x2,  0(x29)         # x2 = 127
        lbu     x3,  0(x29)         # x3 = 127
        sh      x1,  2(x29)
        lh      x4,  2(x29)         # x4 = 127
        lhu     x5,  2(x29)         # x5 = 127
        bne     x2,  x1,  error     # verify loads correct

end:
        jal     x0,  end            # halt

error:
        jal     x0,  error          # halt on unexpected path

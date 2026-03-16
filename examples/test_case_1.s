# test_case_1.s  –  24 instructions
# Exercises: addi, add, sub, and, or, xor, sll, srl, sra,
#            slt, sltu, sw, lw, blt, jal
# Simple sum-1-to-5 loop plus arithmetic showcase

        addi    x1,  x0,  10        # x1 = 10
        addi    x2,  x0,  3         # x2 = 3
        add     x3,  x1,  x2        # x3 = 13
        sub     x4,  x1,  x2        # x4 = 7
        and     x5,  x1,  x2        # x5 = 2  (10 & 3)
        or      x6,  x1,  x2        # x6 = 11 (10 | 3)
        xor     x7,  x1,  x2        # x7 = 9  (10 ^ 3)
        sll     x8,  x1,  x2        # x8 = 80 (10 << 3)
        srl     x9,  x8,  x2        # x9 = 10 (80 >> 3)
        sra     x10, x8,  x2        # x10= 10 (80 >>> 3, same for positive)
        slt     x11, x2,  x1        # x11= 1  (3 < 10)
        sltu    x12, x2,  x1        # x12= 1  (3 < 10 unsigned)

        # Store results to memory (base = 200)
        addi    x20, x0,  200       # x20 = memory base
        sw      x3,  0(x20)         # mem[200] = 13
        sw      x4,  4(x20)         # mem[204] = 7
        lw      x21, 0(x20)         # x21 = 13 (reload)
        lw      x22, 4(x20)         # x22 = 7  (reload)

        # Loop: sum 1..5 into x25
        addi    x23, x0,  1         # counter = 1
        addi    x24, x0,  0         # sum     = 0
        addi    x26, x0,  6         # limit   = 6

loop:
        blt     x23, x26, body      # if counter < 6 → body
        jal     x0,  done           # else halt
body:
        add     x24, x24, x23       # sum += counter
        addi    x23, x23, 1         # counter++
        jal     x0,  loop           # repeat

done:
        jal     x0,  done           # infinite loop (halt)

addi x10, x0, 2
addi x19, x0, 10
addi x20, x0, 0

start_loop:
beq x19, x10, exit_loop
addi x20, x20, 100
addi x19, x19, -1
beq x0, x0, start_loop

exit_loop:
add x21, x20, x0

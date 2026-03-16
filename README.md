# RISC-V Assembler (Lab 8-4)

A two-pass assembler for a subset of RV32I, written in C.
Supports all required arithmetic, memory, and control instructions from the CS61C reference card.

---

## Supported Instructions

| Format | Instructions |
|--------|-------------|
| R-type | `add` `sub` `and` `or` `xor` `sll` `srl` `sra` `slt` `sltu` |
| I-type (arith) | `addi` `andi` `ori` `xori` `slti` `sltiu` |
| I-type (shift) | `slli` `srli` `srai` |
| I-type (load)  | `lb` `lbu` `lh` `lhu` `lw` |
| S-type (store) | `sb` `sh` `sw` |
| B-type (branch)| `beq` `bne` `blt` `bge` `bltu` `bgeu` |
| J-type | `jal` `jalr` |
| U-type (bonus) | `lui` `auipc` |

---

## Build

Requires `gcc`.

```bash
# Build the assembler
gcc -Wall -Wextra -Isrc src/main.c src/assembler.c src/parser.c \
    src/registers.c src/instr_table.c src/symbols.c src/encode.c \
    -o assembler
```

Or with `make` (Linux/macOS):

```bash
make assembler
```

---

## Usage

```bash
# Print hex output to stdout
./assembler examples/test_case_1.s

# Write hex output to a file
./assembler examples/test_case_1.s output.hex
```

Output format — one instruction per line:

```
0x00000000: 0x00A00093
0x00000004: 0x00300113
0x00000008: 0x002081B3
...
```

Each line shows `PC: machine_code`. Paste the machine code column into the Venus simulator to verify.

---

## Assembly Syntax

```asm
# Comments start with #

        addi    x1, x0, 10      # x1 = 10

# Label on its own line
loop:
        addi    x1, x1, -1
        bne     x1, x0, loop    # backward branch to label

# Label and instruction on the same line
start:  addi    x2, x0, 5

# Memory operands
        lw      x3, 4(x2)       # load word at address x2+4
        sw      x3, 0(x1)       # store word
```

Both `x`-notation (`x0`–`x31`) and ABI names (`zero`, `ra`, `sp`, `a0`, `t0`, …) are accepted for registers.

---

## Repository Structure

```
riscv-assembler-lab/
├── Makefile
├── src/
│   ├── main.c          # entry point, CLI argument handling
│   ├── assembler.c/h   # two-pass pipeline coordinator
│   ├── parser.c/h      # line → label + mnemonic + operands
│   ├── registers.c/h   # register name → number
│   ├── instr_table.c/h # instruction specification table
│   ├── symbols.c/h     # label symbol table
│   └── encode.c/h      # bit-field packing (R/I/S/B/U/J)
├── tests/
│   ├── test_registers.c
│   ├── test_parser.c
│   ├── test_instr_table.c
│   ├── test_symbols.c
│   ├── test_encode.c
│   └── test_assembler.c
└── examples/
    ├── test_case_1.s   # 26 instructions
    ├── test_case_2.s   # 57 instructions
    └── test_case_3.s   # 141 instructions
```

---

## Two-Pass Design

**Pass 1** — scans every line and records labels with their byte addresses in the symbol table.

**Pass 2** — encodes each instruction. Branch and jump targets are resolved by looking up the label address computed in pass 1, then calculating `offset = label_address − PC`.

This allows forward references such as `beq x1, x2, done` where `done:` appears later in the file.

---

## Running Tests

Build and run each module test individually:

```bash
gcc -Wall -Wextra -Isrc tests/test_registers.c  src/registers.c  -o test_registers  && ./test_registers
gcc -Wall -Wextra -Isrc tests/test_parser.c     src/parser.c     -o test_parser     && ./test_parser
gcc -Wall -Wextra -Isrc tests/test_instr_table.c src/instr_table.c -o test_instr_table && ./test_instr_table
gcc -Wall -Wextra -Isrc tests/test_symbols.c    src/symbols.c    -o test_symbols    && ./test_symbols
gcc -Wall -Wextra -Isrc tests/test_encode.c     src/encode.c src/registers.c -o test_encode && ./test_encode
gcc -Wall -Wextra -Isrc tests/test_assembler.c  src/assembler.c src/parser.c src/registers.c \
    src/instr_table.c src/symbols.c src/encode.c -o test_assembler && ./test_assembler
```

Or with make:

```bash
make test
```

**140 tests, 0 failures.**

---

## Verifying Against Venus

1. Open [Venus](https://venus.cs61c.org) (or the desktop version).
2. Paste the contents of an `.s` example file into the editor.
3. Assemble in Venus and note the machine code dump.
4. Run `./assembler examples/test_case_1.s` and compare the hex values column by column.

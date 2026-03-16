CC     = gcc
CFLAGS = -Wall -Wextra -Isrc

SRC = src/assembler.c src/parser.c src/registers.c \
      src/instr_table.c src/symbols.c src/encode.c

# ── Main assembler ──────────────────────────────────────────────────
assembler: src/main.c $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

# ── Individual module tests ─────────────────────────────────────────
test_registers: tests/test_registers.c src/registers.c
	$(CC) $(CFLAGS) $^ -o $@

test_parser: tests/test_parser.c src/parser.c
	$(CC) $(CFLAGS) $^ -o $@

test_instr_table: tests/test_instr_table.c src/instr_table.c
	$(CC) $(CFLAGS) $^ -o $@

test_symbols: tests/test_symbols.c src/symbols.c
	$(CC) $(CFLAGS) $^ -o $@

test_encode: tests/test_encode.c src/encode.c src/registers.c
	$(CC) $(CFLAGS) $^ -o $@

test_assembler: tests/test_assembler.c $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

# ── Run all tests ───────────────────────────────────────────────────
test: test_registers test_parser test_instr_table test_symbols \
      test_encode test_assembler
	@echo ""; echo "── test_registers ──────────────────"
	./test_registers
	@echo ""; echo "── test_parser ─────────────────────"
	./test_parser
	@echo ""; echo "── test_instr_table ────────────────"
	./test_instr_table
	@echo ""; echo "── test_symbols ────────────────────"
	./test_symbols
	@echo ""; echo "── test_encode ─────────────────────"
	./test_encode
	@echo ""; echo "── test_assembler ──────────────────"
	./test_assembler

# ── Assemble example files ──────────────────────────────────────────
examples: assembler
	./assembler examples/test_case_1.s examples/test_case_1.hex
	./assembler examples/test_case_2.s examples/test_case_2.hex
	./assembler examples/test_case_3.s examples/test_case_3.hex

# ── Cleanup ─────────────────────────────────────────────────────────
clean:
	rm -f assembler test_registers test_parser test_instr_table \
	      test_symbols test_encode test_assembler \
	      examples/*.hex *.hex

.PHONY: test examples clean

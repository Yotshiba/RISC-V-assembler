#include "assembler.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <input.s> [output.hex]\n", argv[0]);
        return 1;
    }

    const char *input  = argv[1];
    const char *output = (argc == 3) ? argv[2] : NULL;

    if (assemble_file(input, output) != 0)
        return 1;

    return 0;
}

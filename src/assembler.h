#ifndef ASSEMBLER_H
#define ASSEMBLER_H

/*
 * assembler.h - Two-pass assembler pipeline
 */

/*
 * Assemble input_path and write hex output to output_path.
 * If output_path is NULL, results go to stdout.
 * Returns 0 on success, -1 on error.
 */
int assemble_file(const char *input_path, const char *output_path);

#endif /* ASSEMBLER_H */

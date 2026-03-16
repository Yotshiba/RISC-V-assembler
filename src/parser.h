#ifndef PARSER_H
#define PARSER_H

/*
 * parser.h - Assembly line parser
 * Breaks a line of text into label, mnemonic, and operands.
 */

#define MAX_OPERANDS   4
#define MAX_TOKEN_LEN  64
#define MAX_LINE_LEN   256

typedef struct {
    char label[MAX_TOKEN_LEN];                  /* optional label (without ':') */
    char mnemonic[MAX_TOKEN_LEN];               /* instruction mnemonic         */
    char operands[MAX_OPERANDS][MAX_TOKEN_LEN]; /* operand tokens               */
    int  num_operands;
} ParsedLine;

/*
 * Parse one line of assembly text into a ParsedLine.
 * Returns 0 on success.
 * If the line is empty, a comment, or label-only, mnemonic[0] == '\0'.
 */
int parse_line(const char *line, ParsedLine *out);

#endif /* PARSER_H */

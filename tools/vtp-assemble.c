/*
 * Copyright 2020 Lucas Hinderberger
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vtp/codec.h>

#define LINE_BUFFER_SIZE (128)
#define NEXT_CHAR(parser) (parser->input[0])
#define TOKEN_BUFFER_SIZE (16)

enum eOutputMode {
    OUTPUT_MODE_DEFAULT = 0,
    OUTPUT_MODE_C_ARRAY
};
typedef enum eOutputMode OutputMode;

struct sAssemblerArgs {
    FILE* input;
    FILE* output;
    OutputMode output_mode;
};
typedef struct sAssemblerArgs AssemblerArgs;

enum eCharacterClass {
    CHAR_CLASS_ALPHA,
    CHAR_CLASS_COMMENT_DASH,
    CHAR_CLASS_DIGIT,
    CHAR_CLASS_LINE_BREAK,
    CHAR_CLASS_SYMBOL,
    CHAR_CLASS_WHITESPACE,
    CHAR_CLASS_OTHER
};
typedef enum eCharacterClass CharacterClass;

enum eParserError {
    PARSER_OK,
    PARSER_EOF,
    PARSER_INVALID_TOKEN_TYPE,
    PARSER_TOKEN_TOO_LONG,
    PARSER_UNEXPECTED_SYMBOL,
    PARSER_VALUE_OUT_OF_RANGE
};
typedef enum eParserError ParserError;

enum eSymbol {
    SYMBOL_AMP, SYMBOL_FREQ, SYMBOL_TIME,
    SYMBOL_CHANNEL, SYMBOL_COMMENT, SYMBOL_MILLISECONDS, SYMBOL_PLUS, SYMBOL_WILDCARD
};
typedef enum eSymbol Symbol;

enum eTokenType {
    TOKEN_NONE,
    TOKEN_SYMBOL,
    TOKEN_NUMBER,
    TOKEN_LINE_BREAK,
    TOKEN_WHITESPACE
};
typedef enum eTokenType TokenType;

union uTokenValue {
    Symbol symbol;
    unsigned long number;
};
typedef union uTokenValue TokenValue;

struct sToken {
    TokenType type;
    TokenValue value;
};
typedef struct sToken Token;

struct sParser {
    const char* input;
    Token token;
};
typedef struct sParser Parser;

CharacterClass char_class(char c);
char consume_char(Parser* parser);
ParserError next_token(Parser* parser);
ParserError skip_to_line_break(Parser* parser);

ParserError parse_instruction_v1(Parser* parser, VTPInstructionV1* out);
ParserError parse_format_a_parameters(Parser* parser, VTPInstructionParamsA* out);
ParserError parse_format_b_parameters(Parser* parser, VTPInstructionParamsB* out);

ParserError parse_channel_select(Parser* parser, unsigned char* out);
ParserError parse_time_offset(Parser* parser, unsigned long* out);

ParserError parse_comment(Parser* parser);
ParserError parse_empty_line(Parser* parser);
ParserError parse_line_end(Parser* parser);

void read_command_line_args(int argc, char** args, AssemblerArgs* out);


void print_parser_error(ParserError error, unsigned int line, unsigned int column) {
    fprintf(stderr, "Error at (%u,%u): ", line, column);

    switch (error) {
        case PARSER_INVALID_TOKEN_TYPE:
            fputs("Invalid token type", stderr);
            break;
        case PARSER_TOKEN_TOO_LONG:
            fputs("Token too long", stderr);
            break;
        case PARSER_UNEXPECTED_SYMBOL:
            fputs("Unexpected symbol", stderr);
            break;
        case PARSER_VALUE_OUT_OF_RANGE:
            fputs("Value out of range", stderr);
            break;
        default:
            fprintf(stderr, "Unknown error");
            break;
    }

    fputc('\n', stderr);
}

int validate_instruction(const VTPInstructionV1* instruction, unsigned int line) {
    switch (instruction->code) {
        case VTP_INST_INCREMENT_TIME:
            if (instruction->params.format_a.parameter_a > 0xFFFFFFF) {
                fprintf(stderr, "Parameter A out of range at line %d\n", line);
                return 0;
            }
            break;
        case VTP_INST_SET_FREQUENCY:
        case VTP_INST_SET_AMPLITUDE:
            if (instruction->params.format_b.parameter_a >= 1024) {
                fprintf(stderr, "Parameter A out of range at line %d\n", line);
                return 0;
            }
            if (instruction->params.format_b.time_offset >= 1024) {
                fprintf(stderr, "Time offset out of range at line %d\n", line);
                return 0;
            }
            break;
        default:
            fprintf(stderr, "Unknown instruction at line %d\n", line);
            return 0;
    }

    return 1;
}

void write_instruction_binary(FILE* f, VTPInstructionWord instruction) {
    unsigned char buffer[4];

    vtp_write_instruction_words(1, &instruction, buffer);

    if (fwrite(buffer, 1, 4, f) != 4) {
        fputs("I/O error writing to output file\n", stderr);
        exit(1);
    }
}

void write_instruction_hex(FILE* f, VTPInstructionWord instruction, int first) {
    if (!first)
        fputs(", ", f);
    fprintf(f, "0x%08lx", instruction);
}

int main(int argc, char** args) {
    AssemblerArgs parsed_args;
    char line_buffer[LINE_BUFFER_SIZE];
    int n_instructions, instructions_valid;
    Parser parser;
    ParserError err;
    unsigned int line;
    VTPError vtp_err;
    VTPInstructionV1 instruction;
    VTPInstructionWord instruction_word;

    read_command_line_args(argc, args, &parsed_args);

    instructions_valid = 1;
    line = 0;
    n_instructions = 0;

    while (fgets(line_buffer, LINE_BUFFER_SIZE, parsed_args.input)) {
        line++;
        parser.input = line_buffer;

        if ((err = next_token(&parser)) != PARSER_OK && err != PARSER_EOF) {
            print_parser_error(err, line, parser.input - line_buffer);
            exit(1);
        }

        if (err == PARSER_EOF)
            continue;

        if ((err = parse_instruction_v1(&parser, &instruction)) != PARSER_OK && err != PARSER_EOF) {
            print_parser_error(err, line, parser.input - line_buffer);
            exit(1);
        }

        if (err == PARSER_EOF)
            continue;

        n_instructions++;

        if (!validate_instruction(&instruction, line))
            instructions_valid = 0;

        if (instructions_valid) {
            if ((vtp_err = vtp_encode_instruction_v1(&instruction, &instruction_word)) != VTP_OK) {
                fprintf(stderr, "Unexpected VTP error: %d", vtp_err);
                exit(1);
            }

            switch (parsed_args.output_mode) {
                case OUTPUT_MODE_DEFAULT:
                    write_instruction_binary(parsed_args.output, instruction_word);
                    break;
                case OUTPUT_MODE_C_ARRAY:
                    write_instruction_hex(parsed_args.output, instruction_word, n_instructions == 1);
                    break;
            }
        }
    }

    if (instructions_valid)
        return 0;
    else
        return 1;
}


CharacterClass char_class(char c) {
    if (isalpha(c))
        return CHAR_CLASS_ALPHA;
    else if (isdigit(c))
        return CHAR_CLASS_DIGIT;
    else if (c == '-')
        return CHAR_CLASS_COMMENT_DASH;
    else if (ispunct(c))
        return CHAR_CLASS_SYMBOL;
    else if (c == ' ' || c == '\t')
        return CHAR_CLASS_WHITESPACE;
    else if (c == '\n')
        return CHAR_CLASS_LINE_BREAK;
    else
        return CHAR_CLASS_OTHER;
}

char consume_char(Parser* parser) {
    char c = NEXT_CHAR(parser);

    if (c == 0)
        return 0;

    parser->input++;

    return c;
}

ParserError next_token(Parser* parser) {
    char token_buffer[TOKEN_BUFFER_SIZE];
    CharacterClass token_class;
    size_t i;

    token_class = char_class(NEXT_CHAR(parser));
    i = 0;

    while (NEXT_CHAR(parser) && char_class(NEXT_CHAR(parser)) == token_class) {
        if (i >= (TOKEN_BUFFER_SIZE - 1))
            return PARSER_TOKEN_TOO_LONG;

        token_buffer[i] = (char)tolower(consume_char(parser));

        i++;
    }

    token_buffer[i] = 0;

    switch (token_class) {
        case CHAR_CLASS_ALPHA:
        case CHAR_CLASS_SYMBOL:
            parser->token.type = TOKEN_SYMBOL;

            if (!strcmp("amp", token_buffer))
                parser->token.value.symbol = SYMBOL_AMP;
            else if (!strcmp("freq", token_buffer))
                parser->token.value.symbol = SYMBOL_FREQ;
            else if (!strcmp("time", token_buffer))
                parser->token.value.symbol = SYMBOL_TIME;
            else if (!strcmp("ch", token_buffer))
                parser->token.value.symbol = SYMBOL_CHANNEL;
            else if (!strcmp("ms", token_buffer))
                parser->token.value.symbol = SYMBOL_MILLISECONDS;
            else if (!strcmp("+", token_buffer))
                parser->token.value.symbol = SYMBOL_PLUS;
            else if (!strcmp("*", token_buffer))
                parser->token.value.symbol = SYMBOL_WILDCARD;
            else
                return PARSER_UNEXPECTED_SYMBOL;
            break;
        case CHAR_CLASS_COMMENT_DASH:
            if (token_buffer[0] == '-' && token_buffer[1] == '-') {
                parser->token.type = TOKEN_SYMBOL;
                parser->token.value.symbol = SYMBOL_COMMENT;
            }
            else {
                return PARSER_UNEXPECTED_SYMBOL;
            }
            break;
        case CHAR_CLASS_DIGIT:
            parser->token.type = TOKEN_NUMBER;
            parser->token.value.number = strtoul(token_buffer, NULL, 10);
            break;
        case CHAR_CLASS_LINE_BREAK:
            parser->token.type = TOKEN_LINE_BREAK;
            break;
        case CHAR_CLASS_OTHER:
            parser->token.type = TOKEN_NONE;
            break;
        case CHAR_CLASS_WHITESPACE:
            parser->token.type = TOKEN_WHITESPACE;
            break;
        default:
            return PARSER_INVALID_TOKEN_TYPE;
    }

    return PARSER_OK;
}

ParserError skip_to_line_break(Parser* parser) {
    while (char_class(NEXT_CHAR(parser)) != CHAR_CLASS_LINE_BREAK && NEXT_CHAR(parser) != 0)
        consume_char(parser);

    return next_token(parser);
}

ParserError parse_instruction_v1(Parser* parser, VTPInstructionV1* out) {
    ParserError err;

    while (parser->token.type == TOKEN_WHITESPACE || parser->token.type == TOKEN_LINE_BREAK) {
        if ((err = parse_empty_line(parser)) != PARSER_OK)
            return err;
        if ((err = next_token(parser)) != PARSER_OK)
            return err;
    }

    if (parser->token.type == TOKEN_NONE)
        return PARSER_EOF;

    if (parser->token.type != TOKEN_SYMBOL)
        return PARSER_INVALID_TOKEN_TYPE;

    switch (parser->token.value.symbol) {
        case SYMBOL_COMMENT:
            if ((err = parse_comment(parser)) != PARSER_OK)
                return err;
            return parse_instruction_v1(parser, out);
        case SYMBOL_TIME:
            out->code = VTP_INST_INCREMENT_TIME;
            break;
        case SYMBOL_AMP:
            out->code = VTP_INST_SET_AMPLITUDE;
            break;
        case SYMBOL_FREQ:
            out->code = VTP_INST_SET_FREQUENCY;
            break;
        default:
            return PARSER_UNEXPECTED_SYMBOL;
    }

    if ((err = next_token(parser)) != PARSER_OK)
        return err;

    if (parser->token.type != TOKEN_WHITESPACE)
        return PARSER_INVALID_TOKEN_TYPE;

    if ((err = next_token(parser)) != PARSER_OK)
        return err;

    switch (out->code) {
        case VTP_INST_INCREMENT_TIME:
            return parse_format_a_parameters(parser, &out->params.format_a);
        case VTP_INST_SET_AMPLITUDE:
        case VTP_INST_SET_FREQUENCY:
            return parse_format_b_parameters(parser, &out->params.format_b);
        default:
            /* Cannot be reached */
            exit(1);
    }
}

ParserError parse_format_a_parameters(Parser* parser, VTPInstructionParamsA* out) {
    ParserError err;

    if ((err = parse_time_offset(parser, &out->parameter_a)) != PARSER_OK)
        return err;

    if ((err = next_token(parser)) != PARSER_OK)
        return err;

    return parse_line_end(parser);
}

ParserError parse_format_b_parameters(Parser* parser, VTPInstructionParamsB* out) {
    ParserError err;

    if (parser->token.type != TOKEN_SYMBOL)
        return PARSER_INVALID_TOKEN_TYPE;

    /* Time offset */
    if (parser->token.value.symbol == SYMBOL_PLUS) {
        unsigned long time_offset;

        if ((err = parse_time_offset(parser, &time_offset)) != PARSER_OK)
            return err;

        if (time_offset > 0xFFFF) /* Note that this intentionally does only check for the limit of the unsigned int type */
            return PARSER_VALUE_OUT_OF_RANGE;

        out->time_offset = (unsigned int)time_offset;

        if ((err = next_token(parser)) != PARSER_OK)
            return err;

        if (parser->token.type == TOKEN_WHITESPACE) {
            if ((err = next_token(parser)) != PARSER_OK)
                return err;
        }
    }
    else {
        out->time_offset = 0;
    }

    /* Channel select */
    if (parser->token.value.symbol != SYMBOL_CHANNEL)
        return PARSER_UNEXPECTED_SYMBOL;

    if ((err = parse_channel_select(parser, &out->channel_select)) != PARSER_OK)
        return err;

    if ((err = next_token(parser)) != PARSER_OK)
        return err;

    if (parser->token.type == TOKEN_WHITESPACE) {
        if ((err = next_token(parser)) != PARSER_OK)
            return err;
    }

    /* Parameter A */
    if (parser->token.type != TOKEN_NUMBER)
        return PARSER_INVALID_TOKEN_TYPE;
    if (parser->token.value.number > 0xFFFF)  /* Note that this intentionally does only check for the limit of the unsigned int type */
        return PARSER_VALUE_OUT_OF_RANGE;
    out->parameter_a = (unsigned int)parser->token.value.number;

    if ((err = next_token(parser)) != PARSER_OK)
        return err;

    return parse_line_end(parser);
}

ParserError parse_channel_select(Parser* parser, unsigned char* out) {
    ParserError err;

    if (parser->token.type != TOKEN_SYMBOL)
        return PARSER_INVALID_TOKEN_TYPE;

    if (parser->token.value.symbol != SYMBOL_CHANNEL)
        return PARSER_UNEXPECTED_SYMBOL;

    if ((err = next_token(parser)) != PARSER_OK)
        return err;

    if (parser->token.type == TOKEN_NUMBER) {
        if (parser->token.value.number == 0 || parser->token.value.number > 0xFF)
            return PARSER_VALUE_OUT_OF_RANGE;
        *out = (unsigned char)parser->token.value.number;
    }
    else if (parser->token.type == TOKEN_SYMBOL) {
        if (parser->token.value.symbol != SYMBOL_WILDCARD)
            return PARSER_UNEXPECTED_SYMBOL;
        *out = 0;
    }
    else {
        return PARSER_INVALID_TOKEN_TYPE;
    }

    return PARSER_OK;
}

ParserError parse_time_offset(Parser* parser, unsigned long* out) {
    ParserError err;

    if (parser->token.type != TOKEN_SYMBOL)
        return PARSER_INVALID_TOKEN_TYPE;
    if (parser->token.value.symbol != SYMBOL_PLUS)
        return PARSER_UNEXPECTED_SYMBOL;


    if ((err = next_token(parser)) != PARSER_OK)
        return err;

    if (parser->token.type != TOKEN_NUMBER)
        return PARSER_INVALID_TOKEN_TYPE;

    *out = parser->token.value.number;


    if ((err = next_token(parser)) != PARSER_OK)
        return err;

    if (parser->token.type != TOKEN_SYMBOL)
        return PARSER_INVALID_TOKEN_TYPE;
    if (parser->token.value.symbol != SYMBOL_MILLISECONDS)
        return PARSER_UNEXPECTED_SYMBOL;


    return PARSER_OK;
}

ParserError parse_comment(Parser* parser) {
    ParserError err;

    if (parser->token.type != TOKEN_SYMBOL)
        return PARSER_INVALID_TOKEN_TYPE;
    if (parser->token.value.symbol != SYMBOL_COMMENT)
        return PARSER_UNEXPECTED_SYMBOL;

    if ((err = skip_to_line_break(parser)) != PARSER_OK)
        return err;

    return PARSER_OK;
}

ParserError parse_empty_line(Parser* parser) {
    ParserError err;

    if (parser->token.type == TOKEN_WHITESPACE) {
        if ((err = next_token(parser)) != PARSER_OK)
            return err;
    }

    if (parser->token.type != TOKEN_LINE_BREAK && parser->token.type != TOKEN_NONE)
        return PARSER_INVALID_TOKEN_TYPE;

    return PARSER_OK;
}

ParserError parse_line_end(Parser* parser) {
    ParserError err;

    if (parser->token.type == TOKEN_WHITESPACE) {
        if ((err = next_token(parser)) != PARSER_OK)
            return err;
    }

    if (parser->token.type == TOKEN_LINE_BREAK || parser->token.type == TOKEN_NONE)
        return PARSER_OK;
    else
        return parse_comment(parser);
}

void read_command_line_args(int argc, char** args, AssemblerArgs* out) {
    int i;
    static const char* ARGUMENT_FORMAT = "%20s %s\n";

    memset(out, 0, sizeof(AssemblerArgs));

    for (i=1; i < argc; i++) {
        char* arg = args[i];

        if (!strcmp(arg, "-c")) {
            if (out->output_mode) {
                fprintf(stderr, "Duplicate output mode specified: %s\n", arg);
                exit(1);
            }

            out->output_mode = OUTPUT_MODE_C_ARRAY;
        }
        else if (!strcmp(arg, "-o")) {
            if (i+1 == argc) {
                fprintf(stderr, "Option without corresponding parameter: %s\n", arg);
                exit(1);
            }

            i++;
            arg = args[i];

            if (out->output) {
                fprintf(stderr, "Duplicate output file specified: %s\n", arg);
                exit(1);
            }

            out->output = fopen(arg, "wb");

            if (!out->output) {
                fprintf(stderr, "Could not open output file: %s\n", arg);
                exit(1);
            }
        }
        else if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
            fputs("Usage:\n\n", stderr);

            fputs("vtp-assemble [-o OUTPUT_FILENAME] [INPUT_FILENAME]\n\n", stderr);

            fputs("This program assembles VTP Assembly Code in its corresponding binary representation.\n", stderr);
            fputs("By default, it reads from stdin and writes to stdout, but you can override this using\n", stderr);
            fputs("the command line parameters listed below.\n\n", stderr);

            fprintf(stderr, ARGUMENT_FORMAT, "INPUT_FILENAME", "The file from which the VTP Assembly Code shall be read (default: stdin)");
            fprintf(stderr, ARGUMENT_FORMAT, "-c", "Output comma-separated C-style hexadecimal numbers instead of binary");
            fprintf(stderr, ARGUMENT_FORMAT, "-o OUTPUT_FILENAME", "The file to which the VTP Binary Code shall be written (default: stdout)");

            exit(0);
        }
        else {
            if (out->input) {
                fprintf(stderr, "Duplicate input file specified: %s\n", arg);
                exit(1);
            }

            out->input = fopen(arg, "r");

            if (!out->input) {
                fprintf(stderr, "Could not open input file: %s\n", arg);
                exit(1);
            }
        }
    }

    if (!out->input)
        out->input = stdin;
    if (!out->output)
        out->output = stdout;
}

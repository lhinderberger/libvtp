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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vtp/codec.h>

struct sDisassemblerArgs {
    FILE* input;
    FILE* output;
};
typedef struct sDisassemblerArgs DisassemblerArgs;

void read_command_line_args(int argc, char** args, DisassemblerArgs* out);
VTPInstructionWord read_instruction_word(FILE* input);
void read_instruction(FILE* input, VTPInstructionV1* instruction_out, unsigned int n_instructions);
void print_vtp_error(VTPError error, unsigned int n_instructions);
void write_instruction_assembly(FILE* output, const VTPInstructionV1* instruction);
void write_parameters_format_a(FILE* output, const VTPInstructionParamsA* params);
void write_parameters_format_b(FILE* output, const VTPInstructionParamsB* params);
void write_time_offset(FILE* output, unsigned long offset);


int main(int argc, char** args) {
    DisassemblerArgs parsed_args;
    VTPInstructionV1 instruction;
    unsigned int n_instructions;

    read_command_line_args(argc, args, &parsed_args);

    n_instructions = 0;
    while (!feof(parsed_args.input)) {
        n_instructions++;

        read_instruction(parsed_args.input, &instruction, n_instructions);
        write_instruction_assembly(parsed_args.output, &instruction);
    }

    return 0;
}


void print_vtp_error(VTPError error, unsigned int n_instructions) {
    fprintf(stderr, "Error at instruction #%u: ", n_instructions);

    switch (error) {
        case VTP_INVALID_INSTRUCTION_CODE:
            fputs("Invalid instruction code", stderr);
            break;
        default:
            fprintf(stderr, "Unexpected error: %d", error);
            break;
    }

    fputc('\n', stderr);
}

void read_command_line_args(int argc, char** args, DisassemblerArgs* out) {
    int i;
    static const char* ARGUMENT_FORMAT = "%20s %s\n";

    memset(out, 0, sizeof(DisassemblerArgs));

    for (i=1; i < argc; i++) {
        char* arg = args[i];

        if (!strcmp(arg, "-o")) {
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

            out->output = fopen(arg, "w");

            if (!out->output) {
                fprintf(stderr, "Could not open output file: %s\n", arg);
                exit(1);
            }
        }
        else if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
            fputs("Usage:\n\n", stderr);

            fputs("vtp-disassemble [-o OUTPUT_FILENAME] [INPUT_FILENAME]\n\n", stderr);

            fputs("This program disassembles VTP Binary Code in its corresponding assembly representation.\n", stderr);
            fputs("By default, it reads from stdin and writes to stdout, but you can override this using\n", stderr);
            fputs("the command line parameters listed below.\n\n", stderr);

            fprintf(stderr, ARGUMENT_FORMAT, "INPUT_FILENAME", "The file from which the VTP Binary Code shall be read (default: stdin)");
            fprintf(stderr, ARGUMENT_FORMAT, "-o OUTPUT_FILENAME", "The file to which the VTP Assembly Code shall be written (default: stdout)");

            exit(0);
        }
        else {
            if (out->input) {
                fprintf(stderr, "Duplicate input file specified: %s\n", arg);
                exit(1);
            }

            out->input = fopen(arg, "rb");

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

void read_instruction(FILE* input, VTPInstructionV1* instruction_out, unsigned int n_instructions) {
    VTPError err;
    VTPInstructionWord encoded = read_instruction_word(input);

    if ((err = vtp_decode_instruction_v1(encoded, instruction_out) != VTP_OK)) {
        print_vtp_error(err, n_instructions);
        exit(1);
    }
}

VTPInstructionWord read_instruction_word(FILE* input) {
    unsigned char buffer[4];

    if (fread(buffer, 1, 4, input) != 4) {
        fputs("Unexpected EOF", stderr);
        exit(1);
    }

    return ((unsigned long)buffer[0] << 24u) | ((unsigned long)buffer[1] << 16u) | ((unsigned long)buffer[2] << 8u) | buffer[3];
}

void write_instruction_assembly(FILE* output, const VTPInstructionV1* instruction) {
    switch (instruction->code) {
        case VTP_INST_INCREMENT_TIME:
            fputs("time ", output);
            write_parameters_format_a(output, &instruction->params.format_a);
            break;
        case VTP_INST_SET_FREQUENCY:
            fputs("freq ", output);
            write_parameters_format_b(output, &instruction->params.format_b);
            break;
        case VTP_INST_SET_AMPLITUDE:
            fputs("amp ", output);
            write_parameters_format_b(output, &instruction->params.format_b);
            break;
    }

    fputc('\n', output);
}

void write_parameters_format_a(FILE* output, const VTPInstructionParamsA* params) {
    write_time_offset(output, params->parameter_a);
}

void write_parameters_format_b(FILE* output, const VTPInstructionParamsB* params) {
    fputs("ch", output);

    if (params->channel_select == 0)
        fputc('*', output);
    else
        fprintf(output, "%u", params->channel_select);

    if (params->time_offset > 0) {
        fputc(' ', output);
        write_time_offset(output, params->time_offset);
    }

    fprintf(output, " %u", params->parameter_a);
}

void write_time_offset(FILE* output, unsigned long offset) {
    fprintf(output, "+%lums", offset);
}

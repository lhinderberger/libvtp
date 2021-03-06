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

#include <vtp/codec.h>

void vtp_decode_params_a(VTPInstructionWord instruction, VTPInstructionParamsA* out) {
    out->parameter_a = instruction & 0x0FFFFFFFu;
}

void vtp_decode_params_b(VTPInstructionWord instruction, VTPInstructionParamsB* out) {
    out->channel_select = (unsigned char)((instruction & 0x0FF00000u) >> 20u);
    out->time_offset = (unsigned int)((instruction & 0x000FFC00u) >> 10u);
    out->parameter_a = (unsigned int)(instruction & 0x000003FFu);
}

void vtp_encode_params_a(const VTPInstructionParamsA* params, VTPInstructionWord* out) {
    *out |= params->parameter_a & 0xFFFFFFFu;
}

void vtp_encode_params_b(const VTPInstructionParamsB* params, VTPInstructionWord* out) {
    *out |= (unsigned long)params->channel_select << 20u;
    *out |= ((unsigned long)params->time_offset & 0x3FFu) << 10u;
    *out |= ((unsigned long)params->parameter_a & 0x3FFu);
}

VTPError vtp_decode_instruction_v1(VTPInstructionWord instruction, VTPInstructionV1* out) {
    out->code = (VTPInstructionCode)((instruction & 0xF0000000u) >> 28u);

    switch (out->code) {
        case VTP_INST_INCREMENT_TIME:
            vtp_decode_params_a(instruction, &out->params.format_a);
            break;
        case VTP_INST_SET_AMPLITUDE:
        case VTP_INST_SET_FREQUENCY:
            vtp_decode_params_b(instruction, &out->params.format_b);
            break;
        default:
            return VTP_INVALID_INSTRUCTION_CODE;
    }

    return VTP_OK;
}

VTPError vtp_encode_instruction_v1(const VTPInstructionV1* instruction, VTPInstructionWord* out) {
    *out = ((unsigned long)instruction->code << 28u);

    switch (instruction->code) {
        case VTP_INST_INCREMENT_TIME:
            vtp_encode_params_a(&instruction->params.format_a, out);
            break;
        case VTP_INST_SET_AMPLITUDE:
        case VTP_INST_SET_FREQUENCY:
            vtp_encode_params_b(&instruction->params.format_b, out);
            break;
        default:
            return VTP_INVALID_INSTRUCTION_CODE;
    }

    return VTP_OK;
}

VTPError vtp_decode_instructions_v1(const VTPInstructionWord instructions[], VTPInstructionV1 out[], size_t n) {
    size_t i;
    VTPError err;

    for (i = 0; i < n; i++) {
        if ((err = vtp_decode_instruction_v1(instructions[i], out + i)) != VTP_OK) {
            return err;
        }
    }

    return VTP_OK;
}

VTPError vtp_encode_instructions_v1(const VTPInstructionV1 instructions[], VTPInstructionWord out[], size_t n) {
    size_t i;
    VTPError err;

    for (i = 0; i < n; i++) {
        if ((err = vtp_encode_instruction_v1(instructions + i, out + i)) != VTP_OK) {
            return err;
        }
    }

    return VTP_OK;
}

unsigned long vtp_get_time_offset_v1(const VTPInstructionV1* instruction) {
    switch (instruction->code) {
        case VTP_INST_INCREMENT_TIME:
            return instruction->params.format_a.parameter_a;
        case VTP_INST_SET_AMPLITUDE:
        case VTP_INST_SET_FREQUENCY:
            return instruction->params.format_b.time_offset;
        default:
            return 0;
    }
}

void vtp_read_instruction_words(size_t n_words, const unsigned char in[], VTPInstructionWord out[]) {
    size_t i;

    for (i=0; i < n_words; i++) {
        out[i] = ((unsigned long)in[i*4] << 24u) | ((unsigned long)in[i*4+1] << 16u) | ((unsigned long)in[i*4+2] << 8u) | in[i*4+3];
    }
}

void vtp_write_instruction_words(size_t n_words, const VTPInstructionWord in[], unsigned char out[]) {
    size_t i;
    VTPInstructionWord instruction;

    for (i=0; i < n_words; i++) {
        instruction = in[i];

        out[i*4]   = (instruction >> 24u) & 0xFFu;
        out[i*4+1] = (instruction >> 16u) & 0xFFu;
        out[i*4+2] = (instruction >> 8u) & 0xFFu;
        out[i*4+3] = instruction & 0xFFu;
    }
}

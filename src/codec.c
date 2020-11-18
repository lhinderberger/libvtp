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
    out->channel_select = (instruction & 0x0FF00000u) >> 20u;
    out->time_offset = (instruction & 0x000FFC00u) >> 10u;
    out->parameter_a = instruction & 0x000003FFu;
}

void vtp_encode_params_a(const VTPInstructionParamsA* params, VTPInstructionWord* out) {
    *out |= ((unsigned long)params->parameter_a) & 0xFFFFFFFu;
}

void vtp_encode_params_b(const VTPInstructionParamsB* params, VTPInstructionWord* out) {
    *out |= (unsigned long)params->channel_select << 20u;
    *out |= ((unsigned long)params->time_offset & 0x3FFu) << 10u;
    *out |= ((unsigned long)params->parameter_a & 0x3FFu);
}

VTPError vtp_decode_instruction_v1(VTPInstructionWord instruction, VTPInstructionV1* out) {
    out->code = (instruction & 0xF0000000u) >> 28u;

    switch (out->code) {
        case 0:
            vtp_decode_params_a(instruction, &out->params.format_a);
            break;
        case 1:
        case 2:
            vtp_decode_params_b(instruction, &out->params.format_b);
            break;
        default:
            return VTP_INVALID_INSTRUCTION_CODE;
    }

    return VTP_OK;
}

VTPError vtp_encode_instruction_v1(const VTPInstructionV1* instruction, VTPInstructionWord* out) {
    *out = (instruction->code << 28u);

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

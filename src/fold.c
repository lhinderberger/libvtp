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

#include <vtp/fold.h>

VTPError apply_fold_format_b(const VTPInstructionParamsB* parameters, VTPAccumulatorV1* accumulator, unsigned int* target);
VTPError set_with_channel_select(unsigned int new_value, unsigned char channel_select, unsigned int* target, unsigned char n_channels);


VTPError vtp_fold_v1(VTPAccumulatorV1* accumulator, const VTPInstructionV1 instructions[], size_t n_instructions) {
    size_t i;
    VTPError err;

    for (i=0; i < n_instructions; i++) {
        if ((err = vtp_fold_single_v1(accumulator, instructions + i)) != VTP_OK) {
            return err;
        }
    }

    return VTP_OK;
}

VTPError vtp_fold_single_v1(VTPAccumulatorV1* accumulator, const VTPInstructionV1* instruction) {
    switch (instruction->code) {
        case VTP_INST_INCREMENT_TIME:
            accumulator->milliseconds_elapsed += instruction->params.format_a.parameter_a;
            return VTP_OK;
        case VTP_INST_SET_FREQUENCY:
            return apply_fold_format_b(&instruction->params.format_b, accumulator, accumulator->frequencies);
        case VTP_INST_SET_AMPLITUDE:
            return apply_fold_format_b(&instruction->params.format_b, accumulator, accumulator->amplitudes);
        default:
            return VTP_INVALID_INSTRUCTION_CODE;
    }
}

VTPError vtp_fold_until_v1(VTPAccumulatorV1* accumulator, const VTPInstructionV1 instructions[], size_t n_instructions, unsigned long until_ms, size_t* n_processed) {
    VTPError err;

    if (n_processed)
        *n_processed = 0;

    while (n_instructions > 0 && (accumulator->milliseconds_elapsed + vtp_get_time_offset_v1(instructions)) <= until_ms) {
        if ((err = vtp_fold_single_v1(accumulator, instructions)) != VTP_OK) {
            return err;
        }

        n_instructions--;
        instructions++;

        if (n_processed)
            *n_processed += 1;
    }

    return VTP_OK;
}


VTPError apply_fold_format_b(const VTPInstructionParamsB* parameters, VTPAccumulatorV1* accumulator, unsigned int* target) {
    accumulator->milliseconds_elapsed += parameters->time_offset;
    return set_with_channel_select(parameters->parameter_a, parameters->channel_select, target, accumulator->n_channels);
}

VTPError set_with_channel_select(unsigned int new_value, unsigned char channel_select, unsigned int* target, unsigned char n_channels) {
    unsigned char i;

    if (channel_select == 0) {
        for (i=0; i < n_channels; i++) {
            target[i] = new_value;
        }
    }
    else {
        if (channel_select > n_channels)
            return VTP_CHANNEL_OUT_OF_RANGE;

        target[channel_select - 1] = new_value;
    }

    return VTP_OK;
}

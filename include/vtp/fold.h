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

#ifndef LIBVTP_FOLD_H
#define LIBVTP_FOLD_H

#include <vtp/codec.h>
#include <vtp/instruction_types.h>

struct sVTPAccumulatorV1 {
    unsigned char n_channels;
    unsigned int* amplitudes;
    unsigned int* frequencies;
    unsigned long milliseconds_elapsed;
};
typedef struct sVTPAccumulatorV1 VTPAccumulatorV1;

VTPError vtp_fold_v1(VTPAccumulatorV1* accumulator, const VTPInstructionV1* instructions, size_t n_instructions);
VTPError vtp_fold_single_v1(VTPAccumulatorV1* accumulator, const VTPInstructionV1* instruction);
VTPError vtp_fold_until_v1(VTPAccumulatorV1* accumulator, const VTPInstructionV1* instructions, size_t n_instructions, unsigned long until_ms, size_t* n_processed);

#endif

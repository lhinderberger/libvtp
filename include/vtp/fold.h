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

/**
 * This structure represents the state of a vibrotactile display
 * after an arbitrary number of VTP instructions has been applied to it, for
 * the purpose of the fold algorithm
 */
struct sVTPAccumulatorV1 {
    /** The number of channels the display supports. Also the length of both the amplitudes and frequencies fields */
    unsigned char n_channels;

    /** The amplitude of each channel, indexed by channel number - 1 */
    unsigned int* amplitudes;

    /** The frequency of each channel, indexed by channel number - 1 */
    unsigned int* frequencies;

    /** Keeps track of time when applying VTP instructions during fold */
    unsigned long milliseconds_elapsed;
};
typedef struct sVTPAccumulatorV1 VTPAccumulatorV1;

/**
 * Applies each given VTPv1 instruction to the accumulator, one after another
 *
 * @param accumulator An accumulator structure. Make sure you initialize all of its fields, if you pass it into this function for the first time.
 * @param instructions An array of VTPv1 instructions that is to be applied to the accumulator.
 * @param n_instructions The number of instructions given in the instructions array.
 * @return VTP_OK on success, otherwise an error code as defined in vtp/error.h
 */
VTPError vtp_fold_v1(VTPAccumulatorV1* accumulator, const VTPInstructionV1 instructions[], size_t n_instructions);

/**
 * Applies a single VTPv1 instruction to the accumulator
 *
 * @param accumulator @see vtp_fold_v1
 * @param instruction The VTPv1 instruction to be applied to the accumulator.
 * @return VTP_OK on success, otherwise an error code as defined in vtp/error.h
 */
VTPError vtp_fold_single_v1(VTPAccumulatorV1* accumulator, const VTPInstructionV1* instruction);

/**
 * Folds VTPv1 instructions, up until the given target time
 *
 * This function will apply as many VTPv1 instructions from the given input as possible, until
 * the given target time (milliseconds_elapsed of accumulator) would be exceeded by applying
 * the next instruction.
 *
 * It is useful for example for simulation purposes or for mapping VTP to a sampling-like interface,
 * by calling this function with uniformly increasing target times.
 *
 * @param accumulator @see vtp_fold_v1
 * @param instructions @see vtp_fold_v1
 * @param n_instructions @see vtp_fold_v1
 * @param until_ms The target time in milliseconds.
 * @param n_processed Returns the count of instructions that actually have been applied to the accumulator.
 * @return VTP_OK on success, otherwise an error code as defined in vtp/error.h
 */
VTPError vtp_fold_until_v1(VTPAccumulatorV1* accumulator, const VTPInstructionV1 instructions[], size_t n_instructions, unsigned long until_ms, size_t* n_processed);

#endif

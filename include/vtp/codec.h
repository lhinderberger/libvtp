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

#ifndef LIBVTP_CODEC_H
#define LIBVTP_CODEC_H

#include <stddef.h>
#include <vtp/error.h>
#include <vtp/instruction_types.h>

/**
 * Decodes a VTPv1 binary instruction word
 *
 * @param instruction The instruction word to decode
 * @param out A structure to write the decoded instruction to
 * @return VTP_OK on success, otherwise an error code as defined in vtp/error.h
 */
VTPError vtp_decode_instruction_v1(VTPInstructionWord instruction, VTPInstructionV1* out);

/**
 * Encodes a VTPv1 instruction into a binary instruction word
 *
 * @param instruction A structure that describes the instruction that is to be encoded
 * @param out A variable to write the encoded instruction word to
 * @return VTP_OK on success, otherwise an error code as defined in vtp/error.h
 */
VTPError vtp_encode_instruction_v1(const VTPInstructionV1* instruction, VTPInstructionWord* out);


/**
 * Decodes multiple VTPv1 binary instruction words
 *
 * @param instructions @see vtp_decode_instruction_v1
 * @param out @see vtp_decode_instruction_v1
 * @param n The number of instruction words to be decoded. Also the number of slots in the output array.
 * @return VTP_OK on success, otherwise an error code as defined in vtp/error.h
 */
VTPError vtp_decode_instructions_v1(const VTPInstructionWord instructions[], VTPInstructionV1 out[], size_t n);

/**
 * Encodes multiple VTPv1 instructions into binary instruction words
 *
 * @param instructions @see vtp_encode_instruction_v1
 * @param out @see vtp_encode_instruction_v1
 * @param n The number of instruction words to be decoded. Also the number of slots in the output array.
 * @return VTP_OK on success, otherwise an error code as defined in vtp/error.h
 */
VTPError vtp_encode_instructions_v1(const VTPInstructionV1 instructions[], VTPInstructionWord out[], size_t n);


/**
 * Reads VTP Binary instruction words from a byte array
 *
 * VTP Binary files are specified to be big-endian. This function can help correctly reading from VTP Binary files.
 *
 * @param n_words The number of words to be read.
 * @param in A byte array containing VTP Binary instruction words, as big endian. Note that the size of this must be at least 4*n_words.
 * @param out The instruction word array to write the result to. Note that the size of this must be at least n_words.
 */
void vtp_read_instruction_words(size_t n_words, const unsigned char in[], VTPInstructionWord out[]);

/**
 * Writes VTP Binary instruction words to a byte array
 *
 * VTP Binary files are specified to be big-endian. This function can help correctly writing to VTP Binary files.
 *
 * @param n_words The number of words that are to be written.
 * @param in An array containing the instruction words that are to be written to the byte array.
 * @param out The byte array to write to. Note that the size of this must be at least 4*n_words.
 */
void vtp_write_instruction_words(size_t n_words, const VTPInstructionWord in[], unsigned char out[]);


/**
 * Calculates the time offset of a given VTPv1 instruction
 *
 * @param instruction The instruction of which the time offset shall be calculated
 * @return The instruction's time offset in milliseconds. If the instruction doesn't support an offset, this returns 0.
 */
unsigned long vtp_get_time_offset_v1(const VTPInstructionV1* instruction);

#endif

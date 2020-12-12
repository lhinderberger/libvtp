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

#include "../vendor/greatest/greatest.h"

#include <vtp/codec.h>


const unsigned char readWriteTestBytes[32] = {
0x10, 0x00, 0x00, 0xEA, 0x20, 0x00, 0x00, 0x7B, 0x10, 0x20, 0x01, 0x59,
0x10, 0x20, 0xC9, 0xC8, 0x10, 0x10, 0x03, 0x15, 0x00, 0x00, 0x07, 0xD0,
0x20, 0x00, 0x00, 0xEA, 0x10, 0x20, 0x02, 0x37
};

const VTPInstructionWord readWriteTestWords[8] = {
0x100000ea, 0x2000007b, 0x10200159, 0x1020c9c8,
0x10100315, 0x000007d0, 0x200000ea, 0x10200237
};


TEST increment_time_can_be_decoded(void) {
    const VTPInstructionWord encoded = 0x056789AB;
    VTPInstructionV1 decoded;

    ASSERT_EQ(VTP_OK, vtp_decode_instruction_v1(encoded, &decoded));
    ASSERT_EQ(VTP_INST_INCREMENT_TIME, decoded.code);
    ASSERT_EQ(0x56789AB, decoded.params.format_a.parameter_a);

    PASS();
}

TEST increment_time_can_be_encoded(void) {
    VTPInstructionV1 instruction;
    VTPInstructionWord encoded;

    instruction.code = VTP_INST_INCREMENT_TIME;
    instruction.params.format_a.parameter_a = 0xCCCCCCD;

    ASSERT_EQ(VTP_OK, vtp_encode_instruction_v1(&instruction, &encoded));
    ASSERT_EQ(0x0CCCCCCD, encoded);

    PASS();
}

TEST set_amplitude_can_be_decoded(void) {
    const VTPInstructionWord encoded = 0x2AACCD6B;
    VTPInstructionV1 decoded;

    ASSERT_EQ(VTP_OK, vtp_decode_instruction_v1(encoded, &decoded));
    ASSERT_EQ(VTP_INST_SET_AMPLITUDE, decoded.code);
    ASSERT_EQ(0xAA, decoded.params.format_b.channel_select);
    ASSERT_EQ(0x333, decoded.params.format_b.time_offset);
    ASSERT_EQ(0x16B, decoded.params.format_b.parameter_a);

    PASS();
}

TEST set_amplitude_can_be_encoded(void) {
    VTPInstructionV1 instruction;
    VTPInstructionWord encoded;

    instruction.code = VTP_INST_SET_AMPLITUDE;
    instruction.params.format_b.channel_select = 0xAA;
    instruction.params.format_b.time_offset = 0x333;
    instruction.params.format_b.parameter_a = 0x16B;

    ASSERT_EQ(VTP_OK, vtp_encode_instruction_v1(&instruction, &encoded));
    ASSERT_EQ(0x2AACCD6B, encoded);

    PASS();
}

TEST set_frequency_can_be_decoded(void) {
    const VTPInstructionWord encoded = 0x1AC56BBA;
    VTPInstructionV1 decoded;

    ASSERT_EQ(VTP_OK, vtp_decode_instruction_v1(encoded, &decoded));
    ASSERT_EQ(VTP_INST_SET_FREQUENCY, decoded.code);
    ASSERT_EQ(0xAC, decoded.params.format_b.channel_select);
    ASSERT_EQ(0x15A, decoded.params.format_b.time_offset);
    ASSERT_EQ(0x3BA, decoded.params.format_b.parameter_a);

    PASS();
}

TEST set_frequency_can_be_encoded(void) {
    VTPInstructionV1 instruction;
    VTPInstructionWord encoded;

    instruction.code = VTP_INST_SET_FREQUENCY;
    instruction.params.format_b.channel_select = 0xAC;
    instruction.params.format_b.time_offset = 0x15A;
    instruction.params.format_b.parameter_a = 0x3BA;

    ASSERT_EQ(VTP_OK, vtp_encode_instruction_v1(&instruction, &encoded));
    ASSERT_EQ(0x1AC56BBA, encoded);

    PASS();
}

TEST invalid_instruction_code_yields_error(void) {
    const VTPInstructionWord invalid_instruction_word = 0xFE000000;
    VTPInstructionV1 invalid_instruction, temp_instruction;
    VTPInstructionWord temp_instruction_word;

    invalid_instruction.code = (VTPInstructionCode)0xFE;

    ASSERT_EQ(VTP_INVALID_INSTRUCTION_CODE, vtp_encode_instruction_v1(&invalid_instruction, &temp_instruction_word));
    ASSERT_EQ(VTP_INVALID_INSTRUCTION_CODE, vtp_decode_instruction_v1(invalid_instruction_word, &temp_instruction));

    PASS();
}

TEST array_can_be_decoded(void) {
    const VTPInstructionWord encoded[3] = { 0x056789AB, 0x2AACCD6B, 0x1AC56BBA };
    VTPInstructionV1 decoded[3];

    ASSERT_EQ(VTP_OK, vtp_decode_instructions_v1(encoded, decoded, 3));

    ASSERT_EQ(VTP_INST_INCREMENT_TIME, decoded[0].code);
    ASSERT_EQ(0x56789AB, decoded[0].params.format_a.parameter_a);

    ASSERT_EQ(VTP_INST_SET_AMPLITUDE, decoded[1].code);
    ASSERT_EQ(0xAA, decoded[1].params.format_b.channel_select);
    ASSERT_EQ(0x333, decoded[1].params.format_b.time_offset);
    ASSERT_EQ(0x16B, decoded[1].params.format_b.parameter_a);

    ASSERT_EQ(VTP_INST_SET_FREQUENCY, decoded[2].code);
    ASSERT_EQ(0xAC, decoded[2].params.format_b.channel_select);
    ASSERT_EQ(0x15A, decoded[2].params.format_b.time_offset);
    ASSERT_EQ(0x3BA, decoded[2].params.format_b.parameter_a);

    PASS();
}

TEST array_can_be_encoded(void) {
    VTPInstructionV1 instructions[3];
    VTPInstructionWord encoded[3];

    instructions[0].code = VTP_INST_INCREMENT_TIME;
    instructions[0].params.format_a.parameter_a = 0xCCCCCCD;

    instructions[1].code = VTP_INST_SET_AMPLITUDE;
    instructions[1].params.format_b.channel_select = 0xAA;
    instructions[1].params.format_b.time_offset = 0x333;
    instructions[1].params.format_b.parameter_a = 0x16B;

    instructions[2].code = VTP_INST_SET_FREQUENCY;
    instructions[2].params.format_b.channel_select = 0xAC;
    instructions[2].params.format_b.time_offset = 0x15A;
    instructions[2].params.format_b.parameter_a = 0x3BA;

    ASSERT_EQ(VTP_OK, vtp_encode_instructions_v1(instructions, encoded, 3));
    ASSERT_EQ(0x0CCCCCCD, encoded[0]);
    ASSERT_EQ(0x2AACCD6B, encoded[1]);
    ASSERT_EQ(0x1AC56BBA, encoded[2]);

    PASS();
}

TEST instruction_words_can_be_read_from_bytes(void) {
    VTPInstructionWord wordsRead[8];

    vtp_read_instruction_words(8, readWriteTestBytes, wordsRead);
    ASSERT_MEM_EQ(readWriteTestWords, wordsRead, 8*sizeof(VTPInstructionWord));

    PASS();
}

TEST instruction_words_can_be_written_to_bytes(void) {
    unsigned char bytesWritten[32];

    vtp_write_instruction_words(8, readWriteTestWords, bytesWritten);
    ASSERT_MEM_EQ(readWriteTestBytes, bytesWritten, 32);

    PASS();
}




GREATEST_SUITE(codec_suite) {
    RUN_TEST(increment_time_can_be_decoded);
    RUN_TEST(increment_time_can_be_encoded);
    RUN_TEST(set_amplitude_can_be_decoded);
    RUN_TEST(set_amplitude_can_be_encoded);
    RUN_TEST(set_frequency_can_be_decoded);
    RUN_TEST(set_frequency_can_be_encoded);
    RUN_TEST(invalid_instruction_code_yields_error);
    RUN_TEST(array_can_be_decoded);
    RUN_TEST(array_can_be_encoded);
    RUN_TEST(instruction_words_can_be_read_from_bytes);
    RUN_TEST(instruction_words_can_be_written_to_bytes);
}

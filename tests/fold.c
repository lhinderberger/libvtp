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

#include <vtp/fold.h>


#define N_TEST_INSTRUCTIONS (8)

#define DECLARE_TEST \
    VTPAccumulatorV1 accumulator; \
    VTPInstructionV1 instructions[N_TEST_INSTRUCTIONS]; \
    unsigned int amplitudes[3], frequencies[3];

#define PREPARE_TEST \
    if (vtp_decode_instructions_v1(testdata_words, instructions, N_TEST_INSTRUCTIONS) != VTP_OK) { \
        fputs("Test data broken\n", stderr); \
        exit(-1); \
    } \
     \
    accumulator.amplitudes = amplitudes; \
    accumulator.frequencies = frequencies; \
    accumulator.milliseconds_elapsed = 0; \
    accumulator.n_channels = 3; \

/*
 * Corresponding VTP Assembly Code:
 *
 * freq ch* 234
 * amp ch* 123
 * freq ch2 345
 *
 * freq +50ms ch2 456
 * freq ch1 789
 *
 * time +2000ms
 * amp ch* 234
 * freq ch2 567
 */
const VTPInstructionWord testdata_words[N_TEST_INSTRUCTIONS] = {
    0x100000ea, 0x2000007b, 0x10200159, 0x1020c9c8,
    0x10100315, 0x000007d0, 0x200000ea, 0x10200237
};


TEST fold_yields_expected_accumulation(void) {
    DECLARE_TEST
    PREPARE_TEST

    ASSERT_EQ(VTP_OK, vtp_fold_v1(&accumulator, instructions, N_TEST_INSTRUCTIONS));

    ASSERT_EQ(234, accumulator.amplitudes[0]);
    ASSERT_EQ(234, accumulator.amplitudes[1]);
    ASSERT_EQ(234, accumulator.amplitudes[2]);
    ASSERT_EQ(789, accumulator.frequencies[0]);
    ASSERT_EQ(567, accumulator.frequencies[1]);
    ASSERT_EQ(234, accumulator.frequencies[2]);

    ASSERT_EQ(2050, accumulator.milliseconds_elapsed);

    PASS();
}

TEST fold_until_stops_at_the_right_time(void) {
    DECLARE_TEST
    size_t n_processed;

    PREPARE_TEST

    ASSERT_EQ(VTP_OK, vtp_fold_until_v1(&accumulator, instructions, N_TEST_INSTRUCTIONS, 0, &n_processed));
    ASSERT_EQ(3, n_processed);
    ASSERT_EQ(123, accumulator.amplitudes[0]);
    ASSERT_EQ(123, accumulator.amplitudes[1]);
    ASSERT_EQ(123, accumulator.amplitudes[2]);
    ASSERT_EQ(234, accumulator.frequencies[0]);
    ASSERT_EQ(345, accumulator.frequencies[1]);
    ASSERT_EQ(234, accumulator.frequencies[2]);
    ASSERT_EQ(0, accumulator.milliseconds_elapsed);

    ASSERT_EQ(VTP_OK, vtp_fold_until_v1(&accumulator, instructions + 3, N_TEST_INSTRUCTIONS - 3, 10, &n_processed));
    ASSERT_EQ(0, n_processed);
    ASSERT_EQ(123, accumulator.amplitudes[0]);
    ASSERT_EQ(123, accumulator.amplitudes[1]);
    ASSERT_EQ(123, accumulator.amplitudes[2]);
    ASSERT_EQ(234, accumulator.frequencies[0]);
    ASSERT_EQ(345, accumulator.frequencies[1]);
    ASSERT_EQ(234, accumulator.frequencies[2]);
    ASSERT_EQ(0, accumulator.milliseconds_elapsed);

    ASSERT_EQ(VTP_OK, vtp_fold_until_v1(&accumulator, instructions + 3, N_TEST_INSTRUCTIONS - 3, 50, &n_processed));
    ASSERT_EQ(2, n_processed);
    ASSERT_EQ(123, accumulator.amplitudes[0]);
    ASSERT_EQ(123, accumulator.amplitudes[1]);
    ASSERT_EQ(123, accumulator.amplitudes[2]);
    ASSERT_EQ(789, accumulator.frequencies[0]);
    ASSERT_EQ(456, accumulator.frequencies[1]);
    ASSERT_EQ(234, accumulator.frequencies[2]);
    ASSERT_EQ(50, accumulator.milliseconds_elapsed);

    ASSERT_EQ(VTP_OK, vtp_fold_until_v1(&accumulator, instructions + 5, N_TEST_INSTRUCTIONS - 5, 2050, &n_processed));
    ASSERT_EQ(3, n_processed);
    ASSERT_EQ(234, accumulator.amplitudes[0]);
    ASSERT_EQ(234, accumulator.amplitudes[1]);
    ASSERT_EQ(234, accumulator.amplitudes[2]);
    ASSERT_EQ(789, accumulator.frequencies[0]);
    ASSERT_EQ(567, accumulator.frequencies[1]);
    ASSERT_EQ(234, accumulator.frequencies[2]);
    ASSERT_EQ(2050, accumulator.milliseconds_elapsed);

    ASSERT_EQ(VTP_OK, vtp_fold_until_v1(&accumulator, instructions + 8, N_TEST_INSTRUCTIONS - 8, 231195, &n_processed));
    ASSERT_EQ(0, n_processed);
    ASSERT_EQ(234, accumulator.amplitudes[0]);
    ASSERT_EQ(234, accumulator.amplitudes[1]);
    ASSERT_EQ(234, accumulator.amplitudes[2]);
    ASSERT_EQ(789, accumulator.frequencies[0]);
    ASSERT_EQ(567, accumulator.frequencies[1]);
    ASSERT_EQ(234, accumulator.frequencies[2]);
    ASSERT_EQ(2050, accumulator.milliseconds_elapsed);

    PASS();
}

TEST fold_until_past_does_nothing(void) {
    DECLARE_TEST
    size_t n_processed;

    PREPARE_TEST

    ASSERT_EQ(VTP_OK, vtp_fold_until_v1(&accumulator, instructions, N_TEST_INSTRUCTIONS, 0, &n_processed));
    ASSERT_EQ(VTP_OK, vtp_fold_until_v1(&accumulator, instructions, N_TEST_INSTRUCTIONS, 50, &n_processed));
    ASSERT_EQ(VTP_OK, vtp_fold_until_v1(&accumulator, instructions, N_TEST_INSTRUCTIONS, 5, &n_processed));

    ASSERT_EQ(123, accumulator.amplitudes[0]);
    ASSERT_EQ(123, accumulator.amplitudes[1]);
    ASSERT_EQ(123, accumulator.amplitudes[2]);
    ASSERT_EQ(789, accumulator.frequencies[0]);
    ASSERT_EQ(456, accumulator.frequencies[1]);
    ASSERT_EQ(234, accumulator.frequencies[2]);
    ASSERT_EQ(50, accumulator.milliseconds_elapsed);

    PASS();
}

TEST fold_with_invalid_instruction_code_yields_error(void) {
    DECLARE_TEST
    PREPARE_TEST

    instructions[2].code = 0xAB;

    ASSERT_EQ(VTP_INVALID_INSTRUCTION_CODE, vtp_fold_v1(&accumulator, instructions, 3));

    PASS();
}

TEST fold_with_no_instructions_does_nothing(void) {
    DECLARE_TEST
    PREPARE_TEST

    accumulator.frequencies[0] = accumulator.frequencies[1] = accumulator.frequencies[2] = 2311;
    accumulator.amplitudes[0] = accumulator.amplitudes[1] = accumulator.amplitudes[2] = 1995;

    ASSERT_EQ(VTP_OK, vtp_fold_v1(&accumulator, instructions, 0));

    ASSERT_EQ(2311, frequencies[0]);
    ASSERT_EQ(2311, frequencies[1]);
    ASSERT_EQ(2311, frequencies[2]);
    ASSERT_EQ(1995, amplitudes[0]);
    ASSERT_EQ(1995, amplitudes[1]);
    ASSERT_EQ(1995, amplitudes[2]);
    ASSERT_EQ(0, accumulator.milliseconds_elapsed);

    PASS();
}

TEST fold_with_out_of_range_channel_yields_error(void) {
    DECLARE_TEST
    PREPARE_TEST

    instructions[2].params.format_b.channel_select = 23;

    ASSERT_EQ(VTP_CHANNEL_OUT_OF_RANGE, vtp_fold_v1(&accumulator, instructions, 3));

    PASS();
}

GREATEST_SUITE(fold_suite) {
    RUN_TEST(fold_yields_expected_accumulation);
    RUN_TEST(fold_until_stops_at_the_right_time);
    RUN_TEST(fold_until_past_does_nothing);
    RUN_TEST(fold_with_invalid_instruction_code_yields_error);
    RUN_TEST(fold_with_no_instructions_does_nothing);
    RUN_TEST(fold_with_out_of_range_channel_yields_error);
}

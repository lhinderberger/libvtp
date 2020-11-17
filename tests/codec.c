#include "../vendor/greatest/greatest.h"

TEST increment_time_can_be_decoded(void) {
    FAIL();
}

TEST increment_time_can_be_encoded(void) {
    FAIL();
}

TEST set_amplitude_can_be_decoded(void) {
    FAIL();
}

TEST set_amplitude_can_be_encoded(void) {
    FAIL();
}

TEST set_frequency_can_be_decoded(void) {
    FAIL();
}

TEST set_frequency_can_be_encoded(void) {
    FAIL();
}

TEST invalid_instruction_code_yields_error(void) {
    FAIL();
}

GREATEST_SUITE(codec_suite) {
    RUN_TEST(increment_time_can_be_decoded);
    RUN_TEST(increment_time_can_be_encoded);
    RUN_TEST(set_amplitude_can_be_decoded);
    RUN_TEST(set_amplitude_can_be_encoded);
    RUN_TEST(set_frequency_can_be_decoded);
    RUN_TEST(set_frequency_can_be_encoded);
    RUN_TEST(invalid_instruction_code_yields_error);
}

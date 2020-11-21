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

TEST fold_yields_expected_accumulation(void) {
    FAIL();
}

TEST fold_until_stops_at_the_right_time(void) {
    FAIL();
}

TEST fold_until_past_does_nothing(void) {
    FAIL();
}

TEST fold_with_no_instructions_does_nothing(void) {
    FAIL();
}

GREATEST_SUITE(fold_suite) {
    RUN_TEST(fold_yields_expected_accumulation);
    RUN_TEST(fold_until_stops_at_the_right_time);
    RUN_TEST(fold_until_past_does_nothing);
    RUN_TEST(fold_with_no_instructions_does_nothing);
}

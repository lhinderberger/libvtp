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

GREATEST_MAIN_DEFS();

GREATEST_SUITE_EXTERN(codec_suite);
GREATEST_SUITE_EXTERN(fold_suite);

int main(int argc, char ** argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(codec_suite);
    RUN_SUITE(fold_suite);
    GREATEST_MAIN_END();
}

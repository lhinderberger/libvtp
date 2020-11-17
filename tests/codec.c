#include "../vendor/greatest/greatest.h"

TEST foo(void) {
    FAIL();
}

TEST bar(void) {
    PASS();
}

GREATEST_SUITE(codec_suite) {
    RUN_TEST(foo);
    RUN_TEST(bar);
}

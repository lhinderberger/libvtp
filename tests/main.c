#include "../vendor/greatest/greatest.h"

GREATEST_MAIN_DEFS();

GREATEST_SUITE_EXTERN(codec_suite);

int main(int argc, char ** argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(codec_suite);
    GREATEST_MAIN_END();
}

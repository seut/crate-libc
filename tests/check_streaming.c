#include <check.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#include "../src/streaming.h"


START_TEST(test_stream_byte)
{
    uint16_t bufferSize = 1;
    char buffer[bufferSize];
    StreamOutput *out = StreamOutput_alloc(bufferSize);
    StreamInput *in = StreamInput_alloc(out->buffer, bufferSize);

    char value = 0xC;

    StreamOutput_writeByte(out, value);
    char valueRead = StreamInput_readByte(in);

    if (valueRead != value) {
        ck_abort_msg("Written and read byte are not the same, read value: %c", valueRead);
    }

}
END_TEST

START_TEST(test_stream_int)
{
    uint16_t bufferSize = 4;
    char buffer[bufferSize];
    StreamOutput *out = StreamOutput_alloc(bufferSize);
    StreamInput *in = StreamInput_alloc(out->buffer, bufferSize);

    int32_t value = 12;

    StreamOutput_writeInt(out, value);
    int32_t valueRead = StreamInput_readInt(in);

    if (valueRead != value) {
        ck_abort_msg("Written and read integer are not the same, read value: %d", valueRead);
    }

}
END_TEST

START_TEST(test_stream_vint)
{
    uint16_t bufferSize = 5;
    char buffer[bufferSize];
    StreamOutput *out = StreamOutput_alloc(bufferSize);
    StreamInput *in = StreamInput_alloc(out->buffer, bufferSize);

    int32_t value = INT_MAX;

    StreamOutput_writeVInt(out, value);
    int32_t valueRead = StreamInput_readVInt(in);

    if (valueRead != value) {
        ck_abort_msg("Written and read variable integer are not the same, read value: %d", valueRead);
    }

}
END_TEST

Suite * streaming_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Streaming");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_stream_byte);
    tcase_add_test(tc_core, test_stream_int);
    tcase_add_test(tc_core, test_stream_vint);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = streaming_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#include <check.h>
#include <stdlib.h>
#include <limits.h>
#include "../src/streaming.h"

START_TEST(test_stream_byte)
{
    uint16_t bufferSize = 1;
    StreamOutput *out = StreamOutput_alloc(bufferSize);

    uint8_t value = 0xC;

    StreamOutput_writeByte(out, value);

    StreamInput *in = StreamInput_alloc(out->buffer, out->bufferSize);
    uint8_t valueRead = StreamInput_readByte(in);

    if (valueRead != value) {
        ck_abort_msg("Written and read byte are not equal, read value: %c, expected: %c", valueRead, value);
    }

    StreamOutput_free(out);
    StreamInput_free(in);
}
END_TEST

START_TEST(test_stream_int)
{
    uint16_t bufferSize = 4;
    StreamOutput *out = StreamOutput_alloc(bufferSize);

    int32_t value = 12;

    StreamOutput_writeInt(out, value);

    StreamInput *in = StreamInput_alloc(out->buffer, out->bufferSize);
    int32_t valueRead = StreamInput_readInt(in);

    if (valueRead != value) {
        ck_abort_msg("Written and read integer are not equal, read value: %d, expected: %d", valueRead, value);
    }

    StreamOutput_free(out);
    StreamInput_free(in);
}
END_TEST

START_TEST(test_stream_vint)
{
    uint16_t bufferSize = 5;
    StreamOutput *out = StreamOutput_alloc(bufferSize);

    int32_t value = INT_MAX;

    StreamOutput_writeVInt(out, value);

    StreamInput *in = StreamInput_alloc(out->buffer, out->bufferSize);
    int32_t valueRead = StreamInput_readVInt(in);

    if (valueRead != value) {
        ck_abort_msg("Written and read variable integer are not equal, read value: %d, expected: %d", valueRead, value);
    }

    StreamOutput_free(out);
    StreamInput_free(in);
}
END_TEST

START_TEST(test_stream_long)
{
    uint16_t bufferSize = 4;
    StreamOutput *out = StreamOutput_alloc(bufferSize);

    int64_t value = UINT_MAX * 2;

    StreamOutput_writeLong(out, value);

    StreamInput *in = StreamInput_alloc(out->buffer, out->bufferSize);
    int64_t valueRead = StreamInput_readLong(in);

    if (valueRead != value) {
        ck_abort_msg("Written and read long are not equal, read value: %lld, expected %lld", valueRead, value);
    }

    StreamOutput_free(out);
    StreamInput_free(in);
}
END_TEST

START_TEST(test_stream_vlong)
{
    uint16_t bufferSize = 9;
    StreamOutput *out = StreamOutput_alloc(bufferSize);

    int64_t value = LONG_MAX;

    StreamOutput_writeVLong(out, value);

    StreamInput *in = StreamInput_alloc(out->buffer, out->bufferSize);
    int64_t valueRead = StreamInput_readVLong(in);

    if (valueRead != value) {
        ck_abort_msg("Written and read variable long are not equal, read value: %lld, expected: %lld", valueRead, value);
    }

    StreamOutput_free(out);
    StreamInput_free(in);
}
END_TEST

START_TEST(test_stream_string)
{
    uint16_t bufferSize = 8;
    StreamOutput *out = StreamOutput_alloc(bufferSize);

    uint8_t value[] = "C programming is a lot of copy and paste";
    uint32_t stringLength = (uint32_t) strlen((const char *) value);

    StreamOutput_writeString(out, value, stringLength);

    StreamInput *in = StreamInput_alloc(out->buffer, out->bufferSize);
    uint8_t * valueRead = StreamInput_readString(in);

    if (strncmp((const char *) value, (const char *) valueRead, stringLength) != 0) {
        ck_abort_msg("Written and read strings are not equal, read value: %s, expected: %s", valueRead, value);
    }

    StreamOutput_free(out);
    StreamInput_free(in);
}
END_TEST

START_TEST(test_stream_payload)
{
    uint16_t bufferSize = 1;
    StreamOutput *out = StreamOutput_alloc(bufferSize);

    uint8_t value1 = 0xC;
    int32_t value2 = 12;
    int64_t value3 = LONG_MAX;

    StreamOutput_writeByte(out, value1);
    StreamOutput_writeInt(out, value2);
    StreamOutput_writeVLong(out, value3);

    StreamInput *in = StreamInput_alloc(out->buffer, out->bufferSize);

    uint8_t value1Read = StreamInput_readByte(in);
    if (value1Read != value1) {
        ck_abort_msg("Written and read value1 are not equal, read value: %c, expected: %c", value1Read, value1);
    }
    int32_t value2Read = StreamInput_readInt(in);
    if (value2Read != value2) {
        ck_abort_msg("Written and read value2 are not equal, read value: %d, expected: %d", value2Read, value2);
    }
    int64_t value3Read = StreamInput_readVLong(in);
    if (value3Read != value3) {
        ck_abort_msg("Written and read value3 are not equal, read value: %lld, expected: %lld", value3Read, value3);
    }

    StreamOutput_free(out);
    StreamInput_free(in);
}
END_TEST

Suite * streaming_suite(void)
{
    Suite *s;
    TCase *tc_dataTypes;
    TCase *tc_payload;

    s = suite_create("Streaming");

    /* DataTypes test case */
    tc_dataTypes = tcase_create("DataTypes");

    tcase_add_test(tc_dataTypes, test_stream_byte);
    tcase_add_test(tc_dataTypes, test_stream_int);
    tcase_add_test(tc_dataTypes, test_stream_vint);
    tcase_add_test(tc_dataTypes, test_stream_long);
    tcase_add_test(tc_dataTypes, test_stream_vlong);
    tcase_add_test(tc_dataTypes, test_stream_string);
    suite_add_tcase(s, tc_dataTypes);

    /* Payload test case */
    tc_payload = tcase_create("Payload");
    tcase_add_test(tc_payload, test_stream_payload);
    suite_add_tcase(s, tc_payload);

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

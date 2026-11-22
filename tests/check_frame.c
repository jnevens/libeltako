#include <check.h>
#include <stdlib.h>
#include <stdint.h>

#include <libeltako/frame.h>
#include <libeltako/version.h>

START_TEST(test_frame_create_and_parse)
{
	uint8_t data[4] = { 0x09, 0x00, 0x64, 0x02 };
	eltako_frame_t *frame;
	uint8_t *raw;

	frame = eltako_frame_create(0x05, data, 0xeeeeee00, 0x30);
	ck_assert_ptr_nonnull(frame);
	ck_assert_uint_eq(eltako_frame_get_raw_size(frame), 14);

	raw = eltako_frame_get_data(frame);
	ck_assert_uint_eq(raw[0], 0xA5);
	ck_assert_uint_eq(raw[1], 0x5A);
	ck_assert_uint_eq(raw[3], 0x05);
	ck_assert_uint_eq(raw[12], 0x30);
	ck_assert_uint_eq(raw[13], eltako_frame_calculate_crc(frame));
	ck_assert_int_eq(eltako_frame_parse(raw, 14), FRAME_PARSE_STATE_COMPLETE);
	ck_assert_int_eq(eltako_frame_parse(raw, 4), FRAME_PARSE_STATE_INCOMPLETE);
	ck_assert_int_eq(eltako_frame_parse(NULL, 0), FRAME_PARSE_STATE_ERROR);

	eltako_frame_destroy(frame);
	ck_assert_str_eq(eltako_version(), ELTAKO_VERSION);
}
END_TEST

static Suite *frame_suite(void)
{
	Suite *s = suite_create("frame");
	TCase *tc = tcase_create("core");

	tcase_add_test(tc, test_frame_create_and_parse);
	suite_add_tcase(s, tc);
	return s;
}

int main(void)
{
	int failed;
	SRunner *sr = srunner_create(frame_suite());

	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

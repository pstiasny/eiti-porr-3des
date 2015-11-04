#include <stdlib.h>
#include <check.h>

#include "des.h"

START_TEST (test_des_encrypt_block)
{
    KS ks;
    uint64_t out;

    build_KS(0x0123456789ABCDEFL, &ks);

    out = des_encrypt_block(0x5468652071756663L, ks);
    ck_assert_uint_eq(out, 0xA28E91724C4BBA31L);

    out = des_encrypt_block(0x6B2062726F776E20L, ks);
    ck_assert_uint_eq(out, 0x167E47EC24F71D63L);

    out = des_encrypt_block(0x666F78206A756D70L, ks);
    ck_assert_uint_eq(out, 0x2C1A917234425365L);
}
END_TEST

START_TEST (test_des_decrypt_block)
{
    KS ks;
    uint64_t out;

    build_KS(0x23456789ABCDEF01L, &ks);

    out = des_encrypt_block(0xA28E91724C4BBA31L, ks);
    ck_assert_uint_eq(out, 0x5A2EA7F983A2F53FL);

    out = des_encrypt_block(0x167E47EC24F71D63L, ks);
    ck_assert_uint_eq(out, 0xEA141A7DD69701F0L);

    out = des_encrypt_block(0x2C1A917234425365L, ks);
    ck_assert_uint_eq(out, 0x8059EE8212E22A79L);
}
END_TEST

START_TEST (test_3des_encrypt_block)
{
    KS ks[3];
    uint64_t out;

    build_KS(0x0123456789ABCDEFL, &ks[0]);
    build_KS(0x23456789ABCDEF01L, &ks[1]);
    build_KS(0x456789ABCDEF0123L, &ks[2]);

    out = des3_encrypt_block(0x5468652071756663L, ks);
    ck_assert_uint_eq(out, 0xA826FD8CE53B855FL);

    out = des3_encrypt_block(0x6B2062726F776E20L, ks);
    ck_assert_uint_eq(out, 0xCCE21C8112256FE6L);

    out = des3_encrypt_block(0x666F78206A756D70L, ks);
    ck_assert_uint_eq(out, 0x68D5C05DD9B6B900L);
}
END_TEST

START_TEST (test_3des_decrypt_block)
{
    KS ks[3];
    uint64_t out;

    build_KS(0x0123456789ABCDEFL, &ks[0]);
    build_KS(0x23456789ABCDEF01L, &ks[1]);
    build_KS(0x456789ABCDEF0123L, &ks[2]);

    out = des3_decrypt_block(0xA826FD8CE53B855FL, ks);
    ck_assert_uint_eq(out, 0x5468652071756663L);

    out = des3_decrypt_block(0xCCE21C8112256FE6L, ks);
    ck_assert_uint_eq(out, 0x6B2062726F776E20L);

    out = des3_decrypt_block(0x68D5C05DD9B6B900L, ks);
    ck_assert_uint_eq(out, 0x666F78206A756D70L);
}
END_TEST

Suite * des_suite(void)
{
    Suite *s;
    TCase *tc_api;

    s = suite_create("DES");

    tc_api = tcase_create("API");
    tcase_add_test(tc_api, test_des_encrypt_block);
    tcase_add_test(tc_api, test_des_decrypt_block);
    tcase_add_test(tc_api, test_3des_encrypt_block);
    tcase_add_test(tc_api, test_3des_decrypt_block);
    suite_add_tcase(s, tc_api);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = des_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

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

START_TEST (test_s_block)
{
    /* top left of the matrix */
    ck_assert_uint_eq(S(0b000000, S_mx[0]), 14);
    /* end of first row */
    ck_assert_uint_eq(S(0b011110, S_mx[0]), 7);
    /* end of first column */
    ck_assert_uint_eq(S(0b100001, S_mx[0]), 15);
    /* bottom right */
    ck_assert_uint_eq(S(0b111111, S_mx[0]), 13);
}
END_TEST

START_TEST (test_f_s_blocks)
{
    ck_assert_uint_eq(f_s_blocks(0x000000000000L), 0xEFA72C4D);
    ck_assert_uint_eq(f_s_blocks(0x00000000003FL), 0xEFA72C4B);
    ck_assert_uint_eq(f_s_blocks(0xFC0000000000L), 0xDFA72C4D);
    ck_assert_uint_eq(f_s_blocks(0xFFFFFFFFFFFFL), 0xD9CE3DCB);

    ck_assert_uint_eq(f_s_blocks(0x28031E58620AL), 0xFFFFFFFF);
    ck_assert_uint_eq(f_s_blocks(0x71A08869029AL), 0);
    ck_assert_uint_eq(f_s_blocks(0x71A08869028EL), 1);
}
END_TEST

START_TEST (test_f)
{
    /* trivial outputs */
    ck_assert_uint_eq(f(0, 0x71A08869029AL), 0);
    ck_assert_uint_eq(f(0, 0x28031E58620AL), 0xFFFFFFFF);
    ck_assert_uint_eq(f(0xFFFFFFFFL, 0x8E5F7796FD65L), 0);
    ck_assert_uint_eq(f(0xFFFFFFFFL, 0xD7FCE1A79DF5L), 0xFFFFFFFF);

    /* output (P) permutation:
       Bit 32 coming out of S blocks ends up as bit 21 after P.  */
    ck_assert_uint_eq(f(0, 0x71A08869028EL), 0x800);
}
END_TEST

START_TEST (test_f_E_permutation)
{
    ck_assert_uint_eq(E(0x1), 0x800000000002L);
    ck_assert_uint_eq(E(0x2), 0x4L);
    ck_assert_uint_eq(E(0x80000000LL), 0x400000000001LL);
}
END_TEST

START_TEST (test_IP)
{
    ck_assert_uint_eq(IP(0x56E99EACDE5FF4B1), 0x73F57DA2DECA3E35);
    ck_assert_uint_eq(IP(0x0123456789ABCDEF), 0xCC00CCFFF0AAF0AA);
    ck_assert_uint_eq(IP(0x29D823373C5649FB), 0xE2BA38CD829DD3AC);
}
END_TEST

START_TEST (test_IP_inv)
{
    ck_assert_uint_eq(IP_inv(0x56E99EACDE5FF4B1), 0x32E4EDB5EE1BF89F);
    ck_assert_uint_eq(IP_inv(0x0123456789ABCDEF), 0xFF330FAA00330FAA);
    ck_assert_uint_eq(IP_inv(0x29D823373C5649FB), 0x4F27A1DAB3C73A12);
}
END_TEST

START_TEST (test_rotl28)
{
    ck_assert_uint_eq(rotl28(0x7519888, 1), 0xEA33110);
    ck_assert_uint_eq(rotl28(0xD466221, 2), 0x5198887);
    ck_assert_uint_eq(rotl28(0xEA33110, 1), 0xD466221);
    ck_assert_uint_eq(rotl28(0x5198887, 2), 0x466221D);
}
END_TEST

START_TEST (test_KS)
{
    KS ks;

        printf("test ks ********\n");
    build_KS(0x4000000002000000, &ks);

    ck_assert_uint_eq(ks[0], 0x8000200000);
    ck_assert_uint_eq(ks[1], 0x800000020000);
    ck_assert_uint_eq(ks[2], 0x200000100);
    ck_assert_uint_eq(ks[3], 0x1000000000);
    ck_assert_uint_eq(ks[4], 0x40400000);
    ck_assert_uint_eq(ks[5], 0x4000000008);
    ck_assert_uint_eq(ks[6], 0x100001000);
    ck_assert_uint_eq(ks[7], 0x1000020);
    ck_assert_uint_eq(ks[8], 0x80000004000);
    ck_assert_uint_eq(ks[9], 0x8000000);
    ck_assert_uint_eq(ks[10], 0x800000);
    ck_assert_uint_eq(ks[11], 0x800000200);
    ck_assert_uint_eq(ks[12], 0x2000100000);
    ck_assert_uint_eq(ks[13], 0x400000000);
    ck_assert_uint_eq(ks[14], 0x400000002000);
    ck_assert_uint_eq(ks[15], 0x20000001);
}
END_TEST

Suite * des_suite(void)
{
    Suite *s;
    TCase *tcase;

    s = suite_create("DES");

    tcase = tcase_create("API");
    tcase_add_test(tcase, test_des_encrypt_block);
    tcase_add_test(tcase, test_des_decrypt_block);
    tcase_add_test(tcase, test_3des_encrypt_block);
    tcase_add_test(tcase, test_3des_decrypt_block);
    suite_add_tcase(s, tcase);

    tcase = tcase_create("Internals");
    tcase_add_test(tcase, test_s_block);
    tcase_add_test(tcase, test_f_s_blocks);
    tcase_add_test(tcase, test_f_E_permutation);
    tcase_add_test(tcase, test_f);
    tcase_add_test(tcase, test_IP);
    tcase_add_test(tcase, test_IP_inv);
    tcase_add_test(tcase, test_KS);
    tcase_add_test(tcase, test_rotl28);
    suite_add_tcase(s, tcase);

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

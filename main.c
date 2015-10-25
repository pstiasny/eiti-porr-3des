#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <openssl/des.h>

const char *USAGE = "Usage: %s <enc|dec> <key1> <key2> <key3>\n";

static inline int BIT(uint64_t block, int i) {
    return (block >> (64 - i)) & 1;
}

static inline int BIT_PERM(uint64_t block, int from, int to) {
    return BIT(block, from) << (64 - to);
}

void uint2cblock(uint64_t in, unsigned char *out) {
    int i;
    for (i = 7; i >= 0; --i) {
        out[i] = in & 0xff;
        in >>= 8;
    }
}

uint64_t cblock2uint(unsigned char *in) {
    uint64_t out = 0;
    int i;
    for (i = 0; i < 8; ++i) {
        out <<= 8;
        out |= in[i];
    }
    return out;
}

uint64_t KS(uint64_t key, int i) {}

uint64_t IP(uint64_t block) {
    return
        BIT_PERM(block, 58,  1) | BIT_PERM(block, 50,  2) | BIT_PERM(block, 42,  3) | BIT_PERM(block, 34,  4) |
        BIT_PERM(block, 26,  5) | BIT_PERM(block, 18,  6) | BIT_PERM(block, 10,  7) | BIT_PERM(block,  2,  8) |
        BIT_PERM(block, 60,  9) | BIT_PERM(block, 52, 10) | BIT_PERM(block, 44, 11) | BIT_PERM(block, 36, 12) |
        BIT_PERM(block, 28, 13) | BIT_PERM(block, 20, 14) | BIT_PERM(block, 12, 15) | BIT_PERM(block,  4, 16) |
        BIT_PERM(block, 62, 17) | BIT_PERM(block, 54, 18) | BIT_PERM(block, 46, 19) | BIT_PERM(block, 38, 20) |
        BIT_PERM(block, 30, 21) | BIT_PERM(block, 22, 22) | BIT_PERM(block, 14, 23) | BIT_PERM(block,  6, 24) |
        BIT_PERM(block, 64, 25) | BIT_PERM(block, 56, 26) | BIT_PERM(block, 48, 27) | BIT_PERM(block, 40, 28) |
        BIT_PERM(block, 32, 29) | BIT_PERM(block, 24, 30) | BIT_PERM(block, 16, 31) | BIT_PERM(block,  8, 32) |
        BIT_PERM(block, 57, 33) | BIT_PERM(block, 49, 34) | BIT_PERM(block, 41, 35) | BIT_PERM(block, 33, 36) |
        BIT_PERM(block, 25, 37) | BIT_PERM(block, 17, 38) | BIT_PERM(block,  9, 39) | BIT_PERM(block,  1, 40) |
        BIT_PERM(block, 59, 41) | BIT_PERM(block, 51, 42) | BIT_PERM(block, 43, 43) | BIT_PERM(block, 35, 44) |
        BIT_PERM(block, 27, 45) | BIT_PERM(block, 19, 46) | BIT_PERM(block, 11, 47) | BIT_PERM(block,  3, 48) |
        BIT_PERM(block, 61, 49) | BIT_PERM(block, 53, 50) | BIT_PERM(block, 45, 51) | BIT_PERM(block, 37, 52) |
        BIT_PERM(block, 29, 53) | BIT_PERM(block, 21, 54) | BIT_PERM(block, 13, 55) | BIT_PERM(block,  5, 56) |
        BIT_PERM(block, 63, 57) | BIT_PERM(block, 55, 58) | BIT_PERM(block, 47, 59) | BIT_PERM(block, 39, 60) |
        BIT_PERM(block, 31, 61) | BIT_PERM(block, 23, 62) | BIT_PERM(block, 15, 63) | BIT_PERM(block,  7, 64);
}

uint64_t IP_inv(uint64_t block) {
    return
        BIT_PERM(block, 40,  1) | BIT_PERM(block,  8,  2) | BIT_PERM(block, 48,  3)  | BIT_PERM(block, 16,  4) |
        BIT_PERM(block, 56,  5) | BIT_PERM(block, 24,  6) | BIT_PERM(block, 64,  7)  | BIT_PERM(block, 32,  8) |
        BIT_PERM(block, 39,  9) | BIT_PERM(block,  7, 10) | BIT_PERM(block, 47, 11)  | BIT_PERM(block, 15, 12) |
        BIT_PERM(block, 55, 13) | BIT_PERM(block, 23, 14) | BIT_PERM(block, 63, 15)  | BIT_PERM(block, 31, 16) |
        BIT_PERM(block, 38, 17) | BIT_PERM(block,  6, 18) | BIT_PERM(block, 46, 19)  | BIT_PERM(block, 14, 20) |
        BIT_PERM(block, 54, 21) | BIT_PERM(block, 22, 22) | BIT_PERM(block, 62, 23)  | BIT_PERM(block, 30, 24) |
        BIT_PERM(block, 37, 25) | BIT_PERM(block,  5, 26) | BIT_PERM(block, 45, 27)  | BIT_PERM(block, 13, 28) |
        BIT_PERM(block, 53, 29) | BIT_PERM(block, 21, 30) | BIT_PERM(block, 61, 31)  | BIT_PERM(block, 29, 32) |
        BIT_PERM(block, 36, 33) | BIT_PERM(block,  4, 34) | BIT_PERM(block, 44, 35)  | BIT_PERM(block, 12, 36) |
        BIT_PERM(block, 52, 37) | BIT_PERM(block, 20, 38) | BIT_PERM(block, 60, 39)  | BIT_PERM(block, 28, 40) |
        BIT_PERM(block, 35, 41) | BIT_PERM(block,  3, 42) | BIT_PERM(block, 43, 43)  | BIT_PERM(block, 11, 44) |
        BIT_PERM(block, 51, 45) | BIT_PERM(block, 19, 46) | BIT_PERM(block, 59, 47)  | BIT_PERM(block, 27, 48) |
        BIT_PERM(block, 34, 49) | BIT_PERM(block,  2, 50) | BIT_PERM(block, 42, 51)  | BIT_PERM(block, 10, 52) |
        BIT_PERM(block, 50, 53) | BIT_PERM(block, 18, 54) | BIT_PERM(block, 58, 55)  | BIT_PERM(block, 26, 56) |
        BIT_PERM(block, 33, 57) | BIT_PERM(block,  1, 58) | BIT_PERM(block, 41, 59)  | BIT_PERM(block,  9, 60) |
        BIT_PERM(block, 49, 61) | BIT_PERM(block, 17, 62) | BIT_PERM(block, 57, 63)  | BIT_PERM(block, 25, 64);
}

uint32_t f(uint32_t R, uint64_t key) {}

uint64_t des_encrypt_block(uint64_t block, uint64_t key) {
    int i;
    uint32_t L, R, L_next;
    block = IP(block);
    L = block >> 32;
    R = block & 0xFFFFFFFF;
    for (i = 1; i <= 16; ++i) {
        L_next = L ^ f(R, KS(key, i));
        R = L;
        L = L_next;
    }
    block = (uint64_t)L << 32 & R;
    return IP_inv(block);
}

uint64_t des_decrypt_block(uint64_t block, uint64_t key) {
    /* TODO */

    DES_cblock K, input, output;
    DES_key_schedule KS;

    uint2cblock(key, K);
    uint2cblock(block, input);
    DES_set_key(&K, &KS);
    DES_ecb_encrypt(&input, &output, &KS, DES_DECRYPT);
    return cblock2uint(output);
}

uint64_t des3_encrypt_block(uint64_t block, uint64_t keys[3]) {
    uint64_t b1, b2;

    b1 = des_encrypt_block(block, keys[0]);
    b2 = des_decrypt_block(b1, keys[1]);
    return des_encrypt_block(b2, keys[2]);
}

uint64_t des3_decrypt_block(uint64_t block, uint64_t keys[3]) {
    uint64_t b1, b2;

    b1 = des_decrypt_block(block, keys[2]);
    b2 = des_encrypt_block(b1, keys[1]);
    return des_decrypt_block(b2, keys[0]);
}

int main(int argc, char *argv[]) {
    enum { ENCRYPT, DECRYPT } mode;
    uint64_t keys[3], in, out;
    int i;
    unsigned char cin[8], cout[8];

    if (argc != 5) {
        fprintf(stderr, USAGE, argv[0]);
        return 1;
    }

    if (0 == strcmp(argv[1], "enc")) {
        mode = ENCRYPT;
    } else if (0 == strcmp(argv[1], "dec")) {
        mode = DECRYPT;
    } else {
        fprintf(stderr, USAGE, argv[0]);
        return 1;
    }

    for (i = 0; i < 3; ++i)
        sscanf(argv[2+i], "%llx", &keys[i]);

    while (8 == read(0, cin, 8)) {
        in = cblock2uint(cin);
        if (mode == ENCRYPT)
            out = des3_encrypt_block(in, keys);
        else
            out = des3_decrypt_block(in, keys);
        uint2cblock(out, cout);
        write(1, cout, 8);
    }
    return 0;
}

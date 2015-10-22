#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <openssl/des.h>

const char *USAGE = "Usage: %s <enc|dec> <key1> <key2> <key3>\n";

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

uint64_t des_encrypt_block(uint64_t block, uint64_t key) {
    /* TODO */

    DES_cblock K, input, output;
    DES_key_schedule KS;

    uint2cblock(key, K);
    uint2cblock(block, input);
    DES_set_key(&K, &KS);
    DES_ecb_encrypt(&input, &output, &KS, DES_ENCRYPT);
    return cblock2uint(output);
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

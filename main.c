#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <openssl/des.h>

typedef uint64_t KS[16];
typedef short int perm64_mx_t[64];

extern perm64_mx_t IP_mx, IP_inv_mx, PC_1_mx, PC_2_mx;

const char *USAGE = "Usage: %s <enc|dec> <key1> <key2> <key3>\n";

static inline int BIT(uint64_t block, int i) {
    return (block >> (64 - i)) & 1;
}

static inline int BIT_PERM(uint64_t block, int from, int to) {
    return BIT(block, from) << (64 - to);
}

uint64_t perm64(uint64_t in, perm64_mx_t perm_mx) {
    int i;
    uint64_t out = 0;
    for (i = 0; i < 64; ++i)
        out = out << 1 | BIT(in, perm_mx[i]);
    return out;
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

/* circular left shift of a 28-bit number */
uint32_t rotl28(uint32_t in, int l) {
    const uint32_t mask = (1 << 28) - 1;
    return ((in << l) | (in >> (28-l))) & 0xFFFFFFF;
}

void build_KS(uint64_t key, KS *ks) {
    const int shifts[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1 };
    int i;
    /* C and D are 28-bit, LSB aligned to the right */
    uint64_t CD = perm64(key, PC_1_mx);
    uint32_t C = CD >> 36, D = (CD >> 8) & 0xFFFFFFF;

    for (i = 0; i < 16; ++i) {
        C = rotl28(C, shifts[i]);
        D = rotl28(D, shifts[i]);
        CD = ((uint64_t)C << 36) | ((D & 0xFFFFFFF) << 8);
        (*ks)[i] = perm64(CD, PC_2_mx);
    }
}

uint64_t IP(uint64_t block) {
    return perm64(block, IP_mx);
}

uint64_t IP_inv(uint64_t block) {
    return perm64(block, IP_inv_mx);
}

uint32_t f(uint32_t R, uint64_t key) {}

uint64_t des_encrypt_block(uint64_t block, KS ks) {
    int i;
    uint32_t L, R, L_next;
    block = IP(block);
    L = block >> 32;
    R = block & 0xFFFFFFFF;
    for (i = 1; i <= 16; ++i) {
        L_next = L ^ f(R, ks[i]);
        R = L;
        L = L_next;
    }
    block = (uint64_t)L << 32 & R;
    return IP_inv(block);
}

uint64_t des_decrypt_block(uint64_t block, KS ks) {
    /* TODO */
    return block;
}

uint64_t des3_encrypt_block(uint64_t block, KS ks[3]) {
    uint64_t b1, b2;

    b1 = des_encrypt_block(block, ks[0]);
    b2 = des_decrypt_block(b1, ks[1]);
    return des_encrypt_block(b2, ks[2]);
}

uint64_t des3_decrypt_block(uint64_t block, KS ks[3]) {
    uint64_t b1, b2;

    b1 = des_decrypt_block(block, ks[2]);
    b2 = des_encrypt_block(b1, ks[1]);
    return des_decrypt_block(b2, ks[0]);
}

int main(int argc, char *argv[]) {
    enum { ENCRYPT, DECRYPT } mode;
    uint64_t keys[3], in, out;
    KS ks[3];
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

    for (i = 0; i < 3; ++i) {
        sscanf(argv[2+i], "%llx", &keys[i]);
        build_KS(keys[i], &ks[i]);
    }

    while (8 == read(0, cin, 8)) {
        in = cblock2uint(cin);
        if (mode == ENCRYPT)
            out = des3_encrypt_block(in, ks);
        else
            out = des3_decrypt_block(in, ks);
        uint2cblock(out, cout);
        write(1, cout, 8);
    }
    return 0;
}

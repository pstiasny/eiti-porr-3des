#include <stdlib.h>
#include <stdbool.h>

#include "des.h"

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

uint64_t E(uint64_t in) {
    int i;
    uint64_t out = 0;
    for (i = 0; i < 48; ++i)
        out = out << 1 | ((in >> (32 - E_mx[i])) & 1);
    return out;
}

int S(int in, perm64_mx_t s) {
    int adr;
    adr=(in & 0b11110) >> 1;
    adr|=((in >> 4) | ((in & 1)) << 4);
    return s[adr];
}

uint32_t perm32(uint32_t in, perm64_mx_t perm_mx) {
    int i;
    uint64_t out = 0;
    for (i = 0; i < 32; ++i)
        out = out << 1 | ((in >> (32 - perm_mx[i])) & 1);
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
        (*ks)[i] = perm64(CD, PC_2_mx) >> 16;
    }
}

uint64_t IP(uint64_t block) {
    return perm64(block, IP_mx);
}

uint64_t IP_inv(uint64_t block) {
    return perm64(block, IP_inv_mx);
}

uint32_t f(uint32_t R, uint64_t key) {
    uint32_t SUM = 0;
    int i;

    uint64_t pre_S = E(R) ^ key;
    for (i = 7; i >= 0; i--) {
        pre_S >>= 6;
        SUM |= S(pre_S & 0b111111, S_mx[i]) << ((7-i) *4); 
    }
    
    return perm32(SUM, P_mx);
}

uint64_t des_encrypt_block(uint64_t block, KS ks) {
    int i;
    uint32_t L, R, L_next;
    block = IP(block);
    L = block >> 32;
    R = block & 0xFFFFFFFF;
    for (i = 0; i < 16; ++i) {
        L_next = L ^ f(R, ks[i]);
        R = L;
        L = L_next;
    }
    block = (uint64_t)L << 32 & R;
    return IP_inv(block);
}

uint64_t des_decrypt_block(uint64_t block, KS ks) {
    int i;
    uint32_t L, R, L_next;
    block = IP(block);
    L = block >> 32;
    R = block & 0xFFFFFFFF;
    for (i = 15; i >= 0; --i) {
        L_next = L ^ f(R, ks[i]);
        R = L;
        L = L_next;
    }
    block = (uint64_t)L << 32 & R;
    return IP_inv(block);
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

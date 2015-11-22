#include <stdlib.h>

#include "des.h"

uint64_t choice(int bits_in, int bits_out, short int mx[], uint64_t in) {
    int i;
    uint64_t out = 0;
    for (i = 0; i < bits_out; ++i)
        out = out << 1 | ((in >> (bits_in - mx[i])) & 1);
    return out;
}

uint8_t S(uint8_t in, short int s[64]) {
    uint8_t adr;
    adr = (in & 0b11110) >> 1;
    adr |= in & 0b100000;
    adr |= (in & 1) << 4;
    return s[adr];
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
    const int shifts[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
    int i;
    /* C and D are 28-bit, LSB aligned to the right */
    uint64_t CD = PC_1(key);
    uint32_t C = CD >> 28, D = CD & 0xFFFFFFF;

    for (i = 0; i < 16; ++i) {
        C = rotl28(C, shifts[i]);
        D = rotl28(D, shifts[i]);
        CD = ((uint64_t)C << 28) | ((uint64_t)D & 0xFFFFFFF);
        (*ks)[i] = PC_2(CD);
    }
}

uint32_t f_s_boxes(uint64_t in) {
    uint32_t out = 0;
    int i;
    for (i = 7; i >= 0; i--) {
        out |= S(in & 0b111111, S_mx[i]) << ((7-i) *4); 
        in >>= 6;
    }
    return out;
}

uint32_t f(uint32_t R, uint64_t key) {
    uint64_t pre_S = E(R) ^ key;
    uint32_t post_S = f_s_boxes(pre_S);
    return P(post_S);
}

uint64_t des_encrypt_block(uint64_t block, KS ks) {
    int i;
    uint32_t L, R, R_next;
    block = IP(block);
    L = block >> 32;
    R = block & 0xFFFFFFFF;
    for (i = 0; i < 16; ++i) {
        R_next = L ^ f(R, ks[i]);
        L = R;
        R = R_next;
    }
    block = (uint64_t)R << 32 | L;
    return IP_inv(block);
}

uint64_t des_decrypt_block(uint64_t block, KS ks) {
    int i;
    uint32_t L, R, R_next;
    block = IP(block);
    L = block >> 32;
    R = block & 0xFFFFFFFF;
    for (i = 15; i >= 0; --i) {
        R_next = L ^ f(R, ks[i]);
        L = R;
        R = R_next;
    }
    block = (uint64_t)R << 32 | L;
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

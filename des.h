#include <stdint.h>

typedef uint64_t KS[16];

extern short int IP_mx[64], IP_inv_mx[64], PC_1_mx[56], PC_2_mx[48],
                 E_mx[48], P_mx[32], S_mx[8][64];

uint64_t choice(int bits_in, int bits_out, short int mx[], uint64_t in);
#define E(in) choice(32, 48, E_mx, in)
#define IP(in) choice(64, 64, IP_mx, in)
#define IP_inv(in) choice(64, 64, IP_inv_mx, in)
#define PC_1(in) choice(64, 56, PC_1_mx, in)
#define PC_2(in) choice(56, 48, PC_2_mx, in)
#define P(in) choice(32, 32, P_mx, in)

uint8_t S(uint8_t in, short int s[64]);
void uint2cblock(uint64_t in, unsigned char *out);
uint64_t cblock2uint(unsigned char *in);
uint32_t rotl28(uint32_t in, int l);
void build_KS(uint64_t key, KS *ks);
uint32_t f_s_blocks(uint64_t in);
uint32_t f(uint32_t R, uint64_t key);
uint64_t des_encrypt_block(uint64_t block, KS ks);
uint64_t des_decrypt_block(uint64_t block, KS ks);
uint64_t des3_encrypt_block(uint64_t block, KS ks[3]);
uint64_t des3_decrypt_block(uint64_t block, KS ks[3]);

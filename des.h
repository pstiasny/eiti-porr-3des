#include <stdint.h>

typedef uint64_t KS[16];
typedef short int perm64_mx_t[64];
typedef short int E_mx_t[48];

extern perm64_mx_t IP_mx, IP_inv_mx, PC_1_mx, PC_2_mx, P_mx;
extern E_mx_t E_mx;
extern perm64_mx_t S_mx[];

uint64_t choice(int bits_in, int bits_out, short int mx[], uint64_t in);
#define E(in) choice(32, 48, E_mx, in)
#define IP(in) choice(64, 64, IP_mx, in)
#define IP_inv(in) choice(64, 64, IP_inv_mx, in)
#define PC_1(in) choice(64, 56, PC_1_mx, in)
#define PC_2(in) choice(56, 48, PC_2_mx, in)
#define P(in) choice(32, 32, P_mx, in)

uint8_t S(uint8_t in, perm64_mx_t s);
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

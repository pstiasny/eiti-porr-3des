#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "des.h"

#define BUF_SIZE 256
const char *USAGE = "Usage: %s <enc|dec> <key1> <key2> <key3>\n";

int main(int argc, char *argv[]) {
    enum { ENCRYPT, DECRYPT } mode;
    uint64_t keys[3], in, out;
    KS ks[3];
    int i, j, readc;
    unsigned char cin[BUF_SIZE][8], cout[BUF_SIZE][8];

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

    while (readc = read(0, cin, 8*BUF_SIZE)) {
        if (readc == 0)
            return 0;
        if (readc == -1) {
            fprintf(stderr, "IO error\n");
            return 1;
        }
        if (readc % 8) {
            fprintf(stderr, "Input length not multiple of 8 bytes\n");
            return 1;
        }

        for (j = 0; j < readc / 8; ++j) {
            in = cblock2uint(cin[j]);
            if (mode == ENCRYPT)
                out = des3_encrypt_block(in, ks);
            else
                out = des3_decrypt_block(in, ks);
            uint2cblock(out, cout[j]);
        }
        write(1, cout, readc);
    }
    return 0;
}

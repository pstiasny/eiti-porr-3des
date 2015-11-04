#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "des.h"

const char *USAGE = "Usage: %s <enc|dec> <key1> <key2> <key3>\n";

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

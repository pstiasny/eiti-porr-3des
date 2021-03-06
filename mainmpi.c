#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "mpi.h"

#include "des.h"

#define BUF_SIZE 256
const char *USAGE = "Usage: %s <enc|dec> <key1> <key2> <key3> <infile>\n";

int main(int argc, char *argv[]) {
    enum { ENCRYPT, DECRYPT } mode;
    struct stat filestat;
    uint64_t *buf, *recvbuf, out;
    uint64_t keys[3];
    KS ks[3];
    int i, j, readc, rank, numprocs, infile;
    uint64_t blockcount;
    unsigned char cbuf[8];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if (argc != 6) {
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
    
    if (rank == 0) {
        infile = open(argv[5], O_RDONLY);
        if (infile < 0) {
            fprintf(stderr, "couldn't open input file");
            return 1;
        }

        if (0 > fstat(infile, &filestat)) {
            fprintf(stderr, "couldn't stat input file");
            return 1;
        }

        if (filestat.st_size % 8) {
            fprintf(stderr, "Input length not multiple of 8 bytes\n");
            return 1;
        }

        buf = (uint64_t*)malloc(filestat.st_size);
        if (!buf) {
            fprintf(stderr, "failed to allocate memory\n");
            return 1;
        }

        for (j = 0; j < filestat.st_size / 8; ++j) {
            readc = read(infile, cbuf, 8);
            if (8 != readc) {
                fprintf(stderr, "IO error\n");
                return 1;
            }

            buf[j] = cblock2uint(cbuf);
        }
    }

    blockcount = filestat.st_size / 8;
    MPI_Bcast(&blockcount,1,MPI_UINT64_T,0,MPI_COMM_WORLD);	
    int *sendcounts, *displs;
    int sendcount_rounded = blockcount / numprocs,
        sendcount_rest = blockcount % numprocs;
    sendcounts = (int*)malloc(numprocs*sizeof(int));
    displs = (int*)malloc(numprocs*sizeof(int));
    for (i = 0; i < numprocs - 1; i++) {
        sendcounts[i] = sendcount_rounded;
        displs[i] = i * sendcount_rounded;
    }
    sendcounts[numprocs-1] = sendcount_rounded + sendcount_rest;
    displs[numprocs-1] = (numprocs-1) * sendcount_rounded;
   
    int recvcount;
    if (rank == (numprocs - 1))
        recvcount = sendcount_rounded + sendcount_rest;
    else
        recvcount = sendcount_rounded;
    recvbuf = (uint64_t*)malloc(recvcount*sizeof(uint64_t));
    

    MPI_Scatterv(buf, sendcounts, displs, MPI_UINT64_T, recvbuf, blockcount,
                 MPI_UINT64_T, 0, MPI_COMM_WORLD);

    for (j = 0; j < recvcount; ++j) {
        if (mode == ENCRYPT)
            out = des3_encrypt_block(recvbuf[j], ks);
        else
            out = des3_decrypt_block(recvbuf[j], ks);
        recvbuf[j] = out;
    }


    MPI_Gatherv(recvbuf, recvcount, MPI_UINT64_T, buf, sendcounts, displs,
                MPI_UINT64_T, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (j = 0; j < blockcount; ++j) {
            uint2cblock(buf[j], cbuf);
            write(1, cbuf, 8);
        }
    }

    MPI_Finalize();
    return 0;
}

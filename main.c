#include <stdio.h>
#include <stdlib.h>
#include "pgm.h"


int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: %s <src.pgm> <dst.pgm>", argv[0]);
        exit(1);
    }
    const char *fileName = argv[1], *destFileName = argv[2];
    PGM_t *image = newPGM(fileName, 0UL, 0UL);
    printf("create ok\n");
    FILE *rImage = fopen(fileName, "r");
    image->read(image, rImage);
    printf("edit ok\n");
    FILE *imageFile = fopen(destFileName, "w");
    image->write(image, imageFile);
    fclose(imageFile);
    fclose(rImage);
    image->free(image);
    printf("write ok\n");
    return 0;
}
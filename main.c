#include <stdio.h>
#include <stdlib.h>
#include "pgm.h"


/**
 * \brief Uses Bresenham’s Line Drawing Algorithm to draw a line from (x0,y0) to (x1, y1)
 * \param x0 the x-component of the start vector
 * \param y0 the y-component of the start vector
 * \param x1 the x-component of the end vector
 * \param y1 the y-component of the end vector
 * \param img a PGM image
 * \param color a color (in grayscale)
 */
void line(long x0, long y0, long x1, long y1, PGM_t *img, unsigned char color)
{
    float t;
    for (t = 0; t < 1.; t += 0.001) {
        long x = (long) (x0 + (x1 - x0) * t);
        long y = (long) (y0 + (y1 - y0) * t);
        img->setPoint(img, (size_t)x, (size_t)y, color);
    }
}

void triangleShape(long x0, long y0, long x1, long y1, long x2, long y2, PGM_t *img, unsigned char color)
{
    line(x0, y0, x1, y1, img, color);
    line(x1, y1, x2, y2, img, color);
    line(x2, y2, x0, y0, img, color);

}

void copyPGMFile(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: %s <src.pgm> <dst.pgm>", argv[0]);
        exit(1);
    }
    const char *fileName = argv[1], *destFileName = argv[2];
    PGM_t *image = newPGM(fileName, 0UL, 0UL, 255);
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
}

int main(int argc, char **argv)
{
    PGM_t *img = newPGM("Line Plot", 100UL, 100UL, 255);
    for(size_t i = 0; i < 100UL; ++i)
        for(size_t j = 0; j < 100UL; ++j)
            img->setPoint(img, i, j, 255);

    triangleShape(33, 33, 88, 88, 0, 60, img, 0);
    FILE *fp = fopen("./lineplot.pgm", "w");
    for(size_t col = 0; col < img->width; ++col) {
        int paint = 0;
        for (size_t row = 0; row < img->height; ++row) {
            if (img->getPoint(img, row, col) == 0) paint = 1;
            if (paint == 1 && img->getPoint(img, row, col) == 255) paint = 2;
            if (paint == 2 && img->getPoint(img, row, col) == 0) break;
            if (paint == 2) img->setPoint(img, row, col, 0);
        }
    }
    img->write(img, fp);
    img->free(img);
    fclose(fp);
    return 0;
}
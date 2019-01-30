#include <stdio.h>
#include <stdlib.h>
#include "pgm.h"
#include <math.h>

#define SWAP(a, b, t) \
{ t c = a; a = b; b = c; }

struct _LinearFunction;

typedef long (*LinearFunctionEvaluator_t)(struct _LinearFunction, long x);

typedef struct _LinearFunction (*GenerateLinearFunction_t)(struct _LinearFunction);

typedef struct _LinearFunction {
    long m, b;
    LinearFunctionEvaluator_t eval;
    GenerateLinearFunction_t transpose;
} LinearFunction_t;

long evaluateLinearFunction(struct _LinearFunction lf, long x)
{
    return x * lf.m + lf.b;
}

struct _LinearFunction transposeLinearFunction(struct _LinearFunction f1)
{
    // y = mx + b
    // x=(y - b)/m=1/m*y - b/m
    struct _LinearFunction f2 = {.b = (long) (-(float) f1.b / (float) f1.m),
            .m = (long) ((float) 1 / (float) f1.m)};
    return f2;
}

int pointAbove(LinearFunction_t f, long x0, long y0)
{
    return f.eval(f, x0) > y0;
}

int pointBelow(LinearFunction_t f, long x0, long y0)
{
    return f.eval(f, x0) < y0;
}

int pointOnLine(LinearFunction_t f, long x0, long y0)
{
    return f.eval(f, x0) == y0;
}

struct _LinearFunction newLinearFunction(long m, long b)
{
    return (struct _LinearFunction) {m, b, &evaluateLinearFunction, &transposeLinearFunction};
}

struct _LinearFunction newVector2(long x0, long y0, long x1, long y1)
{
    return newLinearFunction((y0 - y1) / (x0 - x1), y0 - (y0 - y1) / (x0 - x1) * x0);
}

void fillNeighbors(PGM_t *img, long x, long y, unsigned char color)
{
    static const long nh[4][2] = {{0,  1},
                                  {1,  0},
                                  {0,  -1},
                                  {-1, 0}};
    for (int i = 0; i < 4; ++i) {
        long xt = nh[i][0] + x, yt = nh[i][1] + y;
        if (xt >= 0 && xt < img->height && yt >= 0 && yt < img->width) {
            img->setPoint(img, (size_t) xt, (size_t) yt, color);
        }
    }
}

void line(long x0, long y0, long x1, long y1, PGM_t *image, unsigned char color)
{
    int steep = 0;
    if (labs(x0 - x1) < labs(y0 - y1)) {
        SWAP(x0, y0, long);
        SWAP(x1, y1, long);
        steep = 1;
    }
    if (x0 > x1) {
        SWAP(x0, x1, long);
        SWAP(y0, y1, long);
    }
    long dx = x1 - x0;
    long dy = y1 - y0;
    long derror2 = labs(dy) * 2;
    long error2 = 0;
    long y = y0;
    for (long x = x0; x <= x1; x++) {
        if (steep) {
            image->setPoint(image, (size_t) y, (size_t) x, color);
        } else {
            image->setPoint(image, (size_t) x, (size_t) y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}


void triangleShape(long x0, long y0, long x1, long y1, long x2, long y2, PGM_t *img, unsigned char color)
{
    /* sort from left to right by pairwise comparison */
    if (x1 < x0 && x1 < x2) {
        SWAP(x0, x1, long); SWAP(y0, y1, long);
    }
    if (x2 < x0 && x2 < x1) {
        SWAP(x0, x2, long); SWAP(y0, y2, long);
    }
    if (x1 > x0 && x1 > x2) {
        SWAP(x1, x2, long); SWAP(y1, y2, long);
    }
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
    for (size_t i = 0; i < 100UL; ++i)
        for (size_t j = 0; j < 100UL; ++j)
            img->setPoint(img, i, j, 255);

    triangleShape(10, 10, 30, 20, 50, 50, img, 0);
    FILE *fp = fopen("./lineplot.pgm", "w");
    img->write(img, fp);
    img->free(img);
    fclose(fp);
    return 0;
}
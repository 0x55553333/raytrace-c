/*
 * pgm.h / pgm.c - rjf's tiny pgm parser
 * copyright (c) rui-jie fang 2019.
 */

#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>


void enablePrintDebugPGM(struct _PGM *pgm)
{
    pgm->printDebug = 1;
}

void disablePrintDebugPGM(struct _PGM *pgm)
{
    pgm->printDebug = 0;
}

size_t getXTranslationPGM(struct _PGM *pgm, size_t row, size_t col)
{
    return row * (pgm->width / 8UL + 1) + col / 8UL;
}

size_t getYTranslationPGM(struct _PGM *pgm, size_t row, size_t col)
{
    return 8UL * (col % 8UL);
}

void setPointPGM(struct _PGM *pgm, size_t row, size_t col, unsigned char px)
{
    size_t x = pgm->getXTranslation(pgm, row, col),
            y = pgm->getYTranslation(pgm, row, col);
    pgm->image[x] &= ~(0xffULL << y); /* Clear 8-bit value */
    pgm->image[x] |= (((PGMPackedPixel_t) px) << y) & (0xffULL << y); /* Set */
}

unsigned getPointPGM(struct _PGM *pgm, size_t row, size_t col)
{
    size_t x = pgm->getXTranslation(pgm, row, col),
            y = pgm->getYTranslation(pgm, row, col);
    unsigned k = (unsigned) ((pgm->image[x] >> y) & 0xffULL);
    return k;
}

void writePGM(struct _PGM *pgm, FILE *file)
{
    fprintf(file, "P2 %lu %lu %lu\n", pgm->width, pgm->height, pgm->maxPixels); /* 8 bytes */
    fprintf(file, "# %s\n", pgm->name);
    for(size_t r = 0; r < pgm->height; ++r) {
        for(size_t c = 0; c < pgm->width; ++c) {
            fprintf(file, "%u ", (unsigned) pgm->getPoint(pgm, r, c));
        }
        fprintf(file, "\n");
    }

}

void readPGM(struct _PGM *pgm, FILE *file)
{
    size_t idx = 0, cap = 15UL, ssz, i = 0, j = 0;
    char *buf, ch = 0;
    int going = 1, state = 0;
    pgm->width = pgm->height = pgm->maxPixels = pgm->size = 0;

    while (going) {
        buf = malloc(cap); ch = 0;
        while (ch != '\n') {
            ch = (char) fgetc(file);
            if (idx >= cap) {
                cap *= 2; buf = realloc(buf, cap);
            }
            buf[idx] = ch; ++idx;
        }
        if (idx == cap - 1) {
            ++cap; buf = realloc(buf, cap);
        }
        buf[idx] = '\0';
        ssz = idx + 1; idx = 0;
        while (idx < ssz && (buf[idx] == ' ' || buf[idx] == '\t')) ++idx;
        if (buf[idx] == '#') {
            goto next_token;
        } else if (buf[idx] == '\n') {
            goto next_token;
        }
        switch (state) {
            case 0: {
                if (buf[idx] != 'P' || buf[idx+1] != '2') {
                    buf[idx+2] = '\0';
                    if (pgm->printDebug)
                        printf("err: %s is not a good format\n", buf);
                    free(buf); buf = NULL;
                    return;
                }
                idx += 2;
                state = 1;
                while (buf[idx] == ' ' || buf[idx] == '\t') ++idx;
                if (buf[idx] == '\n' || buf[idx] == '\0' || buf[idx] == '\r') goto next_token;
            }
            case 1: {
                pgm->width = strtoul(buf + idx, NULL, 10);
                if (pgm->width == 0) {
                    if (pgm->printDebug)
                        printf("err: %s yielded width = 0\n", buf + idx);
                    free(buf); buf = NULL;
                    return;
                }
                while (isdigit(buf[idx])) ++idx;
                while (buf[idx] == ' ' || buf[idx] == '\t') ++idx;
                state = 2;
            }
            case 2: {
                if (buf[idx] == '\n' || buf[idx] == '\r' || buf[idx] == '\0' || idx == ssz)
                    goto next_token;
                pgm->height = strtoul(buf + idx, NULL, 10);
                if (pgm->height == 0) {
                    if (pgm->printDebug)
                        printf("err: %s yielded height = 0\n", buf);
                    free(buf); buf = NULL; return;
                }
                while (isdigit(buf[idx])) ++idx;
                state = 3;
            }
            case 3: {
                if (buf[idx] == '\n' || buf[idx] == '\r' || buf[idx] == '\0' || idx == ssz)
                    goto next_token;
                pgm->maxPixels = strtoul(buf + idx, NULL, 10);
                pgm->size = pgm->height * (pgm->width / 8UL + 1UL);
                pgm->image = malloc(pgm->size * sizeof(PGMPackedPixel_t));
                state = 4;
                break;
            }
            case 4: {
                while (idx < ssz) {
                    if (j == pgm->width) {
                        ++i; j = 0;
                    }
                    if (i == pgm->height) {
                        going = 0; break;
                    }
                    while (buf[idx] == '\t' || buf[idx] == ' ') ++idx;
                    if (buf[idx] == '\r' || buf[idx] == '\n' || buf[idx] == '\0')
                        break;
                    unsigned d = (unsigned) strtoul(buf + idx, NULL, 10);
                    pgm->setPoint(pgm, i, j, (unsigned char) d);
                    while (isdigit(buf[idx])) ++idx;
                    ++j;
                }
            }
        }

        next_token:
        free(buf); buf = NULL;
        cap = 15UL; ssz = idx = 0;
    }

}

void freePGM(struct _PGM *pgm)
{
    if (pgm->image != NULL) free(pgm->image);
    pgm->size = pgm->height = pgm->width = 0;
    free(pgm);
}

PGM_t* newPGM(const char* name, size_t width, size_t height, unsigned maxPixels)
{
    PGM_t *pgm = malloc(sizeof(struct _PGM));
    pgm->width = width;
    pgm->height = height;
    pgm->maxPixels = maxPixels;
    pgm->size = height * (width / 8ULL + 1);
    pgm->name = name;
    pgm->write = &writePGM;
    pgm->read = &readPGM;
    pgm->free = &freePGM;
    pgm->getXTranslation = &getXTranslationPGM;
    pgm->getYTranslation = &getYTranslationPGM;
    pgm->setPoint = &setPointPGM;
    pgm->getPoint = &getPointPGM;
    pgm->debugToStdio = &enablePrintDebugPGM;
    pgm->disableDebugToStdio = &disablePrintDebugPGM;
    pgm->printDebug = 0;
    if (pgm->size > 0)
        pgm->image = malloc(pgm->size*sizeof(PGMPackedPixel_t));
    else pgm->image = NULL;
    return pgm;
}

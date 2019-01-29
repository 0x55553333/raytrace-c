/*
 * pgm.h / pgm.c - rjf's tiny pgm parser
 * copyright (c) rui-jie fang 2019.
 */

#ifndef RAYTRACE_C_PGM_H
#define RAYTRACE_C_PGM_H
#include <stdlib.h>
#include <stdio.h>
struct _PGM;
typedef void (*PGMWriter_t) (struct _PGM *pgm, FILE *file);
typedef void (*PGMReader_t) (struct _PGM *pgm, FILE *file);
typedef void (*PGMDestructor_t) (struct _PGM *pgm);
typedef void (*PGMMethod_t) (struct _PGM *pgm);
typedef size_t (*PGMGetXTranslation_t) (struct _PGM *pgm, size_t row, size_t col);
typedef size_t (*PGMGetYTranslation_t) (struct _PGM *pgm, size_t row, size_t col);
typedef void (*PGMSetPoint_t) (struct _PGM *pgm, size_t row, size_t col, unsigned char px);
typedef unsigned  (*PGMGetPoint_t) (struct _PGM *pgm, size_t row, size_t col);
typedef uint64_t PGMPackedPixel_t;
struct _PGM {
    /* Image representation in bit vector format */
    PGMPackedPixel_t *image;
    /* Name of the image */
    const char* name;
    /* Image specs */
    size_t width;
    size_t height;
    /* Size of image in longwords */
    size_t size;
    /* Max pixel, required by PGM format */
    size_t maxPixels;
    /**
     * \brief Writes image to an open FILE* handle.
     * \param PGM_t *pgm, the open context.
     * \param FILE *file, the file to write to.
     */
    PGMWriter_t write;
    /**
     * \brief Reads image in from an open FILE* handle.
     * and erases previous image.
     * \param PGM_t *pgm, the open context.
     * \param FILE *file, the file to read from.
     */
    PGMReader_t read;
    /**
     * \brief Destructs a given PGM context.
     */
    PGMDestructor_t free;
    /**
     * \brief Returns the row coordinate of a point inside
     * the bit vector.
     * \param PGM_t *pgm, the open context.
     * \param size_t row, the row index of the point inside the original image.
     * \param size_t col, the col index of the point inside the original image.
     * \return The translated row coordinate of (row,col) inside the bit vector.
     */
    PGMGetXTranslation_t getXTranslation;
    /**
     * \brief Returns the column coordinate of a point inside
     * the bit vector.
     * \param PGM_t *pgm, the open context.
     * \param size_t row, the row index of the point inside the original image.
     * \param size_t col, the col index of the point inside the original image.
     * \return The translated column coordinate of (row,col) inside the bit vector.
     */
    PGMGetYTranslation_t getYTranslation;
    /**
     * \brief Sets a point defined by (row, col) of the PGM image.
     * \param PGM_t *pgm, the open context.
     * \param size_t row, the row index of the point.
     * \param size_t col, the col index of the point.
     * \param unsigned px, the value contained by the point,
     * not to be greater than pgm->maxPixels.
     */
    PGMSetPoint_t setPoint;
    /**
     * \brief Returns the value of a point defined by (row, col) of the PGM image.
     * \param size_t row, the row index of the point.
     * \param size_t col, the col index of the point.
     * \return an unsigned value denoting the value contained by the point (row, col).
     */
    PGMGetPoint_t getPoint;
    /**
     * \brief Enables debugging to stdio output.
     * \param PGM_t *pgm, the open context.
     */
    PGMMethod_t debugToStdio;
    /**
     * \brief Disables debugging to stdio output.
     * \param PGM_t *pgm, the open context.
     */
    PGMMethod_t disableDebugToStdio;
    /* User-defined flag for enabling debugging via stdio */
    int printDebug;
};typedef struct _PGM PGM_t;
PGM_t* newPGM(const char* name, size_t width, size_t height, unsigned maxPixels);
#endif //RAYTRACE_C_PGM_H

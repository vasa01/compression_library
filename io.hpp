/*
libit - Library for basic source and channel coding functions
Copyright (C) 2005-2008 Francois Cayre, Vivien Chappelier, Herve Jegou

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
I/O functions for the libit library
Copyright (C) 2005 Herve Jegou
*/

#ifndef __it_io_h
#define __it_io_h

#include <stdarg.h>
#include <assert.h>
#include "mat.hpp"

/*---------------------------------------------------------------------------*/
/*! \defgroup io I/O functions                                               */
/* @{                                                                        */
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Extending printing of formatted string (includes vector and matrix)  */
void it_vprintf(char *fmt, va_list args);
void it_fprintf(FILE * output, char *fmt, ...);
void it_printf(char *fmt, ...);
void it_vfprintf(FILE * output, char *fmt, va_list ap);

/* Error functions                                                      */
void it_warning(char *fmt, ...);
void it_error(char *fmt, ...);

void __it_assert(int a, const char *filename, int line, char *message);
#ifdef NDEBUG
#define it_assert(a,expr) ((void) (0))
#else
#if defined(__GNUC__)
#define it_assert(a,expr) __it_assert((long)(a), __FILE__, __LINE__, expr)
#else
#define it_assert(a,expr) __it_assert((long)(a), "", 0, expr)
#endif
#endif

/* Modify the default format for vector printing (called by format $v)  */
void it_set_vec_default_fmt(char *fmt);

/* Modify the default format for matrix printing (called by format #m)  */
void it_set_mat_default_fmt(char *fmt);


/* Read integers and double from an NOT null-terminated string */
char *it_read_int(char *s, int *p_val);
char *it_read_float(char *s, float *p_val);
char *it_read_double(char *s, double *p_val);
char *it_read_cplx(char *s, cplx * p_val);

/* Read a vector from a string and return the position after the vector */
char *it_read_vec(char *s, vec * v);
char *it_read_fvec(char *s, vec * v);
char *it_read_ivec(char *s, ivec * v);
char *it_read_bvec(char *s, bvec * v);
char *it_read_cvec(char *s, cvec * v);

/* Read a matrix from a string and return the position after the matrix */
char *it_read_mat(char *s, mat * m);
char *it_read_fmat(char *s, mat * m);
char *it_read_imat(char *s, imat * m);
char *it_read_bmat(char *s, bmat * m);
char *it_read_cmat(char *s, cmat * m);

/* Create a vector from a string */
vec  vec_new_string(char *s);
vec  fvec_new_string(char *s);
ivec ivec_new_string(char *s);
bvec bvec_new_string(char *s);
cvec cvec_new_string(char *s);

/* Create a matrix from a string */
mat  mat_new_string(char *s);
mat  fmat_new_string(char *s);
imat imat_new_string(char *s);
bmat bmat_new_string(char *s);
cmat cmat_new_string(char *s);

/*----------------------------------------------------------------------*/
/* Determines the type of a PNM file, based on magic numbers.
The returned value is a character between '1' and '6'.
If an error occured, the returned value the value 0                  */
char pnm_type(const char *filename);

/* Retrieve some information about an pnm file.
String comments must be pre-allocated                                */
int  pnm_info(const char *filename, char *p_pnm_type, int *p_width,
	int *p_height, int *p_max_val, char *comments, int length);

/* Read a pgm file and return the corresponding matrix of double values */
mat  mat_pgm_read(const char *filename);

/* Read a pgm file and return the corresponding matrix of int values */
imat imat_pgm_read(const char *filename);

/* Write a matrix of double as a pgm image file                         */
int  mat_pgm_write(const char *filename, mat m);

/* Write a matrix of integers as a pgm file                             */
int  imat_pgm_write(const char *filename, imat m);

/*----------------------------------------------------------------------*/
/* WAV file handling functions                                          */
int  wav_info(const char *filename, int *p_channels, int *p_srate,
	int *p_depth, int *p_length);
mat  mat_wav_read(const char *filename);
imat imat_wav_read(const char *filename);
void mat_wav_write(const char *filename, mat m, int srate, int depth);
void imat_wav_write(const char *filename, imat m, int srate, int depth);


/*----------------------------------------------------------------------*/
/* Write and read matrix in a pack format (dimension+data)              */

void vec_fwrite(FILE * stream, vec v);
void fvec_fwrite(FILE * stream, vec v);
void bvec_fwrite(FILE * stream, bvec v);
void ivec_fwrite(FILE * stream, ivec v);
void cvec_fwrite(FILE * stream, cvec v);


/* Return 1 if the vector is successfully read, 0 otherwise */
int  vec_fread(FILE * stream, vec v);
int  fvec_fread(FILE * stream, vec v);
int  bvec_fread(FILE * stream, bvec v);
int  ivec_fread(FILE * stream, ivec v);
int  cvec_fread(FILE * stream, cvec v);

/* return NULL if the vector can't be read */
vec  vec_new_fread(FILE * stream);
vec  fvec_new_fread(FILE * stream);
bvec bvec_new_fread(FILE * stream);
ivec ivec_new_fread(FILE * stream);
cvec cvec_new_fread(FILE * stream);

void mat_fwrite(FILE * stream, mat m);
void fmat_fwrite(FILE * stream, mat m);
void bmat_fwrite(FILE * stream, bmat m);
void imat_fwrite(FILE * stream, imat m);
void cmat_fwrite(FILE * stream, cmat m);

/* Read a vector in a binary format from a buffer.
The function returns the number of bytes read */
int vec_sread(void * buffer, vec v);
int fvec_sread(void * buffer, vec v);
int ivec_sread(void * buffer, ivec v);

mat  mat_new_fread(FILE * stream);
bmat bmat_new_fread(FILE * stream);
imat imat_new_fread(FILE * stream);
cmat cmat_new_fread(FILE * stream);


/*----------------------------------------------------------------------*/
/* Read files                                                           */

/* Read a binary vector from a file. If the parameter nb_max is > 0,
then the number of bits read is bounded to this quantity.              */
bvec bvec_file_read_bits(const char *filename, int nb_max);

/* Write a vector of bits in a file */
void bvec_file_write_bits(const char *filename, bvec v);

/* @} */

#endif

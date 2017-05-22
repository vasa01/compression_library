/*
libit - Library for basic source and channel coding functions
Copyright (C) 2005-2005 Vivien Chappelier, Herve Jegou

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
Vectors functions
Copyright (C) 2005-2007 Vivien Chappelier, Herve Jegou
*/


#ifndef __it_vec_h
#define __it_vec_h

#include "cplx.hpp"
#include "types.hpp"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/*---------------------------------------------------------------------------*/
/*! \defgroup vec Vectors                                                    */
/* @{                                                                        */
/*---------------------------------------------------------------------------*/
/* Notes: the following structures are used to store some vectors values.
For a given vector v of type type_t (with type_t=int, double, byte or complex),
the memory storage is managed as follows (1b=1 byte):

variable      type          memory pos               purpose

none          none          _Vec_header[v]->ptr     start of the memory block allocated for the vector.
This position is not always the one of the vector header,
because a word alignment is enforced while a vector is allocated.
Vec_header(v) _Vec_header_  v-sizeof(header) bytes  Vector header (see below)
v[0]         type_t         v                       first element of the vector
v[1]         type_t         v+sizeof(type_t) bytes  second element...
v[length-1]  type_t         v+length*sizeof(type_t) last element of the vector
v[length], ... v[max_length-1] pre-allocated but not currently unused.

Thus, the amount of memory allocated is greater than the number of element by at least
sizeof(_Vec_header_) bytes, i.e. about 16 bytes on a 32 bits machine.
Usually, vector creation functions allocate vectors such that length=length_max.
When the vector length decreases, the already allocated memory remains allocated.
When the vector length increases, a geometric reallocation strategy is used.
*/

/*---------------------------------------------------------------------------*/
/*! @name Constant vectors                                                   */
/*---------------------------------------------------------------------------*/

typedef void * Vec;
typedef double * vec;
typedef int * ivec;
typedef byte * bvec;
typedef cplx * cvec;


/*@{*/
/*! void vector of length 0, i.e. with no element
\warning this is a constant vector which is in essence used for comparison.
It has to be differentiate from an unallocated vector or from the NULL vector.
*/
extern vec const vec_null;
extern ivec const ivec_null;
extern bvec const bvec_null;
extern cvec const cvec_null;
/*@}*/
/*@}*/


typedef struct _Vec_header_ {
	idx_t length;		/* effective length of the vector (<= max_length)     */
	idx_t length_max;		/* amount of memory allocated for the vector elements */
	void *ptr;			/* memory block associated to this vector             */
	size_t element_size;	/* size of the stored elements                        */
} Vec_header_t;

/* Return the header associated with a given vector v. For internal use. */
#define Vec_header(v) (*(((Vec_header_t *) (v)) - 1))
#define Vec_element_size(v) (Vec_header(v).element_size)



/* We want to make sure the memory is always properly aligned on an 16-byte
boundary. This is needed to guarantee pointer arithmetic works with cplx.
Besides it provides faster memory access in case of MMX/SSE/.. optimizations.
*/
#define IT_ALLOC_ALIGN (sizeof(cplx))


/* The reallocation of memory for vectors is done here according to a geometric
rule so that the complexity of the overall reallocation proccess remains
linear. The following value define the ratio of the realloction procedure.
Note that the value is a tradeoff between memory overhead and computing
cost
*/
#define DYN_ALLOC_RULE(l) ((l)*4/3+1)

/* Vector allocation functions */
Vec  __Vec_new_alloc(size_t elem_size, idx_t length, idx_t length_max);
Vec  __Vec_new_realloc(void *V, size_t elem_size, idx_t length,
	idx_t length_max);



/*! @addtogroup vec                                                          */
/* @{                                                                        */
/*---------------------------------------------------------------------------*/
/*! @name Basic vector allocation, initialization and deletion               */
/*---------------------------------------------------------------------------*/

/*@{*/
/*! Vector allocation with explicit allocation of the maximum number of elements */
#define Vec_new_alloc( type_t, N, length_max ) \
    ((type_t *) __Vec_new_alloc(sizeof(type_t), N, length_max ))

vec vec_new_alloc(idx_t length, idx_t length_max);
ivec ivec_new_alloc(idx_t length, idx_t length_max);
bvec bvec_new_alloc(idx_t length, idx_t length_max);
cvec cvec_new_alloc(idx_t length, idx_t length_max);
/*@}*/


/*@{*/
/*! Allocate a vector */
#define Vec_new( type_t, N ) ((type_t *) __Vec_new( sizeof(type_t), N ))
Vec __Vec_new(size_t elem_size, idx_t N);

vec vec_new(idx_t length);
ivec ivec_new(idx_t length);
bvec bvec_new(idx_t length);
cvec cvec_new(idx_t length);
/*@}*/


/*@{*/
/*! Free a vector */
#define Vec_delete( v ) free( Vec_header(v).ptr )

void vec_delete(vec v);
void ivec_delete(ivec v);
void bvec_delete(bvec v);
void cvec_delete(cvec v);
/*@}*/


/*---------------------------------------------------------------------------*/
/*! @name Length and max length vector operations                            */
/*---------------------------------------------------------------------------*/

/*@{*/
/*! Return the length of the vector */
#define Vec_length( v ) ( Vec_header(v).length )

idx_t vec_length(vec v);
idx_t ivec_length(ivec v);
idx_t bvec_length(bvec v);
idx_t cvec_length(cvec v);
/*@}*/


/* to use the entity end in a vec function, this macro is called */
#define VEC_END_PARAM( v, param ) do { if( param == end ) param = Vec_length( v ) - 1; } while(0)

/*@{*/
/*! Return the maximum length of the vector (number of allocated elements) */
#define Vec_length_max( v ) ( Vec_header(v).length_max )

idx_t vec_length_max(vec v);
idx_t ivec_length_max(ivec v);
idx_t bvec_length_max(bvec v);
idx_t cvec_length_max(cvec v);
/*@}*/


/*@{*/
/*! Modify the maximum length of a vector
\warning the function Vec_set_length_max may change the value of the pointer
corresponding to vector v. Consequently, the functions calling this function
must be handled with care, especially when vectors are passed as parameters   */
#define Vec_set_length_max( v, N ) do {                         \
  int L = Vec_length(v);					\
  size_t elem_size = Vec_element_size(v);			\
								\
  assert( N >= Vec_length(v) );					\
  if( N > Vec_length(v) ) {					\
    Vec new_Vec = __Vec_new_realloc(v, elem_size, L, N);	\
    (v) = (v) + ((byte *) new_Vec - (byte *) (v)) / elem_size;	\
  }								\
} while(0)


vec _vec_set_length_max(vec v, idx_t N);
ivec _ivec_set_length_max(ivec v, idx_t N);
bvec _bvec_set_length_max(bvec v, idx_t N);
cvec _cvec_set_length_max(cvec v, idx_t N);

#define vec_set_length_max( v, N )  do { v=_vec_set_length_max( v, N ); } while (0)
#define ivec_set_length_max( v, N ) do { v=_ivec_set_length_max( v, N ); } while (0)
#define bvec_set_length_max( v, N ) do { v=_bvec_set_length_max( v, N ); } while (0)
#define cvec_set_length_max( v, N ) do { v=_cvec_set_length_max( v, N ); } while (0)
/*@}*/


/*@{*/
/*! Modify the length of a vector
\warning This function never decreases the max size of the vector
(explictly call Vec_set_length_max for that purpose)
*/
#define Vec_set_length( v, N )      do {                                    \
                                      if( N > Vec_length_max( v ) ) {       \
					Vec_set_length_max( v, N );         \
                                        Vec_length( v ) = (N);              \
                                      } else {                              \
                                      Vec_length( v ) = (N); }              \
                                     } while(0)

vec _vec_set_length(vec v, idx_t N);
ivec _ivec_set_length(ivec v, idx_t N);
bvec _bvec_set_length(bvec v, idx_t N);
cvec _cvec_set_length(cvec v, idx_t N);

#define vec_set_length( v, N )  do { v=_vec_set_length( v, N ); } while (0)
#define ivec_set_length( v, N ) do { v=_ivec_set_length( v, N ); } while (0)
#define bvec_set_length( v, N ) do { v=_bvec_set_length( v, N ); } while (0)
#define cvec_set_length( v, N ) do { v=_cvec_set_length( v, N ); } while (0)
/*@}*/




/*---------------------------------------------------------------------------*/
/*! @name Basic vector initialization/modification                           */
/*---------------------------------------------------------------------------*/

/*@{*/
/*! Initialize the vector from a buffer of the same type */
#define Vec_init( v, buf, N ) memcpy( v, buf, Vec_element_size(v) * (N) );


vec vec_init(vec v, double *buf, idx_t N);
ivec ivec_init(ivec v, int *buf, idx_t N);
bvec bvec_init(bvec v, byte * buf, idx_t N);
cvec cvec_init(cvec v, cplx * buf, idx_t N);
/*@}*/


/*@{*/
/*! Set all the elements of the vector to a given value */
#define Vec_set( v, val ) do {                                              \
                            idx_t i;                                       \
                            assert( v );                                    \
                            for( i = 0 ; i < Vec_length( v ) ; i++ )        \
                              v[ i ] = val; } while( 0 )

vec vec_set(vec v, double val);
ivec ivec_set(ivec v, int val);
bvec bvec_set(bvec v, byte val);
cvec cvec_set(cvec v, cplx val);
/*@}*/


/*@{*/
/*! Constant vector */
vec  vec_new_set(double val, idx_t N);
ivec ivec_new_set(int val, idx_t N);
bvec bvec_new_set(byte val, idx_t N);
cvec cvec_new_set(cplx val, idx_t N);
/*@}*/


/*@{*/
/*! Reduce a vector to length 0 */
#define Vec_void(v) do {                                                  \
  assert( v );                                                            \
  Vec_length( v ) = 0;                                                    \
} while(0)

void vec_void(vec);
void ivec_void(ivec);
void bvec_void(bvec);
void cvec_void(cvec);
/*@}*/


/*@{*/
/*! Create a vector of length 0 */
#define Vec_new_void( type_t ) Vec_new( type_t, 0 )

vec  vec_new_void();
ivec ivec_new_void();
bvec bvec_new_void();
cvec cvec_new_void();
/*@}*/


/*@{*/
/*! Set all the elements of a vector to 0 */
void vec_zeros(vec);
void ivec_zeros(ivec);
void bvec_zeros(bvec);
void cvec_zeros(cvec);
/*@}*/


/*@{*/
/*! Create a vectors of zeros */
vec vec_new_zeros(idx_t N);
ivec ivec_new_zeros(idx_t N);
bvec bvec_new_zeros(idx_t N);
cvec cvec_new_zeros(idx_t N);
/*@}*/


/*@{*/
/*! Set all the vector elements to 1 */
void vec_ones(vec);
void ivec_ones(ivec);
void bvec_ones(bvec);
void cvec_ones(cvec);
/*@}*/


/*@{*/
/*! New vectors of ones */
vec vec_new_ones(idx_t N);
ivec ivec_new_ones(idx_t N);
bvec bvec_new_ones(idx_t N);
cvec cvec_new_ones(idx_t N);
/*@}*/



/*@{*/
/*! Set the elements of a subvector to a given value val */
#define Vec_set_between( v, i1, i2, val ) do {                            \
                            idx_t ve = i2;                                 \
                            idx_t i;                                       \
                            VEC_END_PARAM( v,ve );                          \
                            assert( v );                                    \
                            for( i = i1 ; i <= ve ; i++ )                   \
                              v[ i ] = val; } while( 0 )

vec vec_set_between(vec v, idx_t i1, idx_t i2, double val);
ivec ivec_set_between(ivec v, idx_t i1, idx_t i2, int val);
bvec bvec_set_between(bvec v, idx_t i1, idx_t i2, byte val);
cvec cvec_set_between(cvec v, idx_t i1, idx_t i2, cplx val);

/*@}*/


/*@{*/
/*! Modify a given element subset of a vector to another vector */
#define Vec_set_subvector( v, s, index ) do {				   \
                            idx_t i;					   \
			    assert(v);					   \
			    assert(s);					   \
			    assert(index + Vec_length(s) <= Vec_length(v)); \
			    for(i = 0; i < Vec_length(s); i++)		   \
			      (v)[(index) + i] = (s)[i]; } while(0)


void vec_set_subvector(vec v, vec s, idx_t idx);
void ivec_set_subvector(ivec v, ivec s, idx_t idx);
void bvec_set_subvector(bvec v, bvec s, idx_t idx);
void cvec_set_subvector(cvec v, cvec s, idx_t idx);
/*@}*/


/*@{*/
/*! allocate a new vector formed by a subset of a given vector */
vec vec_get_subvector(vec v, idx_t i1, idx_t i2);
ivec ivec_get_subvector(ivec v, idx_t i1, idx_t i2);
bvec bvec_get_subvector(bvec v, idx_t i1, idx_t i2);
cvec cvec_get_subvector(cvec v, idx_t i1, idx_t i2);
/*@}*/


/*------------------------------------------------------------------------------*/
/*! @name Comparisons functions                                                 */
/*------------------------------------------------------------------------------*/

/*@{*/
/*! Return true iff all the vectors v1 and v2 are equal component per component  */
#define Vec_eq( v1, v2 ) ( Vec_length( v1 ) == Vec_length( v2 ) &&          \
                         !memcmp( v1, v2, Vec_element_size( v1 ) * Vec_length( v1 ) ) )

int  vec_eq(vec v1, vec v2);
int  ivec_eq(ivec v1, ivec v2);
int  bvec_eq(bvec v1, bvec v2);
int  cvec_eq(cvec v1, cvec v2);
/*@}*/


/*@{*/
/* Return 1 if vector v1 is lexicographically greater or equal to vector v2,
0 otherwise. Note that if v1 is a prefix of v2, v1 is assumed to be smaller  */
int  vec_geq(vec v1, vec v2);
int  ivec_geq(ivec v1, ivec v2);
int  bvec_geq(bvec v1, bvec v2);
/*@}*/


/*------------------------------------------------------------------------------*/
/** @name Copy and Conversions Functions                                        */
/*------------------------------------------------------------------------------*/

/*@{*/
/*! Copy the content of a vector into another vector
\warning  Note that these functions do NOT allocate memory for the destination vectors.
Consequently, the amount of allocated elements vec_length_max(v) must be sufficient.
The function returns the number of elements that has really been copied
If the vectors have not the same size, the function returns an error
*/
Vec __Vec_copy(Vec v1, Vec v2);

#define Vec_copy( v1, v2 ) __Vec_copy((Vec) v1, (Vec) v2)
void vec_copy(vec dest, vec orig);
void ivec_copy(ivec dest, ivec orig);
void bvec_copy(bvec dest, bvec orig);
void cvec_copy(cvec dest, cvec orig);
/*@}*/


/*@{*/
/*! Clone a vector (allocate memory and copy data) */
Vec Vec_clone(Vec v);
vec vec_clone(vec v);
ivec ivec_clone(ivec v);
bvec bvec_clone(bvec v);
cvec cvec_clone(cvec v);
/*@}*/


/*@{*/
/*! Copy with conversion (lengths must match) */
void vec_copy_from_ivec(vec dest, ivec orig);
void vec_copy_from_bvec(vec dest, bvec orig);
void vec_copy_from_cvec(vec dest, cvec orig);

void ivec_copy_from_vec(ivec dest, vec orig);
void ivec_copy_from_bvec(ivec dest, bvec orig);
void ivec_copy_from_cvec(ivec dest, cvec orig);

void bvec_copy_from_vec(bvec dest, vec orig);
void bvec_copy_from_ivec(bvec dest, ivec orig);
void bvec_copy_from_cvec(bvec dest, cvec orig);

void cvec_copy_from_vec(cvec dest, vec orig);
void cvec_copy_from_ivec(cvec dest, ivec orig);
void cvec_copy_from_bvec(cvec dest, bvec orig);
/*@}*/


/*@{*/
/*! Vector type conversion */
bvec vec_to_bvec(vec v);
ivec vec_to_ivec(vec v);
cvec vec_to_cvec(vec v);

vec  ivec_to_vec(ivec v);
bvec ivec_to_bvec(ivec v);
cvec ivec_to_cvec(ivec v);

ivec bvec_to_ivec(bvec v);
vec  bvec_to_vec(bvec v);
cvec bvec_to_cvec(bvec v);
/*@}*/


/*@{*/
/*! Copy the vector into a buffer (which must be a valid allocated pointer)      */
void vec_copy_mem(double *buf, vec v);
void ivec_copy_mem(int *buf, ivec v);
void bvec_copy_mem(byte * buf, bvec v);
void cvec_copy_mem(cplx * buf, cvec v);
/*@}*/


/*@{*/
/*! Pack/unpack all bits of a binary vector together */
void bvec_pack(byte * buf, bvec v);
void bvec_unpack(bvec v, byte * buf);
/*@}*/


/*------------------------------------------------------------------------------*/
/** @name Ordered set and stack operations                                      */
/*------------------------------------------------------------------------------*/


/*@{*/
/*! Delete an element from the vector */
#define Vec_del(v, pos) do {                                                \
                               idx_t l = Vec_length(v)-1;                   \
                               idx_t i;                                     \
                               assert(v);                                   \
                               assert((pos) <= l);                          \
                               for( i = (pos) + 1 ; i <= l ; i++ )          \
                                 (v)[ i - 1 ] = (v)[ i ];                   \
                               Vec_length(v)--;                             \
                             } while( 0 )

vec vec_del(vec v, idx_t pos);
ivec ivec_del(ivec v, idx_t pos);
bvec bvec_del(bvec v, idx_t pos);
cvec cvec_del(cvec v, idx_t pos);
/*@}*/


/*@{*/
/*! insert an element in a given vector at the specified position
\warning the function MAY modify the value of the pointer v
The vector max length is increased according to a geometric allocation
*/
#define Vec_ins( v, pos, elt ) do {                                         \
                               idx_t l = Vec_length(v);                     \
                               idx_t i;                                     \
                               assert(v);                                   \
                               assert((pos) <= l);                          \
                               if( l+1 > Vec_length_max(v) )                \
                                 Vec_set_length_max( v, DYN_ALLOC_RULE( l ));\
                               for( i = l ; i >(pos) ; i-- )                \
                                 (v)[i] = (v)[ i-1 ];                       \
                               (v)[(pos)] = (elt);                          \
                               Vec_length(v)++;                             \
                             } while( 0 )

/* To be used as v = ivec_ins( v, pos, elt ) */
vec _vec_ins(vec v, idx_t pos, double elt);
ivec _ivec_ins(ivec v, idx_t pos, int elt);
bvec _bvec_ins(bvec v, idx_t pos, byte elt);
cvec _cvec_ins(cvec v, idx_t pos, cplx elt);

#define vec_ins( v, pos, elt )  do { v=_vec_ins( v, pos, elt ); } while (0)
#define ivec_ins( v, pos, elt ) do { v=_ivec_ins( v, pos, elt ); } while (0)
#define bvec_ins( v, pos, elt ) do { v=_bvec_ins( v, pos, elt ); } while (0)
#define cvec_ins( v, pos, elt ) do { v=_cvec_ins( v, pos, elt ); } while (0)
/*@}*/


/*@{*/
/*! Add an element at the end of the vector */
#define Vec_push( v, elt ) Vec_ins( v, Vec_length(v), elt )

vec _vec_push(vec v, double elt);
ivec _ivec_push(ivec v, int elt);
bvec _bvec_push(bvec v, byte elt);
cvec _cvec_push(cvec v, cplx elt);

#define vec_push( v, elt )  do { v=_vec_push( v, elt ); } while (0)
#define ivec_push( v, elt ) do { v=_ivec_push( v, elt ); } while (0)
#define bvec_push( v, elt ) do { v=_bvec_push( v, elt ); } while (0)
#define cvec_push( v, elt ) do { v=_cvec_push( v, elt ); } while (0)

/*@}*/


/*@{*/
/*! Retrieve the last element at the end of the vector (this removes it) */
#define Vec_pop( v ) Vec_del( v, Vec_length(v)-1 )

vec vec_pop(vec v);
ivec ivec_pop(ivec v);
bvec bvec_pop(bvec v);
cvec cvec_pop(cvec v);
/*@}*/


/*@{*/
/*! Return the last element of vector                                           */
#define Vec_head( v ) ((v)[Vec_length(v)-1])

double vec_head(vec v);
int ivec_head(ivec v);
byte bvec_head(bvec v);
cplx cvec_head(const cvec v);
/*@}*/


/*------------------------------------------------------------------------------*/
/** @name Safe vector access                                                    */
/*------------------------------------------------------------------------------*/

/*@{*/
/*! Safe access to vector's elements */
#define _vec(v, i) (*__vec(v, i))
#define _ivec(v, i) (*__ivec(v, i))
#define _bvec(v, i) (*__bvec(v, i))
#define _cvec(v, i) (*__cvec(v, i))

double *__vec(vec v, idx_t i);
int *__ivec(ivec v, idx_t i);
byte *__bvec(bvec v, idx_t i);
cplx *__cvec(cvec v, idx_t i);
/*@}*/


/*------------------------------------------------------------------------------*/
/** @name Mean, median, min, max, sum and elementary statistic indicators       */
/*------------------------------------------------------------------------------*/

/*@{*/
/*! mean of the input vector */
double vec_mean(vec v);
double ivec_mean(ivec v);
double vec_mean_robust(vec v);
/*@}*/


/*@{*/
/*! median value of the input vector */
double vec_median(vec v);
int  ivec_median(ivec v);
/*@}*/


/*@{*/
/*! vector sum */
double vec_sum(vec v);	/* The sum of all the elements          */
int  ivec_sum(ivec v);	/* use with care: may easily under/overflow */
cplx cvec_sum(cvec v);	/* The sum of all the elements          */
double vec_sum_robust(vec v);
/*@}*/


/*@{*/
/*! cumulative vector sum
\param v the input vector v
\return a new vector of the cumulative sum of the input vector
*/
vec  vec_cum_sum(vec v);
ivec ivec_cum_sum(ivec v);
cvec cvec_cum_sum(cvec v);
/*@}*/


/*@{*/
/*! sum of a subset of the vector specified by an index interval */
double vec_sum_between(vec v, idx_t i1, idx_t i2);
int  ivec_sum_between(ivec v, idx_t i1, idx_t i2);
cplx cvec_sum_between(cvec v, idx_t i1, idx_t i2);
/*@}*/


/*@{*/
/*! The sum of the square components of a vector, i.e. the vector energy */
double vec_sum_sqr(vec v);
/*@}*/


/*@{*/
/*! Mininum of a vector */
double vec_min(vec v);
int  ivec_min(ivec v);
/*@}*/


/*@{*/
/*! Maxinum of a vector */
double vec_max(vec v);
int  ivec_max(ivec v);
/*@}*/


/*@{*/
/*! Position where the vector minimum occurs */
idx_t vec_min_index(vec v);
idx_t ivec_min_index(ivec v);
/*@}*/


/*@{*/
/*! Position where the vector maximum occurs */
idx_t vec_max_index(vec v);
idx_t ivec_max_index(ivec v);
/*@}*/


/*@{*/
/*! Search the k smallest elements
\todo modify the algorithm and use the k-select algorithm
*/
vec  vec_k_min(vec v, int k);
ivec ivec_k_min(ivec v, int k);
/*@}*/


/*@{*/
/*! Search the k greatest elements */
vec  vec_k_max(vec v, int k);
ivec ivec_k_max(ivec v, int k);
/*@}*/


/*@{*/
/*! Search the k smallest elements between two positions */
vec  vec_k_min_between(vec v, int k, idx_t a, idx_t b);
ivec ivec_k_min_between(ivec v, int k, idx_t a, idx_t b);
/*@}*/


/*@{*/
/*! Search the k greatest elements between two positions */
vec  vec_k_max_between(vec v, int k, idx_t a, idx_t b);
ivec ivec_k_max_between(ivec v, int k, idx_t a, idx_t b);
/*@}*/


/*@{*/
/*! Search the indexes where the k smallest elements occur */
ivec vec_k_min_index(vec v, int k);
ivec ivec_k_min_index(ivec v, int k);
/*@}*/


/*@{*/
/*! Search the indexes where the k greatest elements occur */
ivec vec_k_max_index(vec v, int k);
ivec ivec_k_max_index(ivec v, int k);
/*@}*/


/*@{*/
/*! Search the indexes where the k smallest elements between two positions occur */
ivec vec_k_min_index_between(vec v, int k, idx_t a, idx_t b);
ivec ivec_k_min_index_between(ivec v, int k, idx_t a, idx_t b);
/*@}*/


/*@{*/
/*! Search the indexes where the k greatest elements between two positions occur */
ivec vec_k_max_index_between(vec v, int k, idx_t a, idx_t b);
ivec ivec_k_max_index_between(ivec v, int k, idx_t a, idx_t b);
/*@}*/


/*@{*/
/*! Unbiaised variance of the vector (obtained by dividing by N-1) */
double vec_variance(vec v);
double vec_variance_robust(vec v);
/*@}*/

/*@{*/
/*! Covariance between two vectors */
double vec_cov(vec v1, vec v2);
/*@}*/

/*@{*/
/*! Norm of the input vector */
double vec_norm(vec v, double nr);
double ivec_norm(ivec v, int nr);
/*@}*/


/*@{*/
/*! Normalize the vector for a specified norm, so that $sum(||v||_nr)=1$ */
void vec_normalize(vec v, double nr);
/*@}*/


/*@{*/
/*! Return a normalized version of the input vector */
vec vec_new_normalize(vec v, double nr);
/*@}*/



/*------------------------------------------------------------------------------*/
/** @name Basic arithmetic operations                                           */
/*------------------------------------------------------------------------------*/


/*@{*/
/*! Negate the vector */
void vec_neg(vec v);
void ivec_neg(ivec v);
void cvec_neg(cvec v);
/*@}*/


/*@{*/
/*! Complex conjugate */
void cvec_conj(cvec v);
/*@}*/


/*@{*/
/*! elementwise |v_i|^2 */
void cvec_abssqr(cvec v);
/*@}*/


/*@{*/
/*! square the vector component per component */
void vec_sqr(vec v);
void ivec_sqr(ivec v);
/*@}*/


/*@{*/
/*! root square of a vector component per component */
void vec_sqrt(vec v);
/*@}*/

/*@{*/
/*! logarithm of a vector component per component */
void vec_log(vec v);
void vec_log10(vec v);
/*@}*/


/*@{*/
/*! apply exponential */
void vec_exp(vec v);
/*@}*/


/*@{*/
/*! set each component c to c^a */
void vec_pow(vec v, double a);
/*@}*/


/*@{*/
/*! new vector: exponential values of the input vector */
vec vec_new_pow(vec v, double a);
/*@}*/


/*@{*/
/*! Apply absolute value */
void vec_abs(vec v);
void ivec_abs(ivec v);
/*@}*/


/*@{*/
/*! New vector generated with the absolute values of the input vector */
vec  vec_new_abs(vec v);
ivec ivec_new_abs(ivec v);
vec  cvec_new_abs(cvec v);
/*@}*/



/*------------------------------------------------------------------------------*/
/** @name Arithmetic operations with a scalar value                             */
/*------------------------------------------------------------------------------*/

/*@{*/
/*! add a to the components */
void vec_incr(vec v, double a);
void ivec_incr(ivec v, int a);
void cvec_incr(cvec v, cplx a);
void cvec_incr_real(cvec v, double a);
/*@}*/


/*@{*/
/*! substract a specified value to the vector components */
void vec_decr(vec v, double a);
void ivec_decr(ivec v, int a);
void cvec_decr(cvec v, cplx a);
/*@}*/


/*@{*/
/*! substract a specified real value to the complex components */
void cvec_decr_real(cvec v, double a);
/*@}*/


/*@{*/
/*! multiply the components per a specified value */
void vec_mul_by(vec v, double a);
void ivec_mul_by(ivec v, int a);	/* multiply the components per a        */
void cvec_mul_by(cvec v, cplx a);	/* multiply the components per a        */
									/*@}*/


									/*@{*/
									/*! multiply the components per a */
void cvec_mul_by_real(cvec v, double a);
/*@}*/


/*@{*/
/*! divide the components per a */
void vec_div_by(vec v, double a);
void ivec_div_by(ivec v, int a);
void cvec_div_by(cvec v, cplx a);
/*@}*/


/*@{*/
void cvec_div_by_real(cvec v, double a);
/*@}*/


/*------------------------------------------------------------------------------*/
/** @name Component per component operations                                    */
/*------------------------------------------------------------------------------*/

/*@{*/
/*! addition */
void vec_add(vec v1, vec v2);
void ivec_add(ivec v1, ivec v2);
void cvec_add(cvec v1, cvec v2);
/*@}*/


/*@{*/
/*! addition stored in a new vector */
vec  vec_new_add(vec v1, vec v2);
ivec ivec_new_add(ivec v1, ivec v2);
/*@}*/


/*@{*/
/*! Substraction */
void vec_sub(vec v1, vec v2);
void ivec_sub(ivec v1, ivec v2);
void cvec_sub(cvec v1, cvec v2);
/*@}*/


/*@{*/
/*! Substraction */
vec  vec_new_sub(vec v1, vec v2);
ivec ivec_new_sub(ivec v1, ivec v2);
/*@}*/


/*@{*/
/*! Component per component multiplication

The result is stored in the vector which is the first parameter
*/
void vec_mul(vec v1, vec v2);
void ivec_mul(ivec v1, ivec v2);
void cvec_mul(cvec v1, cvec v2);
/*@}*/


/*@{*/
/*! Component per component multiplication */
vec  vec_new_mul(vec v1, vec v2);
ivec ivec_new_mul(ivec v1, ivec v2);
/*@}*/



/*@{*/
/*! Component per component division */
void vec_div(vec v1, vec v2);
void ivec_div(ivec v1, ivec v2);
void cvec_div(cvec v1, cvec v2);
/*@}*/


/*@{*/
/*! Component per component division */
vec  vec_new_div(vec v1, vec v2);
ivec ivec_new_div(ivec v1, ivec v2);
/*@}*/


/*@{*/
/*! Inner product of two vectors */
double vec_inner_product(vec v1, vec v2);
int  ivec_inner_product(ivec v1, ivec v2);
double vecivec_inner_product(vec v1, ivec v2);
int  bvecivec_inner_product(bvec v1, ivec v2);
/*@}*/


/*@{*/
/*! Robustly computed inner product of two vectors */
double vec_inner_product_robust(vec v1, vec v2);
/*@}*/


/*@{*/
/*! Hermitian product of two complex vectors */
void cvec_conj_mul(cvec v1, cvec v2);
/*@}*/



/*-----------------------------------------------------------------*/
/** @name Component per component arbitrar function                */
/*-----------------------------------------------------------------*/

/*@{*/
/*! Apply a function to all vector elements  */
void  vec_apply_function(vec v, it_function_t function, it_args_t args);
/*@}*/

/*@{*/
/*! Apply a function to all vector elements  */
vec  vec_new_apply_function(vec v, it_function_t function, it_args_t args);
/*@}*/

#define vec_eval(v, f, a) vec_apply_function(v, f, a)
#define vec_new_eval(v, f, a) vec_new_apply_function(v, f, a)


/*@{*/
/*! Apply a function to all vector elements  */
ivec ivec_apply_function(ivec v, it_ifunction_t function, it_args_t args);
/*@}*/

/*@{*/
/*! Apply a function to all vector elements  */
ivec ivec_new_apply_function(ivec v, it_ifunction_t function,
	/*@}*/
	it_args_t args);
#define ivec_eval(v, f, a) ivec_apply_function(v, f, a)
#define ivec_new_eval(v, f, a) ivec_new_apply_function(v, f, a)


/*-----------------------------------------------------------------*/
/** @name Set, sorting and finding functions                       */
/*-----------------------------------------------------------------*/

/*@{*/
/*! Reverse the vector */
void vec_reverse(vec v);
void ivec_reverse(ivec v);
void bvec_reverse(bvec v);
void cvec_reverse(cvec v);
/*@}*/


/*@{*/
/*! Reverse the vector */
vec  vec_new_reverse(vec v);
ivec ivec_new_reverse(ivec v);
bvec bvec_new_reverse(bvec v);
cvec cvec_new_reverse(cvec v);
/*@}*/


/*@{*/
/*! Count the number of elements equal to a given value a */
idx_t vec_count(vec v, double a);
idx_t ivec_count(ivec v, int a);
idx_t bvec_count(bvec v, byte a);
idx_t cvec_count(cvec v, cplx a);
/*@}*/


/*@{*/
/*! Return the first position where the value a occurs, NULL_INDEX
if the research is not successful                               */
idx_t vec_find_first(vec v, double a);
idx_t ivec_find_first(ivec v, int a);
idx_t bvec_find_first(bvec v, byte a);
idx_t cvec_find_first(cvec v, cplx a);
/*@}*/


/*@{*/
/*! Return the set of positions where a given value occurs */
ivec vec_find(vec v, double a);
ivec ivec_find(ivec v, int a);
ivec bvec_find(bvec v, byte a);
ivec cvec_find(cvec v, cplx a);
/*@}*/


/*@{*/
/*! Return the set of positions where a given value occurs assuming a sorted set */
idx_t vec_find_sorted(vec v, double a);
idx_t ivec_find_sorted(ivec v, int a);
idx_t bvec_find_sorted(bvec v, byte a);
/*@}*/


/*@{*/
/*! Replace a given value by another for all vector components */
ivec vec_replace(vec v, double a, double b);
ivec ivec_replace(ivec v, int a, int b);
ivec bvec_replace(bvec v, byte a, byte b);
ivec cvec_replace(cvec v, cplx a, cplx b);
/*@}*/


/*@{*/
/*! Concatenate two vectors */
vec  vec_new_concat(vec v1, vec v2);
ivec ivec_new_concat(ivec v1, ivec v2);
bvec bvec_new_concat(bvec v1, bvec v2);
cvec cvec_new_concat(cvec v1, cvec v2);
/*@}*/


/*@{*/
/*! Vector created from the input vector by taking one occurences
of each possible value of the input vector */
vec  vec_new_unique(vec v);
ivec ivec_new_unique(ivec v);
bvec bvec_new_unique(bvec v);
/*@}*/


/*@{*/
/*! Union of two sets represented by the two input vectors */
vec  vec_new_union(vec v1, vec v2);
ivec ivec_new_union(ivec v1, ivec v2);
bvec bvec_new_union(bvec v1, bvec v2);
/*@}*/


/*@{*/
/*! Intersection of two sets represented by the two input vectors */
vec  vec_new_intersection(vec v1, vec v2);
ivec ivec_new_intersection(ivec v1, ivec v2);
bvec bvec_new_intersection(bvec v1, bvec v2);
/*@}*/


/*@{*/
/*! Return the vector composed of the elements of v indexed by idx */
vec  vec_index_by(vec v, ivec idx);
ivec ivec_index_by(ivec v, ivec idx);
bvec bvec_index_by(bvec v, ivec idx);
cvec cvec_index_by(cvec v, ivec idx);
/*@}*/


/*@{*/
/*! Sort the vector by increasing values */
void Vec_sort(Vec v, int(*elem_leq) (const void *, const void *));
void vec_sort(vec v);
void ivec_sort(ivec v);
void bvec_sort(bvec v);
/*@}*/


/*@{*/
/*! Return a vector of index corresponding to increasing values of the input vector */
ivec Vec_sort_index(Vec v,
	int(*elem_leq_idx) (const void *, const void *));
ivec vec_sort_index(vec v);
ivec ivec_sort_index(ivec v);
ivec bvec_sort_index(bvec v);
/*@}*/



/*-----------------------------------------------------------------*/
/** @name Special vectors                                          */
/*-----------------------------------------------------------------*/


/*@{*/
/*! Set the vector elements to 0,1, ... n-1 */
void vec_range(vec);
void ivec_range(ivec);
void bvec_range(bvec);
void cvec_range(cvec);
/*@}*/


/*@{*/
/*! New vector 0, 1, ... N-1 */
vec  vec_new_range(idx_t n);
ivec ivec_new_range(idx_t n);
bvec bvec_new_range(idx_t n);
cvec cvec_new_range(idx_t n);
/*@}*/


/*@{*/
/*! Set the vector elements to 1,2, ... n */
void vec_1N(vec);
void ivec_1N(ivec);
void bvec_1N(bvec);
void cvec_1N(cvec);
/*@}*/


/*@{*/
/*! The sequence 1, 2.. N */
vec  vec_new_1N(idx_t n);
ivec ivec_new_1N(idx_t n);
bvec bvec_new_1N(idx_t n);
cvec cvec_new_1N(idx_t n);
/*@}*/


/*@{*/
/*! Set the vector to an arithmetic sequence */
void vec_arithm(vec, double start, double incr);
void ivec_arithm(ivec, int start, int incr);
void bvec_arithm(bvec, byte start, byte incr);
void cvec_arithm(cvec, cplx start, cplx incr);
/*@}*/


/*@{*/
/*! New arithmetic sequence */
vec  vec_new_arithm(double start, double incr, idx_t N);
ivec ivec_new_arithm(int start, int incr, idx_t N);
bvec bvec_new_arithm(byte start, byte incr, idx_t N);
cvec cvec_new_arithm(cplx start, cplx incr, idx_t N);
/*@}*/


/*@{*/
/*! Set the vector to an geometric sequence */
void vec_geom(vec, double start, double r);
void ivec_geom(ivec, int start, int r);
void bvec_geom(bvec, byte start, byte r);
void cvec_geom(cvec, cplx start, cplx r);
/*@}*/


/*@{*/
/*! Same but a geometric sequence of geometric factor r */
vec  vec_new_geom(double start, double r, idx_t N);
ivec ivec_new_geom(int start, int r, idx_t N);
bvec bvec_new_geom(byte start, byte r, idx_t N);
cvec cvec_new_geom(cplx start, cplx r, idx_t N);
/*@}*/


/*@{*/
/*! generate $e^{2i Pi k / N} = cos(2 k Pi / N) + i sin(2 k Pi / N)$ */
cvec cvec_new_unit_roots(idx_t N);
/*@}*/


/*-----------------------------------------------------------------*/
/** @name Random vectors                                           */
/*-----------------------------------------------------------------*/

/*! Draw the component of a vector to uniformally random variable in [0,1[ */
void vec_rand(vec v);

/*! Generate a random vector, uniformally drawn in [0,1[ */
vec  vec_new_rand(idx_t N);

/*! Generate a random vector, uniformally drawn in [0,1[ */
void vec_randn(vec v);

/*! Generate a random vector with normal law */
vec  vec_new_randn(idx_t N);

/*! Generate a new vector representing a random permutation */
ivec ivec_new_perm(size_t N);


/*-----------------------------------------------------------------*/
/** @name Convolution                                              */
/*-----------------------------------------------------------------*/

/*@{*/
/*! Convolution of two vectors */
vec  vec_conv(vec v1, vec v2);
ivec ivec_conv(ivec v1, ivec v2);
/*@}*/


/*-----------------------------------------------------------------*/
/** @name Vector <-> sparse vectors conversion                     */
/*-----------------------------------------------------------------*/

/*@{*/
/*! convert a vector into a sparse vector */
void vec_to_spvec(vec v, ivec * svi_out, vec * sv_out);
void ivec_to_spivec(ivec v, ivec * svi_out, ivec * sv_out);
/*@}*/


/*@{*/
/*! convert a sparse vector into a vector */
vec spvec_to_vec(ivec svi, vec sv, int n);
ivec spivec_to_ivec(ivec svi, ivec sv, int n);
/*@}*/


/*@{*/
/*! Inner product between two sparse vectors */
double spvec_inner_product(ivec svi1, vec sv1, ivec svi2, vec sv2);
int spivec_inner_product(ivec svi1, ivec sv1, ivec svi2, ivec sv2);
/*@}*/


/*@}*/

#endif

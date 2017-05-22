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
Various Souce functions
Copyright (C) 2005 Vivien Chappelier, Herve Jegou
*/


#ifndef __it_source_func_h
#define __it_source_func_h

#include "vec.hpp"
#include "mat.hpp"

/*! Return the entropy of a memoryless discrete source */
double entropy(vec pdf);

/*! Entropy rate of a Markov chain defined by transition matrix pt */
double entropy_markov(mat pt);

/*! Return the  entropy of a binary source source */
double entropy_bin(double p);

/*! Return the histogram of the realization S.
The source is assumed to take its values between 0 and omega-1 */
ivec histogram(int omega, ivec S);

/*! Same as histogram return a normalized histogram, i.e. a pdf */
vec  histogram_normalized(int omega, ivec S);

/*! Return the conditionnal (i.e., bi-dimensional) histogram of a source realization */
imat histogram_cond(int omega, ivec S);

/*! Return the expectation of a source defined by the pdf and the symbols values */
double source_expectation(vec pdf, vec symbols);

/*! Return the variance of a source defined by its pdf and its symbols */
double source_variance(vec pdf, vec symbols);

/*! Verify if the input vector is a valid probability density function */
int  is_valid_pdf(vec pdf, double tol);

/*! Check if pt is a valid matrix of transition probabilities (Markov chain) */
int  is_valid_markov_matrix(mat pt, double tol);

/*! Return the stationary probability of a Markov chain defined by transition matrix pt */
vec  markov_marg_pdf(mat pt);


#endif

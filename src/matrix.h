/*
 * matrix.h
 *
 * Copyright (c) 2014 Ben Hamlin <protob3n@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ┏━┓┏━┓╻┏┳┓
 * ┃┓┃┗━┓┃┃┃┃
 * ┗┻┛┗━┛╹╹ ╹
 *
 */

#ifndef QSIM_MATRIX_H
#define QSIM_MATRIX_H

#include <complex.h>
#include <math.h>
#include <stdbool.h>

#include "common.h"

typedef struct matrix {
  unsigned h, w;
  struct matrix *next;
  complex e[];
} *M;

#define elem(m, x, y) ((m)->e[(y) * (m)->w + (x)])
#define mdata(...) (complex[]){__VA_ARGS__}

/*
 * Global quantum operations I2, X, Y, Z, H, S, T, and CX. Must be initialized
 * with init_global_qops() before use.
 */
extern M I2, X, Y, Z, H, S, T, CX;

/*
 * Initialize an hxw matrix. A matrix stack must be created beforehand.
 */
M m_init(unsigned h, unsigned w);

/*
 * Create and initialize a matrix with the specified data. A matrix stack should be
 * initialized first.
 */
M m_create(unsigned h, unsigned w, complex *);

/*
 * Copy a matrix and push the new copy onto the stack.
 */
M m_copy(M m);

/*
 * Return *C <= *A x *B. A matrix stack must be allocated beforehand.
 */
M m_mult(M A, M B);

/*
 * Return *B = k(*A) (e.i., scalar multiplication) A matrix stack must be
 * yadda, yadda...
 */
M m_scalar_mult(complex k, M A);

/*
 * Return *C <= *A + *B
 */
M m_add(M A, M B);

/*
 * Return *C <= *A - *B
 */
M m_sub(M A, M B);

/*
 * Return *C = *A (x) *B. A matrix stack must be created first.
 */
M m_tensor(M A, M B);

/*
 * Return the conjugate transpose. Don't forget the matrix stack.
 */
M m_adjoint(M A);

/*
 * Return true iff A == B.
 */
bool m_equal(M A, M B);

/*
 * Print a matrix.
 */
void m_print(M m);

/*
 * Create a new frame on the matrix stack.
 */
int m_stack_push();

/*
 * Clear the current frame on the matrix stack, freeing all matrices.
 */
void m_stack_clear();

/*
 * Pop the current frame off the matrix stack, freeing all matrices. Return the
 * number of frames currently on the matrix stack.
 */
int m_stack_pop();

/*
 * Initialize global operations I2, X, Y, Z, H, S, T, and CX. A matrix stack
 * must be in place beforehand.
 */
void init_global_qops();

/*
 * Generate rotation operations about the x, y, and z axes of the bloch sphere.
 */
M qop_rotate_x(double angle);
M qop_rotate_y(double angle);
M qop_rotate_z(double angle);

#endif /* QSIM_MATRIX_H */

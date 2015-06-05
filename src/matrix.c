/*
 * matrix.c
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

#include "matrix.h"

M I2, X, Y, Z, H, S, T, CX;

struct m_stack {
  int count;
  struct matrix *top;
  struct m_stack *next;
} *stack = NULL;

void m_push(M m) {
  M old_top = stack->top;
  stack->top = m;
  m->next = old_top;
}

M m_init(unsigned h, unsigned w) {
  assert(h > 0 && w > 0);
  assert(stack != NULL);

  int i;
  M res = malloc_or_die(sizeof(*res) + h * w * sizeof(complex));

  res->h = h;
  res->w = w;
  for(i = 0; i < w * h; ++i)
    res->e[i] = 0;

  m_push(res);
  return res;
}

M m_create(unsigned h, unsigned w, complex *data) {
  int i;
  M res = m_init(h, w);

  for(i = 0; i < h * w; ++i)
    res->e[i] = data[i];

  return res;
}

M m_copy(M m) {
  return m_create(m->h, m->w, m->e);
}

M m_mult(M A, M B) {
  assert(A->w == B->h);

  unsigned x, y, h = A->h, w = B->w;
  unsigned ax, by, v = A->w;
  M res = m_init(h, w);

  for(y = 0; y < h; ++y)
    for(x = 0; x < w; ++x) {
      elem(res, x, y) = 0;

      for(ax = 0, by = 0; ax < v; ++ax, ++by)
        elem(res, x, y) += elem(A, ax, y) * elem(B, x, by);
    }

  return res;
}

M m_scalar_mult(complex k, M A) {
  int i;
  M res = m_copy(A);

  for(i = 0; i < res->w * res->h; ++i)
    res->e[i] *= k;

  return res;
}

M m_add(M A, M B) {
  assert(A->w == B->w && A->h == B->h);

  unsigned x, y, h = A->h, w = A->w;
  M res = m_init(h, w);

  for(y = 0; y < h; ++y)
    for(x = 0; x < w; ++x)
      elem(res, x, y) += elem(A, x, y) + elem(B, x, y);

  return res;
}

M m_sub(M A, M B) {
  assert(A->w == B->w && A->h == B->h);

  unsigned x, y, h = A->h, w = A->w;
  M res = m_init(h, w);

  for(y = 0; y < h; ++y)
    for(x = 0; x < w; ++x)
      elem(res, x, y) += elem(A, x, y) - elem(B, x, y);

  return res;
}

M m_tensor(M A, M B) {
  unsigned w = A->w * B->w, h = A->h * B->h;
  unsigned ax, ay, bx, by;
  M res = m_init(h, w);

  for(ay = 0; ay < A->h; ++ay)
    for(ax = 0; ax < A->w; ++ax)
      for(by = 0; by < B->h; ++by)
        for(bx = 0; bx < B->w; ++bx)
          elem(res, ax * B->w + bx, ay * B->h + by) = elem(A, ax, ay) * elem(B, bx, by);

  return res;
}

M m_adjoint(M A) {
  M res = m_init(A->w, A->h);
  unsigned x, y;

  for(y = 0; y < A->h; ++y)
    for(x = 0; x < A->w; ++x)
      elem(res, y, x) = conj(elem(A, x, y));

  return res;
}

bool m_equal(M A, M B) {
  assert(A->h == B->h && A->w == B->w);

  unsigned h = A->h, w = A->w, x, y;

  for(y = 0; y < h; ++y)
    for(x = 0; x < w; ++x)
      if(!CEQ(elem(A, x, y), elem(B, x, y)))
        return false;

  return true;
}

void m_print(M m) {
  unsigned x, y;
  char buf[256];

  for(y = 0; y < m->h; ++y)
    for(x = 0; x < m->w; ++x)
      if(x < m->w - 1)
        printf("%20s,", complex_to_str(buf, 256, elem(m, x, y)));
      else
        printf("%20s\n", complex_to_str(buf, 256, elem(m, x, y)));
}

int m_stack_push() {
  struct m_stack *old_top = stack;

  stack = malloc_or_die(sizeof(struct m_stack));
  stack->next = old_top;
  stack->top = NULL;
  stack->count = (old_top) ? (old_top->count + 1) : 1;

  return stack->count;
}

void m_stack_clear() {
  M cur;

  while(stack->top) {
    cur = stack->top;
    stack->top = stack->top->next;
    free(cur);
  }
}

int m_stack_pop() {
  struct m_stack *old_top;

  if(stack) {
    m_stack_clear();
    old_top = stack;
    stack = stack->next;
    free(old_top);
  }

  return (stack) ? (stack->count) : 0;
}

void init_global_qops() {
  static bool init = false;

  if(!init) { /* Only do this once, ever. */
    I2 = m_create(2, 2, mdata(1, 0,
                              0, 1));

    X = m_create(2, 2, mdata(0, 1,
                             1, 0));

    Y = m_create(2, 2, mdata(0, -I,
                             I, 0));

    Z = m_create(2, 2, mdata(1,  0,
                             0, -1));

    H = m_create(2, 2, mdata(M_SQRT1_2,  M_SQRT1_2,
                             M_SQRT1_2, -M_SQRT1_2));

    S = m_create(2, 2, mdata(1, 0,
                             0, I));

    T = m_create(2, 2, mdata(1, 0,
                             0, csqrt(I)));

    CX = m_create(4, 4, mdata(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 0, 1,
                              0, 0, 1, 0));

    init = true;
  }
}

M qop_rotate_x(double angle) {
  return m_create(2, 2, mdata(cos(angle / 2),     -I * sin(angle / 2),
                              -I * sin(angle / 2), cos(angle / 2)));
}

M qop_rotate_y(double angle) {
  return m_create(2, 2, mdata(cos(angle / 2), -sin(angle / 2),
                              sin(angle / 2), cos(angle / 2)));
}

M qop_rotate_z(double angle) {
  return m_create(2, 2, mdata(cexp(-I * (angle / 2)), 0,
                              0,                     cexp(I * (angle / 2))));
}

M qop_make_cnot(unsigned width, unsigned control, unsigned target) {
  assert(width < 32);

  unsigned d = 1 << width, c = 1 << control, t = 1 << target, x, y;
  complex e[d * d];

  for(y = 0; y < d; ++y)
    for(x = 0; x < d; ++x)
      if(y & c && x == (y ^ t))
        e[y * d + x] = 1;
      else if(!(y & c) && y == x)
        e[y * d + x] = 1;
      else
        e[y * d + x] = 0;

  return m_create(d, d, e);
}

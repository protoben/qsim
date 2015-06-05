/*
 * test_matrix.c
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

#define __USE_XOPEN
#include "../src/matrix.h"

int main(void) {
  m_stack_push();
  init_global_qops();

  unsigned x, y;
  M m = m_init(4, 1);

  for(y = 0; y < m->h; ++y)
    for(x = 0; x < m->w; ++x)
      elem(m, x, y) = x + y * I;

  m_print(m);
  puts("");
  m_print(m_adjoint(m));
  puts("");
  m_print(m_tensor(H, X));
  puts("");
  m_print(m_mult(Z, Y));
  puts("");
  m_print(m_mult(m_tensor(H, H), m_mult(CX, m_tensor(H, H))));
  puts("");
  m_print(qop_make_cnot(2, 1, 0));

  if(!m_equal(m_mult(H, m_adjoint(H)), I2))
    puts("Something is wrong with the world");

  m_stack_pop();
  return 0;
}

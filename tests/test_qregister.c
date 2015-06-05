/*
 * test_qregister.c
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

#include "../src/qregister.h"

int main(void) {
  m_stack_push();
  init_global_qops();

  qreg qr;

  qr_init(&qr, 2, 0x3);
  qr_print(&qr);
  qr_evolve(&qr, m_tensor(H, H));
  qr_print(&qr);
  qr_evolve(&qr, m_tensor(X, I2));
  qr_print(&qr);
  qr_evolve(&qr, m_tensor(H, I2));
  qr_print(&qr);
  qr_evolve(&qr, m_tensor(I2, H));
  qr_print(&qr);
  qr_free(&qr);

  puts("");

  qr_init(&qr, 5, 0x1);
  qr_print(&qr);
  qr_free(&qr);

  m_stack_pop();
  return 0;
}

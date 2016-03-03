/*
 * qregister.c
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

#include "qregister.h"

void qr_init(qreg *qr, unsigned width, unsigned value) {
  assert(width <= 32);
  assert(value <= ~(((unsigned)-1) << width));

  qr->width = width;
  qr->amplitudes = malloc_or_die(sizeof *qr->amplitudes);
  qr->amplitudes->basis = value;
  qr->amplitudes->amplitude = 1;
  qr->amplitudes->next = NULL;
}

void qr_bases_free(struct basis *head) {
  struct basis *temp;

  while(head) {
    temp = head;
    head = head->next;
    free(temp);
  }
}

void qr_free(qreg *qr) {
  qr_bases_free(qr->amplitudes);
  qr->amplitudes = NULL;
}

void qr_print(qreg *qr) {
  char buf1[64], buf2[64];
  struct basis *cur;

  for(cur = qr->amplitudes; cur; cur = cur->next) {
    printf("(%s) %s", complex_to_str(buf1, 64, cur->amplitude),
                      ket_to_str(buf2, 64, cur->basis, qr->width));
    if(cur->next)
      fputs(" + ", stdout);
  }
}

void qr_bases_add(qreg *qr, unsigned basis, complex amp) {
  struct basis *cur, *prev, *new = malloc_or_die(sizeof *new);

  new->basis = basis;
  new->amplitude = amp;

  if(!qr->amplitudes || qr->amplitudes->basis > basis) {
    new->next = qr->amplitudes;
    qr->amplitudes = new;
    return;
  }

  for(cur = qr->amplitudes; cur; prev = cur, cur = cur->next)
    if(cur->basis > basis)
      break;

  assert(prev->basis != basis);

  prev->next = new;
  new->next = cur;
}

void qr_evolve(qreg *qr, M op) {
  assert(op->w == op->h);
  assert(op->w == 1 << qr->width);

  unsigned x, y, v = op->w;
  complex acc;
  struct basis *cur, *old_amps;

  old_amps = qr->amplitudes;
  qr->amplitudes = NULL;

  for(y = 0; y < v; ++y) {
    cur = old_amps;
    acc = 0;

    for(x = 0; x < v && cur; ++x)
      if(cur->basis == x) {
        acc += cur->amplitude * elem(op, x, y);
        cur = cur->next;
      }

    if(!CEQ(acc, 0))
      qr_bases_add(qr, y, acc);
  }

  qr_bases_free(old_amps);
}

unsigned qr_measure_all(qreg *qr) {
  double r = drand48(), p;
  struct basis *cur = qr->amplitudes, *temp, *save = NULL;
  bool done = false;

  while(cur) {
    p = cabs(cur->amplitude);
    p *= p;

    temp = cur->next;
    if(r <= p && !done) {
      save = cur;
      done = true;
    } else {
      free(cur);
      r -= p;
    }
    cur = temp;
  }

  assert(save != NULL);

  save->amplitude = 1;
  save->next = NULL;
  qr->amplitudes = save;

  return qr->amplitudes->basis;
}

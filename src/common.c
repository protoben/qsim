/*
 * common.c
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

#include "common.h"

void die(int err, const char *fmt, ...) {
  va_list ap;
  char buf[1024];

  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  if(err) {
    errno = err;
    perror(buf);
  } else {
    fprintf(stderr, "%s\n", buf);
  }

  exit(1);
}

void *malloc_or_die(size_t s) {
  void *p;
  
  errno = ENOMEM;
  p = malloc(s);
  if(!p)
    die(errno, "malloc(%lu)", s);

  return p;
}

unsigned strtou_or_die(char *start, char **endp, int base) {
  unsigned long res;

  errno = 0;
  res = strtoul(start, endp, base);
  if(errno)
    die(errno, "strtoul(%s)", start ? start : "NULL");
  if(res > UINT_MAX)
    die(EOVERFLOW, "strtou_or_die(%s)", start);

  return res;
}

FILE *fopen_or_die(const char *fname, const char *mode) {
  FILE *fp;

  fp = fopen(fname, mode);
  if(!fp)
    die(errno, "fopen(%s, %s)", fname, mode);

  return fp;
}

char *complex_to_str(char *buf, size_t s, complex c) {
  if(DEQ(cimag(c), 0))
    snprintf(buf, s, "%.*g", DBL_PRECISION, creal(c));
  else if(DEQ(creal(c), 0))
    snprintf(buf, s, "%.*gi", DBL_PRECISION, cimag(c));
  else
    snprintf(buf, s, "%.*g + %.*gi", DBL_PRECISION, creal(c), DBL_PRECISION, cimag(c));

  return buf;
}

char *ket_to_str(char *buf, size_t s, uint32_t basis, unsigned width) {
  assert(s >= width + 3);
  assert(width <= 32);
  assert(width > 0);

  size_t i = 0;
  uint32_t mask = 0x1 << (width - 1);

  buf[i++] = '|';
  while(i <= width) {
    if(basis & mask)
      buf[i++] = '1';
    else
      buf[i++] = '0';

    mask = mask >> 1;
  }
  buf[i++] = '>';
  buf[i] = '\0';

  return buf;
}

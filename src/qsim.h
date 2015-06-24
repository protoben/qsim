/*
 * qsim.h
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

#ifndef QSIM_H
#define QSIM_H

#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "matrix.h"
#include "qregister.h"
#include "common.h"

#define OPTSTRING "sf:h"
#define PUSAGE(name) fprintf(stderr, \
    "qsim: A quantum circuit simulator.\n\n" \
    "Usage: %s [-sh] -f <filename> <input>\n" \
    "\t-s\tSingle-step mode. Intermediate amplitudes will be printed.\n" \
    "\t-f\tSpecify the .q or .Q file to read from (required, for now).\n" \
    "\t-h\tPrint this message.\n" \
, name)

#ifdef PRINTF_DEBUG
# define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
# define DEBUG(...)
#endif

#define CSD "[[:digit:]]+(,[[:space:]]*[[:digit:]]+)*"
#define GATE "(X|x|not|Y|y|Z|z|H|hadamard|h|S|phase|s|T|t|CX|CNOT|cx|cnot)\\*?"
#define GATE_LINE_REGEX "^[[:space:]]*" GATE "[[:space:]]+" CSD "[[:space:]]*$"

#define REGISTER "(REGISTER|REG|register|reg)"
#define REG_LINE_REGEX "^[[:space:]]*" REGISTER "[[:space:]]+[[:digit:]]+[[:space:]]*$"

#endif /* QSIM_H */

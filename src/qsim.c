/*
 * qsim.c
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

#include "qsim.h"

struct {
  bool singlestep;
  const char *filename;
  unsigned input;
} opts = {0};

void parseargs(int argc, char **argv) {
  int c;

  while((c = getopt(argc, argv, OPTSTRING)) != -1)
    switch(c) {
      case 'f': opts.filename = optarg; break;
      case 's': opts.singlestep = true; break;
      case 'h': PUSAGE(argv[0]); exit(0);
      default: PUSAGE(argv[0]); exit(1);
    }

  if(!opts.filename || !argv[optind]) {
    PUSAGE(argv[0]);
    exit(1);
  }

  opts.input = strtou_or_die(argv[optind], NULL, 0);
}

int parse_operands(char *buf, unsigned *oplist, unsigned maxops) {
  int i;

  while(*buf && !isdigit(*buf))
    ++buf;

  for(i = 0; *buf && i < maxops; ++i) {
    oplist[i] = strtou_or_die(buf, NULL, 0);
    while(*buf && isdigit(*buf))
      ++buf;
    while(*buf && !isdigit(*buf))
      ++buf;
  }

  while(*buf && !isdigit(*buf))
    ++buf;
  if(isdigit(*buf))
    return maxops + 1;

  return i;
}

bool bit_in_ops(unsigned bit, unsigned *ops, unsigned nops) {
  unsigned i;

  for(i = 0; i < nops; ++i)
    if(bit == ops[i])
      return true;

  return false;
}

void do_gate(qreg *regp, char *buf, unsigned line) {
  M gate, compound_gate;
  unsigned noperands, operands[regp->width];
  int i;

  m_stack_push();

  while(isspace(*buf))
    ++buf;

  switch(*buf) {
    case 'X': case 'x': case 'n':
      gate = X;
      DEBUG("Got X gate ");
      break;
    case 'Y': case 'y':
      gate = Y;
      DEBUG("Got Y gate ");
      break;
    case 'Z': case 'z':
      gate = Z;
      DEBUG("Got Z gate ");
      break;
    case 'H': case 'h':
      gate = H;
      DEBUG("Got H gate ");
      break;
    case 'S': case 's': case 'p':
      gate = S;
      DEBUG("Got S gate ");
      break;
    case 'T': case 't':
      gate = T;
      DEBUG("Got T gate ");
      break;
    case 'C': case 'c':
      gate = CX;
      DEBUG("Got CNOT gate ");
    default:
      die(0, "An illegal gate got past the regex, somehow (line %u): %s", line, buf);
  }

  noperands = parse_operands(buf, operands, sizeof(operands) / sizeof(*operands));
  if(noperands > regp->width)
    die(0, "Line %u: Too many operands", line);
  if(!noperands)
    die(0, "Line %u: Expected at least one operand", line);
  if(gate == CX && noperands != 2)
    die(0, "Line %u: CNOT gate requires exactly 2 operands", line);
  DEBUG("with %u operands: %u, ...\n", noperands, operands[0]);

  compound_gate = bit_in_ops(regp->width - 1, operands, noperands) ? gate : I2;
  for(i = (int)regp->width - 2; i >= 0; --i)
    if(bit_in_ops((unsigned)i, operands, noperands))
      compound_gate = m_tensor(compound_gate, gate);
    else
      compound_gate = m_tensor(compound_gate, I2);

  qr_evolve(regp, compound_gate);
    if(opts.singlestep) {
      printf("|psi_%u> = ", line - 1);
      qr_print(regp);
      getchar();
    }

  m_stack_pop();
}

void processline(char *buf, regex_t reg_regex, regex_t gate_regex) {
  static qreg reg = {0};
  static bool reginit = false;
  static unsigned line = 0;
  unsigned i, regwidth;

  ++line;

  if(!regexec(&reg_regex, buf, 0, NULL, 0)) {
    for(i = 0; *buf && !isdigit(buf[i]); ++i);
    regwidth = strtou_or_die(&buf[i], NULL, 0);
    qr_init(&reg, regwidth, opts.input);
    reginit = true;
    DEBUG("Initialized register with width=%u and value=%x\n", regwidth, opts.input);

    if(opts.singlestep) {
      printf("|psi_%u> = ", line - 1);
      qr_print(&reg);
      getchar();
    }
    return;
  }

  if(!regexec(&gate_regex, buf, 0, NULL, 0)) {
    if(!reginit)
      die(0, "You must define a register width (register <width>) before any operations");

    do_gate(&reg, buf, line);
  } else
    die(0, "Line %u does not appear to be a valid operation:\n%s", line, buf);
}

void processfile(const char *filename) {
  FILE *fp = fopen_or_die(filename, "r");
  char *buf = NULL, *nl;
  size_t len = 0;
  static regex_t reg_regex, gate_regex;
  int err;
  char errbuf[1024];
  
  err = regcomp(&reg_regex, REG_LINE_REGEX, REG_EXTENDED | REG_NOSUB);
  if(err) {
    regerror(err, &reg_regex, errbuf, sizeof(errbuf));
    die(0, "regcomp(%s): %s", REG_LINE_REGEX, errbuf);
  }

  err = regcomp(&gate_regex, GATE_LINE_REGEX, REG_EXTENDED | REG_NOSUB);
  if(err) {
    regerror(err, &reg_regex, errbuf, sizeof(errbuf));
    die(0, "regcomp(%s): %s", GATE_LINE_REGEX, errbuf);
  }

  while(getline(&buf, &len, fp) != -1) {
    if((nl = strchr(buf, '\n')))
      *nl = '\0';
    processline(buf, reg_regex, gate_regex);
  }

  regfree(&reg_regex);
  regfree(&gate_regex);
  free(buf);
  fclose(fp);
}

int main(int argc, char **argv) {
  m_stack_push();
  init_global_qops();

  parseargs(argc, argv);

  processfile(opts.filename);

  return 0;

  m_stack_pop();
}

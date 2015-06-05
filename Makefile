CC	= gcc
CFLAGS	= -g -Wall -std=gnu99
LDFLAGS	= -lm

ifdef DEBUG
CFLAGS	+= -DPRINTF_DEBUG
endif

NAME	= $(shell cat .vimtemplates/projectname)
OBJS	= qsim.o qregister.o matrix.o common.o
TESTS	= test_matrix test_qregister

SRC	= src
BIN	= bin
TESTDIR	= tests
OBJS	:= $(patsubst %,${SRC}/%,${OBJS})
TESTS	:= $(patsubst %,${BIN}/%,${TESTS})

all: ${OBJS} ${NAME}

${NAME}: ${OBJS}
	@echo LD $@
	@${CC} ${CFLAGS} -o $@ $^ ${LDFLAGS}

%.o: %.c %.h
	@echo CC $@
	@${CC} ${CFLAGS} -c $< -o $@ ${LDFLAGS}

tests: ${BIN} ${TESTS}

test: tests
	@for i in ${TESTS}; do echo "RUN TEST $$i"; $$i; done

${BIN}/test_matrix: ${TESTDIR}/test_matrix.c ${SRC}/matrix.o ${SRC}/common.o
	@echo CC $@
	@${CC} ${CFLAGS} -o $@ $^ ${LDFLAGS}

${BIN}/test_qregister: ${TESTDIR}/test_qregister.c ${SRC}/qregister.o \
                       ${SRC}/common.o ${SRC}/matrix.o
	@echo CC $@
	@${CC} ${CFLAGS} -o $@ $^ ${LDFLAGS}

${BIN}:
	mkdir ${BIN}

clean:
	rm -f ${OBJS} ${TESTS} ${NAME}

.PHONY: clean test tests all debug

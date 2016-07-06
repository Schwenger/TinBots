# ==== Configuration -- change this! =====

# All automated currently


# ==== Configuration -- I don't see a reason why this would ever change =====

TESTS:=$(wildcard src/*.c)

OBJDIR:=obj
BINDIR:=bin

INCLUDE_FLAGS:=-I .. -I . -I ../src -I ../hal
WARN_FLAGS:=-std=c90 -ansi -Weverything -pedantic
CFLAGS?=-g -c -O2

TBSRCS:=$(wildcard ../src/*.c) $(wildcard ../src/astar/*.c)
TBHEADERS:=$(wildcard ../src/*.h) $(wildcard ../src/astar/*.h) $(wildcard ../*.h) ../hal/hal.h
TBOBJS:=$(patsubst ../src/%.c,${OBJDIR}/%_tb.o,${TBSRCS})
TESTOBJS:=$(patsubst src/%.c,${OBJDIR}/%_test.o,${TESTS})
TESTBINS:=$(patsubst src/%.c,${BINDIR}/%,${TESTS})
RUNTARGETS:=$(patsubst src/%.c,run-%,${TESTS})
# stub.c and stub.o will be built explicitely.


# ===== Main entry point and abstract commands =====

all: run-all

.PHONY: build
build: ${TESTBINS}

.PHONY: clean
clean:
	rm -rf ${OBJDIR} ${BINDIR}

.PHONY: dirs
dirs:
	@mkdir -p ${OBJDIR} ${BINDIR} ${OBJDIR}/astar

.PHONY: run-all
run-all: ${RUNTARGETS} | build

# Debugging the makefile
.PHONY: say
say:
	true "TBSRCS = ${TBSRCS}"
	true "TBOBJS = ${TBOBJS}"
	true "TBHEADERS = ${TBHEADERS}"
	true "RUNTARGETS = ${RUNTARGETS}"
	true "TESTOBJS = ${TESTOBJS}"
	true "TESTBINS = ${TESTBINS}"


# ===== Synthesized test targets =====

.PHONY: ${RUNTARGETS}
${RUNTARGETS}: run-%: ${BINDIR}/%
# This is magic:
	$^


# ===== Plumbing =====

# Object files

${TBOBJS}: ${OBJDIR}/%_tb.o: ../src/%.c | ${TBHEADERS} dirs
	clang ${INCLUDE_FLAGS} ${WARN_FLAGS} ${CFLAGS} -o $@ $<

${OBJDIR}/stub.o: stub.c | ${TBHEADERS} dirs
	clang ${INCLUDE_FLAGS} ${WARN_FLAGS} ${CFLAGS} -o $@ $<

${TESTOBJS}: ${OBJDIR}/%_test.o: src/%.c | ${TBHEADERS} dirs
	clang ${INCLUDE_FLAGS} ${WARN_FLAGS} ${CFLAGS} -o $@ $<

# Binaries

${TESTBINS}: ${BINDIR}/%: ${OBJDIR}/%_test.o ${OBJDIR}/stub.o ${TBOBJS} | dirs
	clang ${LFLAGS} -o $@ $<

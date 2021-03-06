# ==== Configuration -- change this! =====

# All automated currently


# ==== Configuration -- I don't see a reason why this would ever change =====

TESTS:=$(wildcard src/*.c)

OBJDIR:=obj
BINDIR:=bin

INCLUDE_FLAGS:=-I .. -I . -I ../src -I ../hal
WARN_FLAGS:=-std=c90 -ansi -Weverything -pedantic
WARN_FLAGS+=-Wno-padded -Wno-covered-switch-default -Wno-missing-noreturn
WARN_FLAGS+=-Wno-pointer-arith -Wno-gnu-binary-literal -Wno-cast-align
CFLAGS?=-g -c -O2 -DMAP_MAX_WIDTH=20 -DMAP_MAX_HEIGHT=20 -DCONFIG_MAP_ASSERT_ANYWAY
LFLAGS?=-Weverything -lm
# For libubsan, add "-fsanitize=undefined" to CFLAGS, and "-lubsan" to LFLAGS

TBSRCS:=$(wildcard ../src/*.c) $(wildcard ../src/astar/*.c)
TBHEADERS:=$(wildcard ../src/*.h) $(wildcard ../src/astar/*.h) $(wildcard ../*.h) ../hal/hal.h mock.h ../hal/map_heap.h
TBOBJS:=$(patsubst ../src/%.c,${OBJDIR}/%_tb.o,${TBSRCS})
TESTOBJS:=$(patsubst src/%.c,${OBJDIR}/%_test.o,${TESTS})
TESTBINS:=$(patsubst src/%.c,${BINDIR}/%,${TESTS})
RUNTARGETS:=$(patsubst src/%.c,run-%,${TESTS})
VALGRINDTARGETS:=$(patsubst src/%.c,valgrind-%,${TESTS})
# mock.o and map_heap.o will be built "by hand".


# ===== Main entry point and abstract commands =====

all: run-all

.PHONY: build
build: ${TESTBINS} ${OBJDIR}/matlab.o

.PHONY: clean
clean:
	rm -rf ${OBJDIR} ${BINDIR}

.PHONY: dirs
dirs:
	@mkdir -p ${OBJDIR} ${BINDIR} ${OBJDIR}/astar

.PHONY: run-all
run-all: ${RUNTARGETS} ${OBJDIR}/matlab.o

.PHONY: valgrind-all
valgrind-all: ${VALGRINDTARGETS} ${OBJDIR}/matlab.o


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
	@echo "[TEST] Running $@ test ..."
# This is magic:
	@$<

.PHONY: ${VALGRINDTARGETS}
${VALGRINDTARGETS}: valgrind-%: ${BINDIR}/%
	valgrind --leak-check=full $<


# ===== Plumbing =====

# Object files

${TBOBJS}: ${OBJDIR}/%_tb.o: ../src/%.c ${TBHEADERS} | dirs
	@echo "[ CC ] -o $@ $<"
	@clang ${INCLUDE_FLAGS} ${WARN_FLAGS} ${CFLAGS} -o $@ $<

${OBJDIR}/mock.o: mock.c ${TBHEADERS} | dirs
	@echo "[ CC ] -o $@ $<"
	@clang ${INCLUDE_FLAGS} ${WARN_FLAGS} ${CFLAGS} -o $@ $<

${OBJDIR}/map_heap.o: ../hal/map_heap.c ${TBHEADERS} | dirs
	@echo "[ CC ] -o $@ $<"
	@clang ${INCLUDE_FLAGS} ${WARN_FLAGS} ${CFLAGS} -o $@ $<

${OBJDIR}/matlab.o: ../hal/hal_matlab.c ../matlab.h ${TBHEADERS} | dirs
	@echo "[ CC ] -o $@ $<"
	@clang ${INCLUDE_FLAGS} ${WARN_FLAGS} ${CFLAGS} -o $@ $<

${TESTOBJS}: ${OBJDIR}/%_test.o: src/%.c ${TBHEADERS} | dirs
	@echo "[ CC ] -o $@ $<"
	@clang ${INCLUDE_FLAGS} ${WARN_FLAGS} ${CFLAGS} -o $@ $<

# Binaries

${TESTBINS}: ${BINDIR}/%: ${OBJDIR}/%_test.o ${OBJDIR}/mock.o ${OBJDIR}/map_heap.o ${TBOBJS} | dirs
	@echo "[LINK] -o $@"
	@clang ${LFLAGS} -o $@ $^

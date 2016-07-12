CFLAGS:=-I .. -I ../src -I ../hal
CFLAGS+=-Wall -Wextra -pedantic

XC16CALL:=/opt/microchip/xc16/v1.26/bin/xc16-gcc ${CFLAGS} -mcpu=30F6014A -mlarge-code -mlarge-data -O0
GCC0CALL:=gcc ${CFLAGS}
GCC3CALL:=gcc ${CFLAGS} -march=native -O3

GENFILES:=

all:

GENFILES+=map-orig-xc16.S
map-orig-xc16.S: ../src/map_merge.c
	${XC16CALL} -o $@ -S $<

GENFILES+=map-orig-xc16.o
map-orig-xc16.o: map-orig-xc16.S
	${XC16CALL} -o $@ -c $<

GENFILES+=map-direct-xc16.o
map-direct-xc16.o: ../src/map_merge.c
	${XC16CALL} -o $@ -c $<

GENFILES+=map-orig-gcc0.S
map-orig-gcc0.S: ../src/map_merge.c
	${GCC0CALL} -o $@ -S $<

GENFILES+=map-orig-gcc0.o
map-orig-gcc0.o: map-orig-gcc0.S
	${GCC0CALL} -o $@ -c $<

GENFILES+=map-direct-gcc0.o
map-direct-gcc0.o: ../src/map_merge.c
	${GCC0CALL} -o $@ -c $<

GENFILES+=bench-gcc0.o
bench-gcc0.o: bench.c
	${GCC0CALL} -o $@ -c $<

GENFILES+=bench-gcc0
bench-gcc0: bench-gcc0.o map-direct-gcc0.o ../src/map_common.c ../hal/map_static.c
	${GCC0CALL} -o $@ $^

GENFILES+=map-orig-gcc3.S
map-orig-gcc3.S: ../src/map_merge.c
	${GCC3CALL} -o $@ -S $<

GENFILES+=map-orig-gcc3.o
map-orig-gcc3.o: map-orig-gcc3.S
	${GCC3CALL} -o $@ -c $<

GENFILES+=map-direct-gcc3.o
map-direct-gcc3.o: ../src/map_merge.c
	${GCC3CALL} -o $@ -c $<

GENFILES+=bench-gcc3.o
bench-gcc3.o: bench.c
	${GCC3CALL} -o $@ -c $<

GENFILES+=bench-gcc3
bench-gcc3: bench-gcc3.o map-direct-gcc3.o ../src/map_common.c ../hal/map_static.c
	${GCC3CALL} -o $@ $^

.PHONY: run-gcc
run-gcc: bench-gcc0
	./bench-gcc0 & (sleep 5 && killall bench-gcc0)

.PHONY: clean
clean:
	rm -f ${GENFILES}

all: ${GENFILES}

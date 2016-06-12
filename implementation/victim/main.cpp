#define F_CPU 8000000UL

#include "neatavr.hpp"

using namespace NeatAVR;

typedef Pin9 IREmitter;

int main() {
    IREmitter::output();

    return 0;
}
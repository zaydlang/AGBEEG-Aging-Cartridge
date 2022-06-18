#include "util.h"

void waitloop(int loops) {
    for (volatile int i = 0; i < loops; i++) {}
}
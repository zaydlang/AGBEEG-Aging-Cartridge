#include "ci.h"

void ci_send(void* data, int len) {
    for (int i = 0; i < len; i++) {
        REG_TEST_RESULTS = ((byte*) data)[i];
    }

    REG_TEST_RESULTS = 0;
}
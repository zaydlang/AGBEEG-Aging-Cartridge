#ifndef CI_H
#define CI_H

#include <tonc.h>

#define REG_TEST_RESULTS *((vu32*) 0x04999990)

void ci_send(void* data, int len);

#endif
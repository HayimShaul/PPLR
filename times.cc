#include "times.h"

TakeTimes Times::phase1_step1;
TakeTimes Times::phase1_step2;
TakeTimes Times::phase1_step3;

TakeTimes Times::phase2_step1;
TakeTimes Times::phase2_step2;
TakeTimes Times::phase2_step3;

pthread_mutex_t Times::mutex = PTHREAD_MUTEX_INITIALIZER;

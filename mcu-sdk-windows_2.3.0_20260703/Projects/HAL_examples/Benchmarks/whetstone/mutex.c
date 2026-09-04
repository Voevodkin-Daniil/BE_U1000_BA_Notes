#include <stdint.h>
#include <stdio.h>

#include "bmcu_common.h"
#include "mutex.h"

void mutex_lock(mutex_t* mtx) {
    while(mtx->locked == 1);
    mtx->locked = 1;
    __delay_ms(1);
}

void mutex_unlock(mutex_t* mtx) {
    mtx->locked = 0;
    __delay_ms(1);
}

void mutex_init(mutex_t* mtx) {
    mtx->locked = 0;
}

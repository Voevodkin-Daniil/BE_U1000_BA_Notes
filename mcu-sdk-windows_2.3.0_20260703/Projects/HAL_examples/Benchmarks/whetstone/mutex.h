#ifndef _MUTEX_H
#define _MUTEX_H

#include <stdint.h>

typedef struct {
    volatile uint32_t locked;
} mutex_t;

void mutex_lock(mutex_t* mtx);
void mutex_unlock(mutex_t* mtx);
void mutex_init(mutex_t* mtx);

#endif

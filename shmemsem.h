#ifndef SHMEMSEM_H
#include <semaphore.h>

typedef struct {
	int seconds;
	int nanoseconds;
	int sharedmsg[3];
	sem_t mutexclock;
	int pidcounter;
} SharedData;

#define SHMEMSEM_H
#endif

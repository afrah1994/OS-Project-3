/*
 * child.c
 *
 *  Created on: Oct 1, 2019
 *      Author: afrah
 */
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <stdlib.h>
# include <dirent.h>
# include <stdio.h>
# include <string.h>
# include <getopt.h>
# include<stdbool.h>
# include <ctype.h>
# include<sys/wait.h>
# include<signal.h>
# include <sys/mman.h>
# include<sys/time.h>
# include <stdint.h>
#include <fcntl.h>
#include <sys/shm.h>
#include "shmemsem.h"
#include <sys/ipc.h>
#include<semaphore.h>
#include <stdbool.h>
#include <pthread.h>
SharedData *shmem;
int shmid;

void main()
{

		int times=0,timen=0,zero=0,randomnum,timelimit,n=0,sum,i;


		// Create our key
		key_t shmkey = ftok("makefile",777);
		if (shmkey == -1) {
			perror("user:Ftok failed");
			exit(-1);
		}

		// Get our shm id
		shmid = shmget(shmkey, sizeof(SharedData), 0600 | IPC_CREAT);
		if (shmid == -1) {
			perror("user:Failed to allocate shared memory region");
			exit(-1);
		}

		// Attach to our shared memory
		shmem = (SharedData*)shmat(shmid, (void *)0, 0);
		if (shmem == (void *)-1) {
			perror("user:Failed to attach to shared memory region");
			exit(-1);
		}
		shmem->pidcounter++;

		// printf("\ninitial values of shared memory from user process seconds=%d nanoseconds=%d sharedmsg=%d",shmem->seconds,shmem->nanoseconds,shmem->sharedmsg[2]);

		randomnum = (rand() % 1000000);
		timelimit=shmem->nanoseconds+randomnum;

		if(timelimit>1000000000)
		{
			times=shmem->seconds+1;
			timen=timelimit-1000000000;
		}
		else
		{
			times=shmem->seconds;
			timen=timelimit;
		}

		//printf("\nTimelimit for process %d after adding random number: %d %d",getpid(),times,timen);
		bool Iamalive=true;
		do
		{
			sem_wait(&(shmem->mutexclock));
			//printf("\nchild %d entered critical section ",getpid());
			if((shmem->seconds >= times) && (shmem->nanoseconds>=timen) && ( shmem->sharedmsg[0]==0))
			{
				//printf("\nFROM CHILD: process %d finished at simulated seconds=%d nano=%d",getpid(),shmem->seconds,shmem->nanoseconds);
				shmem->sharedmsg[0]=getpid();
				shmem->sharedmsg[1]=times;
				shmem->sharedmsg[2]=timen;
				//printf("\nFROM CHILD: process %d finished at simulated seconds=%d nano=%d",getpid(),shmem->seconds,shmem->nanoseconds);
				Iamalive=false;

				sem_post(&(shmem->mutexclock));

				shmdt(shmem);
				//adding delay
				sum = 0;
				for ( i = 0; i < 100000; i++)
					sum += i;

				exit(0);
			}//endif

			sem_post(&(shmem->mutexclock));
			//adding delay
			sum = 0;
			for ( i = 0; i < 100000; i++)
				sum += i;
			//	printf ( "\nChild %d left the CS", getpid() );

    }while(Iamalive);
}



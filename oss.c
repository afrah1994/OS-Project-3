/*
 ============================================================================
 Name        : simulator.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
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
# include <stdbool.h>
# include <ctype.h>
# include <sys/wait.h>
# include <signal.h>
# include <sys/mman.h>
# include <sys/time.h>
# include <stdint.h>
# include <fcntl.h>
# include <sys/shm.h>
# include <semaphore.h>
# include "shmemsem.h"
#include <stdbool.h>
//global variables
int processnum=0;
pid_t alive[110];
SharedData *shmem;
int shmid;
int x;//number of children forked by program -s
char *logname;//log file name for program -l
bool timeup=false;
void forking();
void handler(int sig);
void cleanup();
void catch_alarm(int sig);
void spawnchild();
void handler(int sig)//SIGTERM handler
{
	sleep(10);//Handler makes the program sleep till it finishes graceful termination
}
void cleanup()
{
	if(timeup==true)
		printf("\ntime is up due to -t");
	printf("\nDoing cleanup");
	shmdt(shmem);// Free up shared memory
	shmctl(shmid, IPC_RMID,NULL);
	exit(0);
}
void catch_alarm (int sig)//SIGALRM handler
{
	timeup=true;
	//sleep(1);
	cleanup();
}
void forking()
{
	int i,sum;
	pid_t pID = fork();

	if (pID < 0)
	{

		   perror("oss: Failed to fork:");
		   exit(EXIT_FAILURE);
	}

	else if (pID == 0)
	{

		alive[processnum]=getpid();
		processnum++;
		static char *args[]={"./user",NULL};
		int status;
		if(( status= (execv(args[0], args)))==-1)
		{
			perror("oss:failed to execv");
			exit(EXIT_FAILURE);
		}
	}

	if(pID>0)
	{
		while(1)
		{
			sem_wait(&(shmem->mutexclock));
			//printf("\nmaster %d in critical section",getpid());
			if(shmem->seconds<2)
			{
				shmem->nanoseconds=shmem->nanoseconds+100000;
				if(shmem->nanoseconds>=1000000000)
				{
					shmem->seconds=shmem->seconds+1;
					shmem->nanoseconds=0;
				}
			}
			if(shmem->seconds==2)
			{
				printf(" \nsimulated clock reached limit of 2 seconds ");
				cleanup();
			}
			if(shmem->sharedmsg[0]!=0)
			{
				printf("\n process %d is exiting at time %d %d",shmem->sharedmsg[0], shmem->sharedmsg[1], shmem->sharedmsg[2]);
				FILE *fp;
				fp = fopen(logname, "a+");
	                if (fp == NULL)
	                {
	                	perror("oss: Unable to open the output file:");
	                }
	                else
	                {
	                	//printf("\nentering data in file");
	                	fprintf(fp,"Master: Child %d is terminating at my time %d.%d because it reached %d.%d in child process\n",shmem->sharedmsg[0],shmem->seconds,shmem->nanoseconds,shmem->sharedmsg[1],shmem->sharedmsg[2]);
	                }
	                fclose(fp);

	              //  int kill_value=kill(shmem->sharedmsg[0],SIGKILL);
	                //printf("\n kill_value= %d",kill_value);
	                int pidforwait=shmem->sharedmsg[0];
	                int corpse,status;
		            while ((corpse = waitpid(pidforwait, &status, 0)) != pidforwait && corpse != -1)
		    		   {

		    			char pmsg[64];
		    			snprintf(pmsg, sizeof(pmsg), "logParse: PID %d exited with status 0x%.4X", corpse, status);
		    			perror(pmsg);
		    		   }

	                shmem->sharedmsg[0]=0;
	                shmem->sharedmsg[1]=0;
	                shmem->sharedmsg[2]=0;

	               // printf("\npidcounter %d",shmem->pidcounter);
	                if(shmem->pidcounter<100)
	                {

	                	printf("\n forking new children");
	                	sem_post(&(shmem->mutexclock));
	                	//delay
	                	//printf("\nstill <100");
	                	sum = 0;
	                	for (i = 0; i < 100000; i++)
	                    sum += i;
	                	forking();
	                }
	                else
	                {
	                		sem_post(&(shmem->mutexclock));
	                		//printf("left cs in else");
	                		//delay
	                		sum=0;
	                		for (i = 0; i < 100000; i++)
	                		sum += i;
	                		printf("\n100 user processes already forked");
	                		cleanup();
	                }
	             }//endif
   		sem_post(&(shmem->mutexclock));
   		//delay
   		//printf("\nleft cs outside");
   		sum=0;
   	    for (i = 0; i < 100000; i++)
   	    sum += i;

 		   }//endwhile
   	}//endif(PID>0)
}

void spawnchild()
{
		int i;

		// Create our key
		key_t shmkey = ftok("makefile",777);
		if (shmkey == -1) {
			perror("oss:Ftok failed");
			exit(-1);
		}

		// Get our shm id
		shmid = shmget(shmkey, sizeof(SharedData), 0666 | IPC_CREAT);
		if (shmid == -1) {
			perror("oss:Failed to allocate shared memory region");
			exit(-1);
		}

		// Attach to our shared memory
		shmem = (SharedData*)shmat(shmid, (void *)0, 0);
		if (shmem == (void *)-1) {
			perror("oss:Failed to attach to shared memory region");
			exit(-1);
		}
		for (i=0;i<100;i++)
		{
			alive[i]=0;
		}
		shmem->pidcounter=0;
		sem_init(&(shmem->mutexclock),1,1);
		shmem->nanoseconds=0;
		shmem->seconds=0;
		shmem->sharedmsg[0]=0;//stores current pid
		shmem->sharedmsg[1]=0;//stores finish time for process seconds
		shmem->sharedmsg[2]=0;//stored finish time for process nanoseconds

		for(i=0;i<x;i++)
		{
			forking();
		}
}
int main(int argc, char **argv)
{
	bool flagmain=true;
    int options;
    bool flagh=false,flags=false,flagl=false,flagt=false;

    logname="log.dat";
    int z=5;
    x=5;

    while((options=getopt(argc,argv,"-:hs::l::t::"))!=-1)
    	    	{
    	    		switch(options)
    	    		{
    	    		case 'h':			flagh=true;
    	    			    			printf("These are the following example commands that the project can do:");
    	    			    			printf("\n-h\n-s , -s9\n-l , -llog.dat\n-t, -t20");
    	    			    			printf("\nIf we do want to specify maximum child processes spawned (-s), log file name (-l) or time limit (in seconds -t) "
    	    			    					"there can be no space between the the command (-s/-l/-t) and their values,"
    	    			    					"as shown in the example above. If no values are passed along with -s/-l/-t "
    	    			    					"the default values 5(child processes), log.dat, 5(seconds) will be used\n The program works without any options (./oss)"
    	    			    					"A few problems were encountered during the project please see README for more details.");
    	    			    			break;
    	    			    			exit(EXIT_SUCCESS);
    	    		case 's':
    	    			    			if(optarg)
    	    			    			{
    	    			    				x=atoi(optarg);
    	    			    			}
    	    			    			break;
    	    		case 'l':
    	    						if(optarg)
    	    						{
    	    							logname=optarg;
    	    						}
    	    						break;
    	    		case 't':
    	    				if(optarg)
    	    				{
    	    					z=atoi(optarg);//stores time our program should run for default is 10
    	    				}
    	    				break;
    	    		}
    	    	}
    signal (SIGALRM, catch_alarm);
    alarm (z);
    if(flagh==false)
	{
    	spawnchild();
	}
	sleep(2);

    return 0;
}

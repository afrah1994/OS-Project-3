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
# include<stdbool.h>
# include <ctype.h>
# include<sys/wait.h>
# include<signal.h>
# include <sys/mman.h>
# include<sys/time.h>
#include <stdint.h>
#define BILLION 1000000000L
static struct timespec *start;
static char *shared;
struct timespec end;
void handler(int sig)//SIGTERM handler
{
	printf("\ntimed out");
	sleep(10);//Handler makes the program sleep till it finishes graceful termination
}
void catch_alarm (int sig)//SIGALRM handler
{
	int i;
	//for(i=0;i<(length+1);i++)
	{
		//if(PID[i]!=0)//if =0 then the child process has been already killed in forkingfunction()

		{   //signal(SIGTERM,handler);
			//kill (PID[i],SIGTERM);
		}


	}
}
void spawnchild(int x, char *logname)
{
	 start = mmap(NULL, sizeof *start, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	 int memFd = shm_open("memory", O_CREAT | O_RDWR, S_IRWXU);
	 if (memFd == -1)
	 {
	     perror("Can't open file");
	    // return 1;
	 }

	 int res = ftruncate(memFd, sizeof *shared);
	 if (res == -1)
	 {
	     perror("Can't truncate file");
	     //return res;
	 }
	 shared = mmap(NULL, sizeof *shared, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
	 if (shared == NULL)
	 {
	     perror("Can't mmap");
	     //return -1;
	 }
	 //shared = mmap(NULL, sizeof *shared, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	 clock_gettime(CLOCK_MONOTONIC, start);
	 uint64_t diff;

	int i;
	for(i=0;i<x;i++)
		    {

		    	 pid_t pID = fork();

		    	   if (pID == 0)                // child
		    	   {
                       shared="Hello from child";
                       printf("shared=%s",shared);
                       int rannum = (rand() %
                                 (1000000 - 0 + 1)) + 0;
                       clock_gettime(CLOCK_MONOTONIC, &end);	/* mark the end time */
                       diff= BILLION * (end.tv_sec - (*start).tv_sec) + end.tv_nsec - (*start).tv_nsec;
		    				printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);
		    				long timelimit=diff+rannum;
		    				char timelimit1[1000];
		    				sprintf(timelimit1, "%ld", timelimit);
		    				 static char *args[]={"./child",NULL};

		    				 execv(args[0], args);
		    				             fprintf(stderr, "Failed to execute %s\n", args[0]);
		    				             printf("failed to execv");
		    				             exit(EXIT_FAILURE);


		    		      exit(EXIT_SUCCESS);
		    		       kill(pID,SIGKILL);

		    	   }
		    	   else if (pID < 0)            // failed to fork
		    	   {
		    		   perror("Assignment 3: Failed to fork:");
		    		   exit(EXIT_FAILURE);
		    	   }
		    	   else if(pID>0)
		    	   {
		    		    int corpse,status;
		    		    if ((shared != NULL) && (shared == '\0')) {
		    		       printf("shared is not empty\n");

			            while ((corpse = waitpid(pID, &status, 0)) != pID && corpse != -1)
			    		   {

			    			char pmsg[64];
			    			clock_gettime(CLOCK_MONOTONIC, &end);	/* mark the end time */


			    			diff= BILLION * (end.tv_sec - (*start).tv_sec) + end.tv_nsec - (*start).tv_nsec;
			    				printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);
			    			printf(pmsg, sizeof(pmsg), "Assignment 3: PID %d exited with status 0x%.4X and time", corpse, status);
			    			perror(pmsg);
			    		   }
			            pID=fork();
			           // shared*="";
		    		    }
		    	   }
		    }//end of for
	//	printf("\nOutput file updated.");

}
int main(int argc, char **argv)
{
    int options;
    bool flagh=false,flags=false,flagl=false,flagt=false;
    char *logname;
    logname="log.dat";
    int x=5,z=5;

    while((options=getopt(argc,argv,"-:hs::l::t::"))!=-1)
    	    	{
    	    		switch(options)
    	    		{
    	    		case 'h':
    	    				flagh=true;
    	    			    			printf("These are the following commands that the project can do:");
    	    			    			printf("\n-h\n-i , -iinputfilename\n-o , -ooutputfilename\n-t, -t20");
    	    			    			printf("\nIf we do want to specify an input/output file name or time limit (in seconds) "
    	    			    					"there can be no space between the the command (-i/-o/-t) and the file name/time value "
    	    			    					"as shown in the example above If no values are passed along with -i/-o/-t "
    	    			    					"the default values input.dat, output.dat, 10(seconds) will be used\n"
    	    			    					"The program will notify once the output file has been updated with a message or if any error arises.");

    	    			    			flagh=true;
    	    			    			break;
    	    			    			exit(EXIT_SUCCESS);
    	    		case 's':

    	    			    			flags=true;
    	    			    			if(optarg)
    	    			    			{
    	    			    				x=atoi(optarg);
    	    			    			}
    	    			    			break;
    	    		case 'l':
    	    						flagl=true;
    	    						if(optarg)
    	    						{
    	    							logname=optarg;
    	    						}
    	    						break;
    	    		case 't':
    	    				flagt=true;
    	    				if(optarg)
    	    				{
    	    					z=atoi(optarg);//stores time our program should run for default is 10
    	    				}
    	    				break;
    	    		}
    	    	}
    signal (SIGALRM, catch_alarm);
        alarm (z);
    if(flags||flagl||flagt)
    {
    	spawnchild(x,logname);
    }
    sleep(2);


    return 0;
}

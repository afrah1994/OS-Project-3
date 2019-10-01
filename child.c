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

int main(int argc, char **argv)
{
    /*if (argc != 3)
    {
        fprintf(stderr, "Usage: %s number1 number2\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    long number1 = strtol(argv[1], 0, 0);
   // long number2 = strtol(argv[2], 0, 0);
    printf("%ld\n", number1);*/
    int memFd = shm_open("memory", O_RDONLY, 0);
    if (memFd == -1)
    {
        perror("Can't open file");
        return 1;
    }

    char *buffer = mmap(NULL, sizeof *buffer, PROT_READ, MAP_SHARED, memFd, 0);
    if (buffer == NULL)
    {
        perror("Can't mmap");
        return -1;
    }
    printf("hello from exec");
    printf("%s",buffer);

    return 0;
}


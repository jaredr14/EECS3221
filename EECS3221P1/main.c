#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include "fork.h"

int was_alarm=0;

/* The main program does much of the work. parses the command line arguments */
/* sets up the alarm and the alarm signal handler opens the files and pipes */
/* for redirection etc., invoke start_child, closes files that should be  */
/* closed, waits for the children to finish and reports their status */
/* or exits printing a message and kill its children if they do not die */
/* in time (just the parent exiting may not be enough to kill the children) */
int main(int argc, char *argv[])
{
  //printf("This is an empty main\n");
  if(argv < 3)
  {
    f_error("not enough args");
  }

  int i;
  int p1 = -1;
  char p[] = "-p-";
  for(i = 0; i < argc; i++)
  {
    if(strcmp(p, argv[i]) == 0)
    {
      p1 = i;
      argv[p1] = NULL;
    }
  }

  if(argv[p1] != NULL)
  {
    f_error("No pipe found");
  }

  int fd[2];
  pipe(fd);

  if (pipe(fd)<0)
    {
      fprintf(stderr, "%s: Error creating pipe: %s\n",argv[0], strerror(errno));
      exit(1);
    }

  int fdin = open("test.in", O_RDONLY);
  if (fdin<0)
    {
      fprintf(stderr, "%s: Error opening test.in: %s\n",argv[0], strerror(errno));
      exit(1);
    }
  int fdout = open("test.out", O_RDWR | O_TRUNC, 0777);
  if (fdout<0)
    {
      fprintf(stderr, "%s: Error opening test.out: %s\n",argv[0], strerror(errno));
      exit(1);
    }
  int fderr1 = open("test.err1", O_RDWR | O_CREAT | O_TRUNC, 0777);
  if (fderr1<0)
    {
      fprintf(stderr, "%s: Error creating test.err1: %s\n",argv[0], strerror(errno));
      exit(1);
    }
  int fderr2 = open("test.err2", O_RDWR | O_CREAT | O_TRUNC, 0777);
  if (fderr2<0)
    {
      fprintf(stderr, "%s: Error creating test.err2: %s\n",argv[0], strerror(errno));
      exit(1);
    }

  start_child(argv[1], argv+1, fdin, fd[1], fderr1);
  start_child(argv[p1+1], argv+p1+1, fdout, fd[0], fderr2);

  wait(NULL);
  close(fd[1]);
  wait(NULL);
}

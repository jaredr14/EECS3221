#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "fork.h"

/* This is the handler of the alarm signal. It just updates was_alarm */
void alrm_handler(int i)
{
	was_alarm==1;
}

/* Prints string s using perror and exits. Also checks errno to make */
/* sure that it is not zero (if it is just prints s followed by newline */
/* using fprintf to the standard error */
void f_error(char *s)
{
	if(errno == 0)
	{
		fprintf(stderr, "%s\n", s);
	}
	perror(s);
	exit(1);

}

/* Creates a child process using fork and a function from the exec family */
/* The standard input output and error are replaced by the last three */
/* arguments to implement redirection and piping */
pid_t start_child(const char *path, char *const argv[],
		  int fdin, int fdout, int fderr)
{
	pid_t pid = fork();

	if(fdin != 0)
	{
		dup2(fdin, 0);
		if (dup2(fdin, 0)<0)
	    {
	      fprintf(stderr, "%s: dup2 failed: %s\n",argv[0], strerror(errno));
	      exit(1);
	    }
	}
	if(fdout != 1)
	{
		dup2(fdout, 1);
		if (dup2(fdout, 1)<0)
	    {
	      fprintf(stderr, "%s: dup2 failed: %s\n",argv[0], strerror(errno));
	      exit(1);
	    }
	}
	if(fdin != 2)
	{
		dup2(fderr, 2);
		if (dup2(fderr, 2)<0)
	    {
	      fprintf(stderr, "%s: dup2 failed: %s\n",argv[0], strerror(errno));
	      exit(1);
	    }
	}

	int i = 0;
	for(i = 3; i < 9; i++)
	{
		close(i);
	}

	execvp(path, argv);

	return pid;
}

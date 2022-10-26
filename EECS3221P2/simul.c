/***********************************************************************/
/**      Author: Jared Rego                                           **/
/**        Date: Jun. 2019                                            **/
/** Description: Skeleton code for Assgn. II                          **/
/***********************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "args.h"
#include "error.h"

int N=5, T=100;
double lam=0.1, mu=0.2;
int nblocked, nticks, nserver, nclient;			/* The number of threads blocked */
int ttl_ttime, ttl_htime, ttl_etime; //total think, hungry, and eat time

/***********************************************************************
                         P H I L O S O P H E R
************************************************************************/
void *philosopher(void *vptr)
{
  // states: thinking -> 0, hungry -> 1, eating -> 2
  unsigned int seed;		/* This is called from main without    */
  int pthrerr;			/* creating a new thread               */
  struct thread_arg *ptr;

  ptr = (struct thread_arg*)vptr;

  while (1) {
    pthrerr = pthread_mutex_lock(ptr->blockutex);
    if (pthrerr != 0)
    {
      fatalerr("philosopher", pthrerr, "Failed, Mutex lock...\n");
    }
    nblocked++;
    if (nblocked == N){
      pthrerr = pthread_cond_signal(ptr->condclk);
      if (pthrerr != 0)
      {
        fatalerr("clk", pthrerr, "Failed, Clock broadcast...\n");
      }
    }
    pthrerr = pthread_cond_wait(ptr->start_line, ptr->blockutex);
    if (pthrerr != 0)
    {
      fatalerr("philosopher", pthrerr, "Failed, phil_cond wait...\n");
    }
    if (ptr->state == 2){
      if (rand0_1(&(ptr->seed)) < ptr->mu){
        ptr->state = 0;
      }
    } else { // philosopher is in state 0
      if (rand0_1(&(ptr->seed)) < ptr->lam){
        // ptr->state = 1;
        ttl_htime++;
        pthrerr = pthread_cond_wait(ptr->chair, ptr->choputex);
        if (pthrerr != 0)
        {
          fatalerr("Chair", pthrerr, "Wait on chair failed.\n");
        }
        pthrerr = pthread_cond_wait(&ptr->condsticks[ptr->id], ptr->choputex);
        if (pthrerr != 0)
        {
          fatalerr("Chopsticks", pthrerr, "Wait on left chopstick failed.\n");
        }
        pthrerr = pthread_cond_wait(&ptr->condsticks[(ptr->id+1)%ptr->N],ptr->choputex);
        if (pthrerr != 0)
        {
          fatalerr("Chopsticks", pthrerr, "Wait on right chopstick failed.\n");
        }
        // ptr->state = 2;
        ttl_etime++;
        pthrerr = pthread_cond_signal(&ptr->condsticks[(ptr->id+1)%ptr->N]);
        if (pthrerr != 0)
        {
          fatalerr("Chopsticks", pthrerr, "Signal on right chopstick failed.\n");
        }
        pthrerr = pthread_cond_signal(&ptr->condsticks[ptr->id]);
        if (pthrerr != 0)
        {
          fatalerr("Chopsticks", pthrerr, "Signal on left chopstick failed.\n");
        }
        pthrerr = pthread_cond_signal(ptr->chair);
        if (pthrerr != 0)
        {
          fatalerr("Chair", pthrerr, "Signal on chair failed.\n");
        }
        // ptr->state = 0;
        ttl_ttime++;
      }
    }
    pthrerr = pthread_mutex_unlock(ptr->blockutex);
    if (pthrerr != 0)
      fatalerr("philosopher", pthrerr, "Mutex unlock failed.\n");
    }
  return NULL;
}

/***********************************************************************
                                C L K
************************************************************************/
void *clk(void *vptr)
{
  int tick = 0;
  float avg_t_time, avg_h_time, avg_e_time;
  int pthrerr;
  struct thread_arg *ptr;

  ptr = (struct thread_arg*)vptr;

  //T cycles
  while(tick < ptr->T)
  {
    pthrerr = pthread_mutex_lock(ptr->blockutex);
    if (pthrerr != 0)
    {
      fatalerr("clk", pthrerr, "Failed, Mutex lock...\n");
    }

    //complete when all N philosophers have blocked per cycle
    while (nblocked < ptr->N)
    {
      pthrerr = pthread_cond_wait(ptr->condclk, ptr->blockutex);
      if (pthrerr != 0)
      {
        fatalerr("clk", pthrerr, "Failed, Condition wait...\n");
      }
    }
    nblocked = 0;

    /*STATISTICS OPERATIONS*/
    pthrerr = pthread_cond_broadcast(ptr->start_line);
    if (pthrerr != 0)
    {
      fatalerr("clk", pthrerr, "Failed, Clock broadcast...\n");
    }
    pthrerr = pthread_mutex_unlock(ptr->blockutex);
    if (pthrerr != 0)
    {
      fatalerr("clk", pthrerr, "Failed, Mutex unlock...\n");
    }
    // increment tick counter
    tick++;
  }
  //Calculations for avg time in each state
  avg_t_time = (float)ttl_ttime/((float)(T)*(float)(N));
  avg_h_time = (float)ttl_htime/((float)(T)*(float)(N));
  avg_e_time = (float)ttl_etime/((float)(T)*(float)(N));
  // report statistics - these are obviously incorrect
  printf("Average thinking time: %6.2f\n",rand0_1(&ptr->seed));
  printf("Average hungry time: %6.2f\n",rand0_1(&ptr->seed));
  printf("Average eating time: %6.2f\n",rand0_1(&ptr->seed));
  exit(0);
}

/***********************************************************************
                               M A I N
************************************************************************/
int main(int argc, char **argv)
{
  int i, pthrerr, chaircnt; // number of available chair
  float lam, mu;
  pthread_cond_t chair, condsticks;
  pthread_mutex_t choputex, blockutex;
  struct thread_arg *allargs;
  pthread_t *alltids;

  // parsing the arguments from cmd line
  i=1;
  while (i<argc-1)
    {
      if (strncmp("--lambda",argv[i],strlen(argv[i]))==0)
	  lam     = atof(argv[++i]);
      else if (strncmp("--mu",argv[i],strlen(argv[i]))==0)
	  mu      = atof(argv[++i]);
      else if (strncmp("--servers",argv[i],strlen(argv[i]))==0)
	  nserver = atoi(argv[++i]);
      else if (strncmp("--clients",argv[i],strlen(argv[i]))==0)
	  nclient = atoi(argv[++i]);
      else if (strncmp("--ticks",argv[i],strlen(argv[i]))==0)
	  nticks  = atoi(argv[++i]);
      else
	fatalerr(argv[i], 0, "Invalid argument\n");
      i++;
    }

  pthread_cond_t start_line, condclk;
  int chopsticks[N];
  // initialize chopsticks
  for (i = 0; i < N; i++) {
    chopsticks[i] = 1;
  }

  // initialize values for statistics and resource availability
  nblocked=0, nticks=0;
  ttl_ttime=0, ttl_htime=0, ttl_etime=0;
  chaircnt=N-1;

  // initialize synchronization mechanisms: condition variables and mutexes
  pthrerr = pthread_cond_init(&start_line, NULL);
  if (pthrerr != 0)
    fatalerr(argv[0], 0, "Failed to initialize start_line.\n");
  pthrerr = pthread_cond_init(&condclk, NULL);
  if (pthrerr != 0)
    fatalerr(argv[0], 0, "Failed to initialize condclk.\n");
  pthrerr = pthread_cond_init(&chair, NULL);
  if (pthrerr != 0)
    fatalerr(argv[0], 0, "Failed to initialize chair.\n");
  pthrerr = pthread_cond_init(&condsticks, NULL);
  if (pthrerr != 0)
    fatalerr(argv[0], 0, "Failed to initialize condsticks.\n");
  pthrerr = pthread_mutex_init(&choputex, NULL);
  if (pthrerr != 0)
    fatalerr(argv[0], 0, "Failed to initialize choputex.\n");
  pthrerr = pthread_mutex_init(&blockutex, NULL);
  if (pthrerr != 0)
    fatalerr(argv[0], 0, "Failed to initialize blockutex.\n");

  /* allocate memory for the threads and the struct for
     their args: N philosophers */
  allargs = (struct thread_arg*) malloc((N+1)*sizeof(struct thread_arg));
  if (allargs == NULL)
    fatalerr(argv[0], 0, "Memory allocation failed.\n");
  alltids = (pthread_t*) malloc((N)*sizeof(pthread_t));
  if (alltids == NULL)
    fatalerr(argv[0], 0, "Memory allocation failed.\n");

  // initialize attributes for all the thread arguments
  allargs[0].start_line = &start_line;
  allargs[0].N = N;
  allargs[0].T = T;
  allargs[0].mu = mu;
  allargs[0].state = 0;
  allargs[0].sticks = chopsticks;
  allargs[0].seed = 100;
  allargs[0].condclk = &condclk;
  allargs[0].chair = &chair;
  allargs[0].condsticks = &condsticks;
  allargs[0].choputex = &choputex;
  allargs[0].blockutex= &blockutex;
  for (i = 0; i < N; i++){
    allargs[i] = allargs[0];
    allargs[i].id = i;
    allargs[i].seed += i;
    // create all N threads with runner function philosopher
    pthrerr = pthread_create(alltids+i,NULL,philosopher,allargs+i);
    if (pthrerr!=0)
      fatalerr(argv[0], pthrerr, "Thread creation failed.\n");
  }
  allargs[N] = allargs[0];
  allargs[N].seed += N;

  // invoke the clock to report the simulation statistics
  clk((void*)(allargs + N));
  exit(-1);
}

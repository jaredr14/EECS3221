/***********************************************************************/
/**      Author: Jared Rego                                           **/
/**        Date: Jun. 2019                                            **/
/** Description: Skeleton code for Assgn. II                          **/
/***********************************************************************/

struct thread_arg
{
  int N, T, id, state, nticks;
  float lam, mu;
  pthread_cond_t *start_line, *condclk, *chair, *condsticks;	/* find a better name */
  pthread_mutex_t *choputex, *blockutex;
  unsigned int seed;		/* Needed for the random number genarator */
  int *sticks;
};

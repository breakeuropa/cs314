#include <stdio.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STACK_SIZE SIGSTKSZ

typedef unsigned int tid_t;
typedef struct thread
{
	tid_t tid;
	int ticket;
	int nice;
      	ucontext_t ctx;
      	struct thread *next;
} thread_t;


tid_t tid_counter = 0;
thread_t *scheduler_thread = NULL;
thread_t *threads = NULL;

void set_timer();

void join()
{
	for (int i = 0; i < tid_counter; i++)
	{
	    	pause();
	}
}

//RR
void scheduler()
{
	int totalTickets = 0;
	int winner;
      	
	if(threads == NULL)
	{
		printf("THREADS = NULL\n");
	    	return;
      	}
	else
	{
	    	thread_t *t = threads;
	    	printf("\n");
		while(t != NULL)
		{
			totalTickets += t->ticket;
		  	printf("%d ", t->tid);
		  	t = t->next;
	    	}
	    	printf("\n\nTotoal tikcets: %d\n", totalTickets);
      	} //view of available threads when scheduler runs
	
	winner = rand()% (totalTickets + 1);
	printf("winner: %d\n", winner);
	printf("threads points to: %d\n", threads->tid);	

	thread_t *chosen = threads;
	thread_t *previous = threads;

	while(chosen->ticket < winner) //finds the correct thread
	{
		printf("\tfinding correct thread...\n");
		winner -= chosen->ticket;	
		//printf("winner: %d\n", winner);
		chosen = chosen->next;
	}
	
	if (chosen == threads)	//if at head of queue
	{
		printf("thread @ head chosen\n");
		if (chosen->next != NULL)
		{
			threads = chosen->next;
			printf("threads points to: %d\n", threads->tid);
			chosen->next = NULL;
		}
		else
		{
			printf("only one thing left\n");
		}
	}
	else
	{
		printf("not at head\n");
		while(previous->next != chosen) //&& chosen->next != NULL) //sets previous
		{
			printf("\tfinding previous...\n");
			previous = previous->next;
		}
		if (chosen->next != NULL)
		{
			printf("\tsetting previous next to chosen next...\n");
			previous->next = chosen->next;
		}
		else
		{	
			//previous = threads;
		}
		printf("\tprevious->next's tid is: %d", previous->next->tid);
		chosen->next = NULL;
	}
	 
	printf("\n\nThe chosen process is: %d with %d tickets!!", chosen->tid, chosen->ticket);
      	printf("\nThe previous thread is: %d\n\n", previous->tid);
	set_timer();
      	swapcontext(&(scheduler_thread->ctx), &(chosen->ctx)); //takes current context, records state, loads context swapping to
								//from, to
}

void set_timer()
{
      	struct itimerval timer;
      	struct sigaction sa;
      	int ms = 10;

	/* Install signal handler for the timer. */
      	memset (&sa, 0, sizeof (sa));
      	sa.sa_handler =  (void (*)(int))scheduler;
      	sigaction (SIGALRM, &sa, NULL);

	/* Configure the timer to expire after ms msec... */
      	timer.it_value.tv_sec = 0;
      	timer.it_value.tv_usec = ms*1000;
      	timer.it_interval.tv_sec = 0;
      	timer.it_interval.tv_usec = 0;

	if (setitimer (ITIMER_REAL, &timer, NULL) < 0) 
	{
	    	perror("Setting timer");
	    	exit(EXIT_FAILURE);
  	}
}


void init_scheduler()
{
	//similar to thread_create but this one is not hooked to anything else, threads are hooked to each other
	//needs to run seperately
      	
	scheduler_thread = (thread_t*)malloc(sizeof(thread_t));
      	getcontext(&scheduler_thread->ctx);
      	scheduler_thread->tid = tid_counter;
      	tid_counter++;
      	
	//  sched_thread->state = ready;
  	
	scheduler_thread->ctx.uc_link = NULL;
      	scheduler_thread->ctx.uc_stack.ss_sp = malloc(STACK_SIZE);
      	scheduler_thread->ctx.uc_stack.ss_size = STACK_SIZE;
      	scheduler_thread->ctx.uc_stack.ss_flags = 0;
      	makecontext(&scheduler_thread->ctx, scheduler, 0);
      	printf("\ninit complete\n\n");
      	set_timer(); 
}

void thread_create(void (startfunc)())
{
	thread_t *newthread = (thread_t*)malloc(sizeof(thread_t));
	

	if(scheduler_thread == NULL)	
	{
	    	init_scheduler();
  	}
  
	newthread->next = NULL;

	getcontext(&newthread->ctx);				//initialize the structure, set up phase
	newthread->ctx.uc_link = &(scheduler_thread->ctx);	//addy of the context of scheduler thread, return control back here when thread exits
      	newthread->ctx.uc_stack.ss_sp = malloc(STACK_SIZE);	//allocating space for a stack
      	newthread->ctx.uc_stack.ss_size = STACK_SIZE;		//size of stack
      	newthread->ctx.uc_stack.ss_flags = 0;			//idk?
      	newthread->tid = tid_counter;				//send tid counter (global)
      	makecontext(&newthread->ctx, startfunc, 1, newthread->tid);	//associate startfunc (passed in) w/ context created

      	
	newthread->nice = rand()%41 - 20;
	//printf("thread%d's nice value: %d\n", newthread->tid, newthread->nice);
	newthread->ticket = 105 - (5*(newthread->nice));
        printf("thread%d's nice is %d & has %d tickets\n", newthread->tid, newthread->nice, newthread->ticket);
	newthread->next = threads;				//head insert basically
      	threads = newthread;
  
      	tid_counter++;  
}

void done()
{
	threads = threads->next;
}

void test()
{
	printf("in thread %d\n", threads->tid);
      	//done();
}

int main()
{
	srand(time(NULL));
	thread_create(test);
      	thread_create(test);
      	thread_create(test);
	
      	join();			//makes main wait

      	return 0;
}

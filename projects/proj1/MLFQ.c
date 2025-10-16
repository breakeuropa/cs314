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

int PUSER = 50;
int QUANTA = 0;

typedef unsigned int tid_t;
typedef struct thread
{
	tid_t tid;
      	ucontext_t ctx;
	int prio;
	int pnice;
	int estCPU;
      	struct thread *next;
} thread_t;
/*
typedef struct queue
{
	struct queue *next;
	thread_t* tHead;
} queue;
*/

tid_t tid_counter = 0;
thread_t *scheduler_thread = NULL;
thread_t *threads = NULL;

thread_t * queue[128] = {0};

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
  	if((QUANTA % 100) == 0)
	{
		thread_t *temp = threads;

		while(temp!=NULL)
		{
			temp->estCPU = (2/3) * temp->estCPU + temp->prio;
			printf("temp's new prio: %d\n", temp->prio);
			temp = temp->next;
		}
	}
	if(threads == NULL)
	{
		printf("shit i sempty\n");
	    	return;
      	}
	else
	{
	    	thread_t *t = threads;
	    	
		while(t!=NULL)
		{
		  	printf("%d ", t->tid);
		  	t = t->next;
	    	}
	    	printf("\n");
      	} //view of available threads when scheduler runs

	thread_t *current = threads;

	if ((current->estCPU % 4) == 0)
	{
		printf("\tchanging priority...\n");
		current->prio = PUSER + (current->estCPU / 4) + 2 * current->pnice;
		
		if (queue[current->prio] == 0)
		{
			queue[current->prio] = current;
		}
		else
		{
			thread_t *prev = NULL;
			thread_t *t = queue[current->prio];
			while (t->next != NULL)
			{
				printf("t = %d, ", t->tid);
				t = t->next;
			}
			t->next = current;
			printf("\nattemtpgint to print queue...\n\n");
			printf("queue[%d] has: ", queue[current->prio]);
		}
		
		printf("\tcurrent's prio is: %d\n", current->prio);
	}

	
	printf("b4 thread reset bs idk\n");

      	thread_t *oldhead = threads;
      	threads = threads->next;
      	oldhead->next = NULL;
      	thread_t *newhead = threads;

      	if(newhead == NULL)
	{
		threads = oldhead;
	}
      	else
	{
	    	thread_t *temp = threads;
	    	while(temp->next != NULL)
		{
		   	temp = temp->next;
	    	}
	    	temp->next = oldhead;
	}
	
	printf("b4 timer\n");
      	set_timer();
	current->estCPU += 1;
      	swapcontext(&(scheduler_thread->ctx), &(threads->ctx)); //takes current context, records state, loads context swapping to
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

	printf("\nTotal Quanta: %d\n\n", QUANTA);
	QUANTA += 1;
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
      	printf("init complete\n");
      	set_timer(); 
}


void insertAtHead(thread_t **head, thread_t *thread)
{
	thread->next = *head;
	*head = thread;
	printf("inserted @ mind blowing head!!!\n");
}

void thread_create(void (startfunc)())
{
	printf("\n\ncreating thread...\n\n");
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
	
	newthread->pnice = rand()%21;
	newthread->estCPU = 0;
	newthread->prio = PUSER + newthread->estCPU;

	printf("thread%d's penice is %d & has prio of %d\n", newthread->tid, newthread->pnice, newthread->prio);

	insertAtHead(&queue[newthread->prio], newthread);
	printf("queue[%d] has thread %d\n", queue[newthread->prio], newthread->tid);

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
      	done();
}

int main()
{
	thread_create(test);
      	thread_create(test);
      	thread_create(test);
      	thread_create(test);
      	thread_create(test);
      	thread_create(test);
      	thread_create(test);
      	join();			//makes main wait

      	return 0;
}

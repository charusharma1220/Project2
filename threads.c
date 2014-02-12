/*
 * threads.c
 *
 *  Created on: Feb 9, 2014
 *      Author: Charu
 */
#include "threads.h"
#include <malloc.h>
#include <stdio.h>
#include <setjmp.h>
#define STACKSIZE 1024000

int current_thread_number=0;

struct thread{
	int threadid;
	void *arg;
	void (*f)(void *arg);
	bool first_time;
	unsigned char *esp;
	unsigned char *ebp;
	jmp_buf buf;
        int env_set;
	unsigned char* stack;
	struct thread* prev;
	struct thread* next;
};

struct thread* first_thread=NULL;
struct thread* last_thread=NULL;
struct thread* current_thread=NULL;
struct thread* prev_thread=NULL;

struct thread* thread_create(void (*f)(void *arg), void *arg)
{
	struct thread* newThread;
	newThread=malloc(sizeof(Thread));
    newThread->stack=malloc(STACKSIZE);
	current_thread_number++;
	newThread->threadid=current_thread_number;
    newThread->esp=newThread->ebp=(unsigned char*)((int)(newThread->stack+STACKSIZE)&(0xFFFFFFF8));
    	newThread->env_set = 0;
	newThread->prev = NULL;
	newThread->next = NULL;
	newThread->arg=arg;
	newThread->first_time=TRUE;
	newThread->f=f;
    return newThread;
}

void thread_add_runqueue(struct thread *t)
{
	if (first_thread==NULL){
		first_thread=t;
		last_thread=t;
	}
	else{
		t->prev=last_thread;
		last_thread->next=t;
		last_thread=t;
	}
}

/*    suspends thread by saving current thread's context to struct thread, then calls
 *  scheduler and dispatcher.  On the next call to thread_yield, process is returned 
 *  from setjmp call and returns to the calling function.
 */
void thread_yield(void)
{
	printf("entered thread_yield\n");
        schedule();
        dispatch();
        printf("setjmp returning to thread %d.\n", current_thread->threadid);
        return;
}

/*      prepares the scheduled thread for execution.
 * goals:
 *  - save sp & bp for last thread
 *  - restore scheduled thread sp & bp
 * note: might be helpful to think of current_thread as the next_thread to dispatch,
 *     not the thread we are departing from.
 */
void dispatch(void){
	printf("Dispatch the threads\n");
	if (current_thread==NULL){
		printf("No threads to dispatch");
		return;
	}
        if (prev_thread != NULL) {
                     // store sp and bp to register a to be saved by setjmp
                printf("entering setjmp\n");
                __asm __volatile("mov %%rsp, %%rax" : "=a" (current_thread->esp) : );
                __asm __volatile("mov %%rbp, %%rax" : "=a" (current_thread->ebp) : );
                prev_thread->env_set = setjmp(prev_thread->buf);//call to longjmp with env 
                                                                //  will start here.
                if (prev_thread->env_set){
                        current_thread=previous_thread;//thread is returning from setjmp
                }
        }
 
              //start running the thread's function if running for the first time
	if (current_thread->first_time){
		printf("first time-dispatching\n");
		current_thread->first_time=FALSE;
		printf("saved registers\n");
		current_thread->f(current_thread->arg);
		thread_exit();
		return;
	}
        else {   //thread is resuming next thread via longjmp
                      // restore sp and bp (stored in register a)
		__asm __volatile("mov %%rax, %%rsp" : : "a" (current_thread->esp) );
		__asm __volatile("mov %%rax, %%rbp" : : "a" (current_thread->ebp) );
		printf("restored registers\n");
                if (!current_thread->env_set) // next thread is not returning from longjmp
                        longjmp(current_thread->buf, 1);
                else {
                        current_thread->env_set = 0;  //reset current_thread's env_set
                                                      //variable if returning from longjmp
                }
		return;
	}

	printf("about to take long jump\n");
	printf("threadid:%d\n",current_thread->threadid);
	printf("back from long jump\n");
}

void schedule(void)
{
	if (first_thread==NULL){
		printf("No more threads to schedule");
		return;
	}
	if (current_thread->next==NULL){
		current_thread=first_thread;
		return;
	}
        prev_thread = current_thread;
	current_thread=current_thread->next;
}

void thread_exit(void)
{
	if (current_thread->prev!=NULL)
		(current_thread->prev)->next=current_thread->next;
	if (current_thread->next!=NULL)
		(current_thread->next)->prev=current_thread->prev;
	if (current_thread->prev==NULL)
		first_thread=current_thread->next;
	if (current_thread->next==NULL)
		last_thread=current_thread->prev;
	struct thread* saved_thread=current_thread;
	schedule();
	if (current_thread==NULL){
		printf("All threads have terminated.\n");
		exit(0);
	}
	current_thread->stack=(unsigned char*)(((int)current_thread->stack)-STACKSIZE);
	free(saved_thread);
	dispatch();
}

void thread_start_threading(void){
	printf("Starting threading\n");
	current_thread=first_thread;
	if (current_thread==NULL){
		printf("There are no threads to schedule.\n");
		exit(0);
	}
	while (current_thread!=NULL)
		dispatch();
}

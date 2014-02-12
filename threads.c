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
	int first_time;
	unsigned char *esp;
	unsigned char *ebp;
	jmp_buf buf;
	unsigned char* stack;
	struct thread* prev;
	struct thread* next;
};

typedef struct thread Thread;   //Rename to something different?

struct thread* first_thread=NULL;
struct thread* last_thread=NULL;
struct thread* current_thread=NULL;

struct thread* thread_create(void (*f)(void *arg), void *arg)
{
	struct thread* newThread;
	newThread=malloc(sizeof(Thread));
    newThread->stack=malloc(STACKSIZE);
	current_thread_number++;
	newThread->threadid=current_thread_number;
    newThread->esp=newThread->ebp=(unsigned char*)((int)(newThread->stack+STACKSIZE)&(0xFFFFFFF8));
	printf("ESP value is %u\n",newThread->esp);
	printf("EBP value is %u\n", newThread->ebp);
    newThread->prev = NULL;
	newThread->next = NULL;
	newThread->arg=arg;
	newThread->first_time=1;
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

void thread_yield(void)
{
	//printf("entered thread_yield\n");
	if (!setjmp(current_thread->buf)){
		//printf("entering setjmp\n");
		__asm __volatile("mov %%esp, %%eax" : "=a" (current_thread->esp) : );
		__asm __volatile("mov %%ebp, %%eax" : "=a" (current_thread->ebp) : );
		schedule();
		dispatch();
	}
	else{
		//printf("long jump destination\n");
		return;
	}
}

void dispatch(void){
	//printf("Dispatch the threads\n");
	if (current_thread==NULL){
		//printf("No threads to dispatch");
		exit(0);
	}

	if (current_thread->first_time){
		//printf("first time-dispatching\n");
		current_thread->first_time=0;
		__asm __volatile("mov %%eax, %%esp" : : "a" (current_thread->esp) );
		__asm __volatile("mov %%eax, %%ebp" : : "a" (current_thread->ebp) );
		//printf("saved registers\n");
		current_thread->f(current_thread->arg);
		thread_exit();
		return;
	}
	//printf("about to take long jump\n");
	//printf("threadid:%d\n",current_thread->threadid);
	longjmp(current_thread->buf, 1);
	//printf("took long jump\n");
}

void schedule(void)
{
	if (first_thread==NULL){
		printf("No more threads to schedule\n");
		current_thread=NULL;
		return;
	}
	if (current_thread->next==NULL){
		current_thread=first_thread;
		return;
	}
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
	free(saved_thread);
	dispatch();
}

void thread_start_threading(void){
	//printf("Starting threading\n");
	current_thread=first_thread;
	if (current_thread==NULL){
		printf("There are no threads to schedule.\n");
		return;
	}
	while (current_thread!=NULL)
		dispatch();
}

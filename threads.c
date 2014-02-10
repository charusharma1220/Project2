/*
 * threads.c
 *
 *  Created on: Feb 9, 2014
 *      Author: Charu
 */
#include "threads.h"
#include <setjmp.h>
#define STACKSIZE 262144
#define NULL 0

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

typedef struct thread thread;   //Rename to something different?

struct thread* first_thread=NULL;
struct thread* last_thread=NULL;
struct thread* current_thread=NULL;

struct thread* thread_create(void (*f)(void *arg), void *arg)
{
	struct thread* newThread;
	newThread=aligned_alloc(8, sizeof(thread));
        newThread->stack=aligned_alloc(8,STACKSIZE);
	newThread->prev = NULL;
	newThread->next = NULL;
	newThread->arg=arg;
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
	if (!setjmp(current_thread->buf)){
		__asm __volatile("mov %%esp, %%eax" : "=a" (current_thread->esp) : );
		__asm __volatile("mov %%ebp, %%eax" : "=a" (current_thread->ebp) : );
		schedule();
		dispatch();
	}
	else{
		return;
	}
}
void dispatch(void)
{
	if (current_thread==NULL){
		printf("No threads to dispatch");
		exit(0);
	}
	if (current_thread->first_time){
		current_thread->first_time=0;
		__asm __volatile("mov %%eax, %%esp"::"a"(current_thread->stack));
		__asm __volatile("mov %%eax, %%ebp"::"a"(current_thread->stack));
		current_thread->f(current_thread->arg);
		thread_exit();
		return;
	}
	longjmp(current_thread->buf, 1);
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
	current_thread=current_thread->next;
}
void thread_exit(void)
{

}
void thread_start_threading(void)
{
	current_thread=first_thread;
	if (current_thread==NULL){
		printf("There are no threads to schedule.\n");
		exit(0);
	}
	while (current_thread!=NULL)
		dispatch();
}


/*
 * threads.c
 *
 *  Created on: Feb 9, 2014
 *      Author: Charu
 */
#include "threads.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
int current_thread_number=0;
#include <setjmp.h>
#define STACKSIZE 1024000

//keep track of number of threads
//store main stack's esp, ebp, and main_buf for main's stack setjmp/longjmp
unsigned long main_esp;
unsigned long  main_ebp;
jmp_buf main_buf;

//thread structure
//store threads in doubly linked list
struct thread{
	int threadid;
	void *arg;
	void (*f)(void *arg);
	int first_time;
	unsigned char* esp;
	unsigned char* ebp;
	jmp_buf buf;
	unsigned char* stack;
	struct thread* prev;
	struct thread* next;
};

typedef struct thread Thread;

//keep first, last, and current_threads initialized to NULL for the linked list
struct thread* first_thread=NULL;
struct thread* last_thread=NULL;
struct thread* current_thread=NULL;

//Creates new thread, assigns thread structure values
struct thread* thread_create(void (*f)(void *arg), void *arg)
{
	struct thread* newThread;
	//allocates memory for thread
	newThread=malloc(sizeof(Thread));
	if (newThread==NULL){
		printf("Malloc for thread failed");
		exit(1);
	}
	//allocates memory for thread stack
    newThread->stack=malloc(STACKSIZE);
    if (newThread->stack==NULL){
    	printf("Malloc for thread stack failed");
    	exit(1);
    }
	current_thread_number++;
	newThread->threadid=current_thread_number;
	//aligns memory to 8 bytes
    newThread->esp=newThread->ebp=(unsigned char*)(((unsigned long)(newThread->stack+STACKSIZE)&(0xFFFFFFFFFFFFFFF8))-512);
    newThread->prev = NULL;
	newThread->next = NULL;
	newThread->arg=arg;
	//boolean variable first time keeps track of whether function has been entered for each thread, initialize as first time
	newThread->first_time=1;
	newThread->f=f;
    return newThread;
}

//Enqueues a thread to the runqueue
void thread_add_runqueue(struct thread *t)
{
	//Case 1: t is the first thread, add to linked list
	if (first_thread==NULL){
		current_thread=t;
		first_thread=t;
		last_thread=t;
		return;
	}
	//Case 2: current_thread is the first thread, add t as the last_thread in the linked list
	if (current_thread==first_thread){
		t->prev=last_thread;
		last_thread->next=t;
		last_thread=t;
		return;
	}
	//Case 3 (General Case): add t to linked list before current_thread
	current_thread->prev->next=t;
	t->prev=current_thread->prev;
	current_thread->prev=t;
	t->next=current_thread;
}

//Switches control to the next thread when called by the user
void thread_yield(void)
{
	//saves the context of current_thread in a buffer for current_thread
	//calls schedule and dispatch if setjmp becomes 0
	if (!setjmp(current_thread->buf)){
		__asm __volatile("mov %%rsp, %%rax" : "=a" (current_thread->esp) : );
		__asm __volatile("mov %%rbp, %%rax" : "=a" (current_thread->ebp) : );
		schedule();
		dispatch();
	}
	//otherwise, longjmp has returned here
	else{
		return;
	}
}

//Starts executing a specified thread
void dispatch(void){
	//Case 1: There are no threads in the queue
	if (current_thread==NULL){
		printf("No threads to dispatch");
		exit(1);
	}
	//Case 2 (General Case): current thread is executing for the first time
	if (current_thread->first_time){
		current_thread->first_time=0;
		//switch the stack of current thread
		__asm __volatile("mov %%rax, %%rsp" : : "a" (current_thread->esp) );
		__asm __volatile("mov %%rax, %%rbp" : : "a" (current_thread->ebp) );
		//run function f as specified for current thread
		current_thread->f(current_thread->arg);
		//long jump to the primary loop, need to execute on main's stack
		longjmp(main_buf,1);
		exit(0);
	}
	//resuming function from yield, long jump to thread yield
	longjmp(current_thread->buf, 1);
}

//Reassigns current thread to the next thread
void schedule(void)
{
	//Case 1: No more threads to schedule
	if (first_thread==NULL){
		current_thread=NULL;
		return;
	}
	//Case 2: current thread is the last thread, go to the first thread
	if (current_thread->next==NULL){
		current_thread=first_thread;
		return;
	}
	//Case 3 (General Case): Set current thread to the next thread
	current_thread=current_thread->next;
}

//Exits thread IF USER HAS EXPLICITLY CALLED THREAD_EXIT
void thread_exit (void){
	//long jump to the primary loop
	longjmp(main_buf,1);
}

//Removes the thread from the linked list, schedules next thread, and frees stack/data structure
void thread_terminate(void)
{
	//Removing thread from linked list
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
	//Freeing structure and stack memory
	free(saved_thread->stack);
	free(saved_thread);
}

//Starts threading. Also serves as the primary loop.
void thread_start_threading(void){
	current_thread=first_thread;
	//Case 1: There are no threads to schedule.
	if (current_thread==NULL){
		return;
	}
	//Using setjmp/longjmp to use main stack for thread_exits...
	while (current_thread!=NULL){
		//store current context (main stack info) in a global buffer main_buf
		if (!setjmp(main_buf)){
			__asm __volatile("mov %%rsp, %%rax" : "=a" (main_esp) : );
			__asm __volatile("mov %%rbp, %%rax" : "=a" (main_ebp) : );
			dispatch();
		}
		//A thread has terminated, return point for thread_exit and dispatch long jump
		else{
			thread_terminate();
		}
	}
}

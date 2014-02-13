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
#include <setjmp.h>
#define STACKSIZE 1024000

int current_thread_number=0;
unsigned long main_esp;
unsigned long  main_ebp;
jmp_buf main_buf;

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
        newThread->esp=newThread->ebp=(unsigned char*)(((unsigned long)(newThread->stack+STACKSIZE)&(0xFFFFFFFFFFFFFFF8))-512);
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
		__asm __volatile("mov %%rsp, %%rax" : "=a" (current_thread->esp) : );
		__asm __volatile("mov %%rbp, %%rax" : "=a" (current_thread->ebp) : );
		schedule();
		dispatch();
	}
	else{
		//printf("long jump destination\n");
		return;
	}
}

void dispatch(void){
	if (current_thread==NULL){
		printf("No threads to dispatch");
		exit(0);
	}
	if (current_thread->first_time){
		current_thread->first_time=0;
		__asm __volatile("mov %%rax, %%rsp" : : "a" (current_thread->esp) );
		__asm __volatile("mov %%rax, %%rbp" : : "a" (current_thread->ebp) );
		current_thread->f(current_thread->arg);
		longjmp(main_buf,1);
		printf("Oops, don't want to be here after long jump!\n");
		exit(0);
	}
	longjmp(current_thread->buf, 1);
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
	free(saved_thread->stack);
	free(saved_thread);
	if (current_thread==NULL)
		printf("All threads have terminated.\n");
}

void thread_start_threading(void){
	current_thread=first_thread;
	if (current_thread==NULL){
		printf("There are no threads to schedule.\n");
		return;
	}
	while (current_thread!=NULL){
		if (!setjmp(main_buf)){
			__asm __volatile("mov %%rsp, %%rax" : "=a" (main_esp) : );
			__asm __volatile("mov %%rbp, %%rax" : "=a" (main_ebp) : );
			dispatch();
		}
		else{
			thread_exit();
		}
	}
}
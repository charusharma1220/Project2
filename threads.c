/*
 * threads.c
 *
 *  Created on: Feb 9, 2014
 *      Author: Charu
 */
#include "threads.h"

struct thread* thread_create(void (*f)(void *arg), void *arg){
	struct thread *newThread=aligned_alloc(sizeof(char), STACKSIZE);
	newThread->prev=NULL;
	newThread->next=NULL;
	newThread->esp=STACKSIZE;
	newThread->ebp=NULL;

        return newThread;
}
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);


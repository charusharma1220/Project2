/*
 * threads.c
 *
 *  Created on: Feb 9, 2014
 *      Author: Charu
 */
#include "threads.h"

struct thread* thread_create(void (*f)(void *arg), void *arg){
	struct thread* newThread;
        newThread = aligned_alloc(sizeof(char), STACKSIZE);
	newThread->prev = NULL;
	newThread->next = NULL;
	newThread->esp = newThread;
	newThread->ebp = NULL;
        return newThread;
}
void thread_add_runqueue(struct thread *t){
	struct thread* current_thread = t;
	struct thread* last_thread = current_thread->prev;
	current_thread->next = last_thread->next;
	last_thread->next = current_thread;
}
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);


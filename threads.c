/*
 * threads.c
 *
 *  Created on: Feb 9, 2014
 *      Author: Charu
 */
#ifndef THREADS_H_
#define THREADS_H_
#define NULL 0

struct thread{
	int threadid;
	void *f(void *arg);
	unsigned char *esp;
	unsigned char *ebp;
	struct thread* prev;
	struct thread* next;
};

struct thread *thread_create(void (*f)(void *arg), void *arg){
	struct thread newThread=posix_memalign(8, STACKSIZE);
	newThread->prev=NULL;
	newThread->next=NULL;
	newThread->esp=STACKSIZE;
	newThread->ebp=;
}
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

#endif // THREADS_H



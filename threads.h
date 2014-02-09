#ifndef THREADS_H_
#define THREADS_H_
#ifndef NULL
#define NULL 0
#endif

#include <stdlib.h>

#define STACKSIZE sizeof(4096)
//#define STACKSIZE size_t 4096

struct thread{
	int threadid;
	void* f;
	unsigned char *esp;
	unsigned char *ebp;
	struct thread* prev;
	struct thread* next;
};

struct thread* thread_create(void (*f)(void *arg), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

#endif // THREADS_H

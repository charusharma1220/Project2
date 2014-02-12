#ifndef THREADS_H_
#define THREADS_H_

#ifndef Bool
typedef int bool;
#endif
#define TRUE 1
#define FALSE 0

struct thread;
typedef struct thread Thread;   

Thread *thread_create(void (*f)(void *arg), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

#endif // THREADS_H

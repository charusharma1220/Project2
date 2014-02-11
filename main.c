#include <stdio.h>
#include "threads.h"

void f3(void *arg)
{
    int i = 0;
    while (1) {
        printf("thread 3: %d\n", i++);
        thread_yield();
    }
}

void f2(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 2: %d\n",i++);
        if (i == 10) {
            i = 0;
        }
        thread_yield();
        printf("came from a long jump\n");
    }

void f1(void *arg)
{
    int i = 100;
    struct thread *t2 = thread_create(f2, NULL);
    thread_add_runqueue(t2);
    struct thread *t3 = thread_create(f3, NULL);
    thread_add_runqueue(t3);
    while(1) {
        printf("thread 1: %d\n", i++);
        if (i == 110) {
            i = 100;
        }
        thread_yield();
        printf("came from a long jump\n");
    }
}

int main(int argc, char **argv)
{
	printf("in main\n");
    struct thread *t1 = thread_create(f1, NULL);
    printf("thread t1 created\n");
    thread_add_runqueue(t1);
    printf("thread t1 added to queue\n");
    thread_start_threading();
    printf("\nexited\n");
    return 0;
}

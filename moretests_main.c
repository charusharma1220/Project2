#include <stdio.h>
#include "threads.h"

void f53(int *arg) {
	printf("Entered thread 3, arg: %d\n",*arg);
	printf("thread 3 exiting\n");
}



void f52(int *arg) {
	printf("Entered thread 2, arg: %d\n",*arg);
	printf("thread 2 exiting\n");
}



void f51(int *arg) {
    printf("Entered thread 1, arg: %d\n",*arg);
    printf("thread 1 exiting\n");
}

void test5() {
	printf("This test starts three threads, first thread exits right away, then thread 2 exits right away, then third thread exits right away\n");
	int i=1;
	struct thread *t1 = thread_create((void *)f51, (void *)&i);
	thread_add_runqueue(t1);
	int j=2;
	struct thread *t2 = thread_create((void *)f52, (void *)&j);
	thread_add_runqueue(t2);
	int k=3;
	struct thread *t3 = thread_create((void *)f53, (void *)&k);
	thread_add_runqueue(t3);
	thread_start_threading();
	printf("exited\n");
}

void test4() {
	printf("no threads added, but start threading\n");
	thread_start_threading();
	printf("exited\n");
}

void f31(void *arg)
{
    int i = 100;
    while(1) {
        printf("thread 1: %d\n", i++);
        if (i == 110) {
            i = 100;
        }
        thread_yield();
    }
}

void test3() {
	printf("single thread in a loop\n");
	struct thread *t1 = thread_create(f31, NULL);
	thread_add_runqueue(t1);
	thread_start_threading();
	printf("exited\n");
}

void f23(int *arg) {
	int local=0;
	printf("Entered thread 3, arg: %d\n",*arg);
	while(1) {
			local=local+3;
	    	thread_yield();
	    	printf("thread3 return from yield, local = %d \n",local);
	    	if (local>50) {
	    		printf("thread 3 exiting\n");
	    		break;
	    	}
	}
}


void f22(int *arg) {
	int local=0;
	printf("Entered thread 2, arg: %d\n",*arg);
	while(1) {
			local=local+2;
	    	thread_yield();
	    	printf("thread2 return from yield, local = %d \n",local);
	    	if (local>3) {
	    		printf("thread 2 exiting\n");
	    		break;
	    	}
	}
}



void f21(int *arg) {
    printf("Entered thread 1, arg: %d\n",*arg);
    printf("thread 1 exiting\n");
}

void test2() {
	printf("This test starts three threads, first thread exits right away, then thread 2 exits, a bit later thread 3 exits\n");
	int i=1;
	struct thread *t1 = thread_create((void *)f21, (void *)&i);
	thread_add_runqueue(t1);
	int j=2;
	struct thread *t2 = thread_create((void *)f22, (void *)&j);
	thread_add_runqueue(t2);
	int k=3;
	struct thread *t3 = thread_create((void *)f23, (void *)&k);
	thread_add_runqueue(t3);
	thread_start_threading();
	printf("exited\n");
}

void f13(void *arg)
{
    int i=0;
    while (1) {
        printf("thread 3: %d\n", i++);
        thread_yield();
    }
}

void f12(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 2: %d\n",i++);
        if (i == 10) {
            i = 0;
        }
        thread_yield();
    }
}

void f11(void *arg)
{
    int i = 100;
    struct thread *t2 = thread_create(f12, NULL);
    thread_add_runqueue(t2);
    printf("after second add\n");
    struct thread *t3 = thread_create(f13, NULL);
    thread_add_runqueue(t3);
    printf("after third add\n");
    while(1) {
        printf("thread 1: %d\n", i++);
        if (i == 110) {
            i = 100;
        }
        thread_yield();
    }
}

void test1() {
	printf("this is the original test case\n");
	struct thread *t1 = thread_create(f11, NULL);
	thread_add_runqueue(t1);
	printf("after first add\n");
	thread_start_threading();
	printf("exited\n");
}


int main(int argc, char **argv)
{
	int test_number;
	test_number=2;

	while (1) {
		printf("type test number\n");
		scanf("%d", &test_number);
		switch(test_number) {
	    	case 1  :
	    		test1();
	    		break;
	    	case 2 :
	    		test2();
	    		break;
	    	case 3 :
	    		test3();
	    		break;
	    	case 4 :
	    		test4();
	    		break;
	    	case 5 :
	    		test5();
	    		break;
	    	default :
	    		printf("exiting program\n");
	    		break;
		}
	}
}

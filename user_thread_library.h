#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#define MAX_THREADS 10

typedef struct {
    int tid;
    pthread_t pthread_id;
    void *(*start_routine)(void *);
    void *arg;
    void *retval;
    int joined;
    int suspended;
} mythread_t;

mythread_t threads[MAX_THREADS];





typedef struct {
    atomic_int flag;
} lock_t;

int mythread_create(void *(*start_routine)(void *), void *arg);
int mythread_yield(void);
int mythread_self(void);
int mythread_join(int tid, void **retval);
int mythread_init(int time_slice);
int mythread_terminate(int tid);
int mythread_suspend(int tid);
int mythread_resume(int tid);
int lock_init(lock_t *lock);
int acquire(lock_t *lock);
int release(lock_t *lock);
void *thread_function(void *arg);

#endif /* MYTHREAD_H */
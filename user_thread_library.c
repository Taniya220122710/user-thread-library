#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <signal.h>
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
int next_tid = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// Global variables for signal handling
pthread_t suspended_thread;

typedef struct {
    atomic_int flag;
} lock_t;


// Signal handler function
void handle_suspend_signal(int signum) {
    printf("Received suspend signal. Suspending current thread %lu\n", pthread_self());
    suspended_thread = pthread_self();
    // Implement your suspension logic here...
}

int mythread_create(void *(*start_routine)(void *), void *arg) {
    // ... (previous thread management code)
     int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < MAX_THREADS; ++i) {
        if (threads[i].tid == 0) {
            threads[i].tid = next_tid++;
            threads[i].start_routine = start_routine;
            threads[i].arg = arg;
            threads[i].retval = NULL;
            threads[i].joined = 0;
            threads[i].suspended = 0;
            
            if (pthread_create(&(threads[i].pthread_id), NULL, start_routine, arg) != 0) {
                pthread_mutex_unlock(&mutex);
                return -1;
            }
            
            printf("Thread %d created\n", threads[i].tid);
            pthread_mutex_unlock(&mutex);
            return threads[i].tid;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}
int mythread_self(void) {
    // ... (previous thread self code)
     pthread_t self_id = pthread_self();
    int i;
    for (i = 0; i < MAX_THREADS; ++i) {
        if (pthread_equal(threads[i].pthread_id, self_id)) {
            return threads[i].tid;
        }
    }
    return -1;
}
static int mythread_yield(void) {
    // ... (previous thread yield code)
    if (threads[mythread_self()].suspended) {
        return 0;
    }
    sched_yield();
    return -1; // Return error code on yield failure
}



int mythread_join(int tid, void **retval) {
    // ... (previous thread join code)
        int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < MAX_THREADS; ++i) {
        if (threads[i].tid == tid) {
            if (threads[i].joined) {
                pthread_mutex_unlock(&mutex);
                return -1;
            }
            threads[i].joined = 1;
            if (pthread_join(threads[i].pthread_id, &(threads[i].retval)) != 0) {
                pthread_mutex_unlock(&mutex);
                return -1;
            }
            printf("Thread %d joined\n", tid);
            if (retval != NULL) {
                *retval = threads[i].retval;
            }
            pthread_mutex_unlock(&mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;

}

int mythread_init(int time_slice) {
    // ... (previous thread init code)
     // Initialization code (if any) related to time_slice
    printf("Thread control initialized with time slice %d\n", time_slice);
    return 0;
}

int mythread_terminate(int tid) {
    // ... (previous thread termination code)
      int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < MAX_THREADS; ++i) {
        if (threads[i].tid == tid) {
            if (threads[i].joined) {
                pthread_mutex_unlock(&mutex);
                return -1;
            }
            pthread_cancel(threads[i].pthread_id);
            threads[i].tid = 0;
            threads[i].joined = 0;
            printf("Thread %d terminated\n", tid);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

int mythread_suspend(int tid) {
    // ... (previous thread suspend code)
       int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < MAX_THREADS; ++i) {
        if (threads[i].tid == tid) {
            threads[i].suspended = 1;
            printf("Thread %d suspended\n", tid);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

int mythread_resume(int tid) {
    int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < MAX_THREADS; ++i) {
        if (threads[i].tid == tid) {
            threads[i].suspended = 0;
            printf("Thread %d resumed\n", tid);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

int lock_init(lock_t *lock) {
    lock->flag = 0; // Set the lock initially as unlocked
    return 0; // Return success
}

int acquire(lock_t *lock) {
    while (atomic_exchange(&lock->flag, 1) == 1) {
        // If the lock is already taken, keep spinning until it's released
        // You may want to implement some form of waiting strategy here
    }
    return 0; // Return success once the lock is acquired
}

int release(lock_t *lock) {
    atomic_store(&lock->flag, 0); // Set the lock as unlocked
    return 0; // Return success
}

void *thread_function(void *arg) {
    // ... (previous thread function code)
    int *num = (int *)arg;
    printf("Thread %d is running, argument: %d\n", mythread_self(), *num);
    mythread_yield();
    return (void *)(*num * 2);
}


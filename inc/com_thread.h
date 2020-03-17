#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#if defined(_WIN32)
#include <windows.h>
#endif

typedef void*(*uavs3e_tfunc_a_t)(void *);
typedef volatile long atom_t;   // 32 bits, signed

#define safe_sem_post(x) if (x) { uavs3e_sem_post(x); }
#define safe_sem_wait(x) if (x) { uavs3e_sem_wait(x); }

#if defined(_WIN32)

#include "com_thread_win32.h"

#define uavs3e_sem_t        sem_t
#define uavs3e_sem_init     sem_init
#define uavs3e_sem_destroy  sem_destroy
#define uavs3e_sem_wait     sem_wait
#define uavs3e_sem_post     sem_post

#else

#pragma comment(lib, "pthreadVC2.lib")

#include <semaphore.h>
#include <pthread.h>

#define uavs3e_pthread_t                pthread_t
#define uavs3e_pthread_create           pthread_create
#define uavs3e_pthread_join             pthread_join
#define uavs3e_pthread_mutex_t          pthread_mutex_t
#define uavs3e_pthread_mutex_init       pthread_mutex_init
#define uavs3e_pthread_mutex_destroy    pthread_mutex_destroy
#define uavs3e_pthread_mutex_lock       pthread_mutex_lock
#define uavs3e_pthread_mutex_unlock     pthread_mutex_unlock
#define uavs3e_pthread_cond_t           pthread_cond_t
#define uavs3e_pthread_cond_init        pthread_cond_init
#define uavs3e_pthread_cond_destroy     pthread_cond_destroy
#define uavs3e_pthread_cond_broadcast   pthread_cond_broadcast
#define uavs3e_pthread_cond_wait        pthread_cond_wait
#define uavs3e_pthread_attr_t           pthread_attr_t
#define uavs3e_pthread_attr_init        pthread_attr_init
#define uavs3e_pthread_attr_destroy     pthread_attr_destroy
#define uavs3e_pthread_num_processors_np pthread_num_processors_np

#define uavs3e_sem_t        sem_t
#define uavs3e_sem_init     sem_init
#define uavs3e_sem_destroy  sem_destroy
#define uavs3e_sem_wait     sem_wait
#define uavs3e_sem_post     sem_post

#define THREAD_PRIORITY_HIGHEST 2
#define THREAD_PRIORITY_NORMAL  1
#define THREAD_PRIORITY_LOWEST  0

#endif


/**
 * ===========================================================================
 * type defines
 * ===========================================================================
 */
typedef struct uavs3e_threadpool_job_t {
    void *(*func)(void *, void *);
    void *arg;
    void *ret;
    int wait;
    uavs3e_pthread_mutex_t mutex;
    uavs3e_pthread_cond_t  cv;
} threadpool_job_t;

typedef struct uavs3e_threadpool_job_list_t {
    threadpool_job_t   **list;
    int                     i_max_size;
    int                     i_size;
    uavs3e_pthread_mutex_t    mutex;
    uavs3e_pthread_cond_t     cv_fill;  /* event signaling that the list became fuller */
    uavs3e_pthread_cond_t     cv_empty; /* event signaling that the list became emptier */
} threadpool_job_list_t;

typedef struct uavs3e_threadpool_t {
    int            exit;
    int            threads;
    uavs3e_pthread_t *thread_handle;
    void*(*init_func)(void *);
    void (*deinit_func)(void *);
    void           *init_arg;

    threadpool_job_list_t uninit; /* list of jobs that are awaiting use */
    threadpool_job_list_t run;    /* list of jobs that are queued for processing by the pool */
    threadpool_job_list_t done;   /* list of jobs that have finished processing */
} threadpool_t;

int   uavs3e_threadpool_init(threadpool_t **p_pool, int threads, int nodes, void*(*init_func)(void *), void *init_arg, void(*deinit_func)(void *));
void  uavs3e_threadpool_run(threadpool_t *pool, void *(*func)(void *, void *), void *arg, int wait_sign);
void *uavs3e_threadpool_wait(threadpool_t *pool, void *arg);
void  uavs3e_threadpool_delete(threadpool_t *pool);

int   uavs3e_threadpool_run_try(threadpool_t *pool, void *(*func)(void *, void *), void *arg, int wait_sign);
void *uavs3e_threadpool_wait_try(threadpool_t *pool, void *arg);
void  uavs3e_threadpool_set_priority(threadpool_t *pool, int priority);

#endif /* __AVS2_THREADPOOL_H__ */
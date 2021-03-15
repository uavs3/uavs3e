#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef _WIN32
#include "win32thread.h"

#define uavs3e_sem_t        sem_t
#define uavs3e_sem_init     sem_init
#define uavs3e_sem_destroy  sem_destroy
#define uavs3e_sem_wait     sem_wait
#define uavs3e_sem_post     sem_post
#else

#include <semaphore.h>
#include <pthread.h>
#include <memory.h>
#include <stdlib.h>

#define avs3_pthread_t                pthread_t
#define avs3_pthread_create           pthread_create
#define avs3_pthread_join             pthread_join
#define avs3_pthread_mutex_t          pthread_mutex_t
#define avs3_pthread_mutex_init       pthread_mutex_init
#define avs3_pthread_mutex_destroy    pthread_mutex_destroy
#define avs3_pthread_mutex_lock       pthread_mutex_lock
#define avs3_pthread_mutex_unlock     pthread_mutex_unlock
#define avs3_pthread_cond_t           pthread_cond_t
#define avs3_pthread_cond_init        pthread_cond_init
#define avs3_pthread_cond_destroy     pthread_cond_destroy
#define avs3_pthread_cond_broadcast   pthread_cond_broadcast
#define avs3_pthread_cond_wait        pthread_cond_wait
#define avs3_pthread_attr_t           pthread_attr_t
#define avs3_pthread_attr_init        pthread_attr_init
#define avs3_pthread_attr_destroy     pthread_attr_destroy
#define avs3_pthread_num_processors_np pthread_num_processors_np
#define AVS3_PTHREAD_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

#if defined(__APPLE__)
#define uavs3e_sem_t sem_t*
int uavs3e_sem_init(uavs3e_sem_t* sem, int shared, const int inStartingCount);
#define uavs3e_sem_destroy(sem) sem_close(*(sem))
#define uavs3e_sem_wait(sem)    sem_wait(*(sem))
#define uavs3e_sem_post(sem)    sem_post(*(sem))
#else
#define uavs3e_sem_t        sem_t
#define uavs3e_sem_init     sem_init
#define uavs3e_sem_destroy  sem_destroy
#define uavs3e_sem_wait     sem_wait
#define uavs3e_sem_post     sem_post
#endif

#endif

#define safe_sem_post(x) if (x) { uavs3e_sem_post(x); }
#define safe_sem_wait(x) if (x) { uavs3e_sem_wait(x); }

/**
 * ===========================================================================
 * type defines
 * ===========================================================================
 */

typedef void*(*avs3_tfunc_a_t)(void *);
typedef volatile long atom_t;   // 32 bits, signed

typedef struct 
{
    void *(*func)(void *);
    void *arg;
    void *ret;
    int wait;
} avs3_threadpool_job_t;

typedef struct 
{
    avs3_threadpool_job_t   **list;
    int                     i_max_size;
    int                     i_size;
    avs3_pthread_mutex_t    mutex;
    avs3_pthread_cond_t     cv_fill;  /* event signaling that the list became fuller */
    avs3_pthread_cond_t     cv_empty; /* event signaling that the list became emptier */
} avs3_threadpool_job_list_t;

typedef struct avs3_threadpool_t
{
    int            exit;
    int            threads;
    avs3_pthread_t *thread_handle;
    void (*init_func)(void *);

    void           *init_arg;

    avs3_threadpool_job_list_t uninit; /* list of jobs that are awaiting use */
    avs3_threadpool_job_list_t run;    /* list of jobs that are queued for processing by the pool */
    avs3_threadpool_job_list_t done;   /* list of jobs that have finished processing */
} avs3_threadpool_t;

avs3_threadpool_t * avs3_threadpool_init(int threads, int nodes, void (*init_func)(void *), void *init_arg);
void   avs3_threadpool_run(avs3_threadpool_t *pool, void *(*func)(void *), void *arg, int wait_sign);
void * avs3_threadpool_wait(avs3_threadpool_t *pool, void *arg);
void   avs3_threadpool_delete(avs3_threadpool_t *pool);
void   avs3_threadpool_set_priority(avs3_threadpool_t *pool, int priority);
void   avs3_threadpool_set_affinity(avs3_threadpool_t *pool, void* mask);

#endif /* __AVS3_THREADPOOL_H__ */

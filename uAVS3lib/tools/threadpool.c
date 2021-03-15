#include <stdlib.h>
#include <assert.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <errno.h>
#include "threadpool.h"

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

/* ---------------------------------------------------------------------------
 * multi line macros
 */
#if !defined(_WIN32) || defined(__GNUC__)
#define __pragma(a)
#endif

#define MULTI_LINE_MACRO_BEGIN  do {
#define MULTI_LINE_MACRO_END \
    __pragma(warning(push))\
    __pragma(warning(disable:4127))\
    } while(0)\
    __pragma(warning(pop))

#define CHECKED_MALLOC(var, type, size)\
    MULTI_LINE_MACRO_BEGIN\
    (var) = (type)malloc(size);\
    if ((var) == NULL) {\
        goto fail;\
    }\
    MULTI_LINE_MACRO_END
#define CHECKED_MALLOCZERO(var, type, size)\
    MULTI_LINE_MACRO_BEGIN\
    CHECKED_MALLOC(var, type, size);\
    memset(var, 0, size);\
    MULTI_LINE_MACRO_END

#define XCHG(type, a, b)\
    MULTI_LINE_MACRO_BEGIN\
    type __tmp = (a); (a) = (b); (b) = __tmp;\
    MULTI_LINE_MACRO_END

#if defined(__APPLE__) 
int uavs3e_sem_count = 0;

int uavs3e_sem_init( uavs3e_sem_t* sem, int shared, const int init_count )
{
    char name[64] = "uavs3e_sem_";
    uavs3e_sem_t semaphore;
    sprintf(name + 11, "%d_%d", getpid(), uavs3e_sem_count++);
    semaphore = sem_open( name, O_CREAT|O_EXCL, 0644, init_count );
    
    if( semaphore == SEM_FAILED )
    {
        switch( errno )
        {
            case EEXIST:
                sem_close(semaphore);
                while(sem_unlink(name)){
                    sem_close(semaphore);
                }
                semaphore = sem_open(name, O_CREAT|O_EXCL, 0644, init_count);
                //printf( "Semaphore with name '%s' already exists.\n", name );
                *sem = semaphore;
                return 0;
                break;
                
            default:
                printf( "Unhandled error: %d.\n", errno );
                break;
        }
        
        return -1;
    }
    
    *sem = semaphore;
    return 0;
}
#endif

/**
 * ===========================================================================
 * function defines
 * ===========================================================================
 */

/* ---------------------------------------------------------------------------
 */
avs3_threadpool_job_t *avs3_frame_shift(avs3_threadpool_job_t **list)
{
    avs3_threadpool_job_t *job = list[0];
    int i;
    for (i = 0; list[i]; i++) {
        list[i] = list[i+1];
    }
    assert(job);
    return job;
}


/* ---------------------------------------------------------------------------
 */
void avs3_sync_frame_list_push(avs3_threadpool_job_list_t *slist, avs3_threadpool_job_t *job)
{
    avs3_pthread_mutex_lock(&slist->mutex);
    while (slist->i_size == slist->i_max_size) {
        avs3_pthread_cond_wait(&slist->cv_empty, &slist->mutex);
    }
    slist->list[slist->i_size++] = job;
    avs3_pthread_cond_broadcast(&slist->cv_fill);
    avs3_pthread_mutex_unlock(&slist->mutex);
}

/* ---------------------------------------------------------------------------
 */
avs3_threadpool_job_t *avs3_sync_frame_list_pop(avs3_threadpool_job_list_t *slist)
{
    avs3_threadpool_job_t *job;
    avs3_pthread_mutex_lock(&slist->mutex);
    while (!slist->i_size) {
        avs3_pthread_cond_wait(&slist->cv_fill, &slist->mutex);
    }
    job = slist->list[--slist->i_size];
    slist->list[slist->i_size] = NULL;
    avs3_pthread_cond_broadcast(&slist->cv_empty);
    avs3_pthread_mutex_unlock(&slist->mutex);
    return job;
}
/* ---------------------------------------------------------------------------
 */
int avs3_sync_frame_list_init(avs3_threadpool_job_list_t *slist, int max_size)
{
    if (max_size < 0) {
        return -1;
    }
    slist->i_max_size = max_size;
    slist->i_size = 0;
    CHECKED_MALLOCZERO(slist->list, avs3_threadpool_job_t **, (max_size + 1) * sizeof(avs3_threadpool_job_t *));
    if (avs3_pthread_mutex_init(&slist->mutex, NULL) ||
        avs3_pthread_cond_init(&slist->cv_fill, NULL) ||
        avs3_pthread_cond_init(&slist->cv_empty, NULL)) {
        return -1;
    }
    return 0;
fail:
    return -1;
}

static void avs3_threadpool_list_delete(avs3_threadpool_job_list_t *slist)
{
    int i;
    for (i = 0; slist->list[i]; i++) {
        free(slist->list[i]);
        slist->list[i] = NULL;
    }
    free(slist->list);
    avs3_pthread_mutex_destroy(&slist->mutex);
    avs3_pthread_cond_destroy(&slist->cv_fill);
    avs3_pthread_cond_destroy(&slist->cv_empty);
}

static void avs3_threadpool_thread(avs3_threadpool_t *pool)
{
    if (pool->init_func) {
        pool->init_func(pool->init_arg);
    }

    while (!pool->exit) {
        avs3_threadpool_job_t *job = NULL;

        avs3_pthread_mutex_lock(&pool->run.mutex);
        while (!pool->exit && !pool->run.i_size) {
            avs3_pthread_cond_wait(&pool->run.cv_fill, &pool->run.mutex);
        }
        if (pool->run.i_size) {
            job = (void *)avs3_frame_shift(pool->run.list);
            pool->run.i_size--;
        }
        
        avs3_pthread_mutex_unlock(&pool->run.mutex);
        if (!job) {
            continue;
        }
       
        job->ret = job->func(job->arg);   /* execute the function */
        if (job->wait) {
            avs3_sync_frame_list_push(&pool->done, (void *)job);
        } else {
            avs3_sync_frame_list_push(&pool->uninit, (void *)job);
        }
    }

    pthread_exit(0);
}

avs3_threadpool_t * avs3_threadpool_init(int threads, int nodes, void(*init_func)(void *), void *init_arg)
{
    int i;
    avs3_threadpool_t *pool;
  
    if (threads <= 0) {
        return NULL;
    }

    CHECKED_MALLOCZERO(pool, avs3_threadpool_t *, sizeof(avs3_threadpool_t));

    pool->init_func = init_func;
    pool->init_arg  = init_arg;
    pool->threads   = threads;

    CHECKED_MALLOC(pool->thread_handle, avs3_pthread_t *, pool->threads * sizeof(avs3_pthread_t));

    if (avs3_sync_frame_list_init(&pool->uninit, nodes) ||
        avs3_sync_frame_list_init(&pool->run   , nodes) ||
        avs3_sync_frame_list_init(&pool->done  , nodes)) {
        goto fail;
    }

    for (i = 0; i < nodes; i++) {
        avs3_threadpool_job_t *job;
        CHECKED_MALLOC(job, avs3_threadpool_job_t*, sizeof(avs3_threadpool_job_t));
        avs3_sync_frame_list_push(&pool->uninit, (void *)job);
    }
    for (i = 0; i < pool->threads; i++) {
        if (avs3_pthread_create(pool->thread_handle + i, NULL, (avs3_tfunc_a_t)avs3_threadpool_thread, pool)) {
            goto fail;
        }
    }

    return pool;
fail:
    return NULL;
}

void avs3_threadpool_set_priority(avs3_threadpool_t *pool, int priority)
{
    int i;
    for (i = 0; i < pool->threads; i++) {
#if defined(_WIN32) && !defined(__GNUC__)
        SetThreadPriority(pool->thread_handle[i].handle, priority);
#endif
    }
}

void avs3_threadpool_set_affinity(avs3_threadpool_t *pool, void* mask)
{
    int i;
    for (i = 0; i < pool->threads; i++) {
#if !defined(_WIN32) && defined(__GNUC__)
        pthread_setaffinity_np(pool->thread_handle[i], sizeof(cpu_set_t), (cpu_set_t*)mask);
#endif
    }
}

void avs3_threadpool_run(avs3_threadpool_t *pool, void *(*func)(void *), void *arg, int wait_sign)
{
    avs3_threadpool_job_t *job = (void *)avs3_sync_frame_list_pop(&pool->uninit);
    job->func = func;
    job->arg  = arg;
    job->wait = wait_sign;
    avs3_sync_frame_list_push(&pool->run, (void *)job);
}

void *avs3_threadpool_wait(avs3_threadpool_t *pool, void *arg)
{
    avs3_threadpool_job_t *job = NULL;
    int i;
    void *ret;

    avs3_pthread_mutex_lock(&pool->done.mutex);
    while (!job) {
        for (i = 0; i < pool->done.i_size; i++) {
            avs3_threadpool_job_t *t = (void *)pool->done.list[i];
            if (t->arg == arg) {
                job = (void *)avs3_frame_shift(pool->done.list + i);
                pool->done.i_size--;
                break;
            }
        }
        if (!job) {
            avs3_pthread_cond_wait(&pool->done.cv_fill, &pool->done.mutex);
        }
    }
    avs3_pthread_mutex_unlock(&pool->done.mutex);

    ret = job->ret;
    avs3_sync_frame_list_push(&pool->uninit, (void *)job);
    return ret;
}


void avs3_threadpool_delete(avs3_threadpool_t *pool)
{
    int i;

    avs3_pthread_mutex_lock(&pool->run.mutex);
    pool->exit = 1;
    avs3_pthread_cond_broadcast(&pool->run.cv_fill);
    avs3_pthread_mutex_unlock(&pool->run.mutex);
    for (i = 0; i < pool->threads; i++) {
        avs3_pthread_join(pool->thread_handle[i], NULL);
    }

    avs3_threadpool_list_delete(&pool->uninit);
    avs3_threadpool_list_delete(&pool->run);
    avs3_threadpool_list_delete(&pool->done);

    free(pool->thread_handle);
    free(pool);
}

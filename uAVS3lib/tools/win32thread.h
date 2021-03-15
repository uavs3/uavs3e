#ifndef _WIN32THREAD_H__
#define _WIN32THREAD_H__

#include <windows.h>

/* the following macro is used within cavs */
#undef ERROR

typedef struct {
    void *handle;
    void *(*func)(void *arg);
    void *arg;
    void *ret;
} avs3_pthread_t;
#define avs3_pthread_attr_t int

/* the conditional variable api for windows 6.0+ uses critical sections and not mutexes */
typedef CRITICAL_SECTION avs3_pthread_mutex_t;
#define AVS3_PTHREAD_MUTEX_INITIALIZER {0}
#define avs3_pthread_mutexattr_t int
#define pthread_exit(a)
/* This is the CONDITIONAL_VARIABLE typedef for using Window's native conditional variables on kernels 6.0+.
 * MinGW does not currently have this typedef. */
typedef struct {
    void *ptr;
} avs3_pthread_cond_t;
#define avs3_pthread_condattr_t int

int avs3_pthread_create(avs3_pthread_t *thread, const avs3_pthread_attr_t *attr,
                        void *(*start_routine)(void *), void *arg);
int avs3_pthread_join(avs3_pthread_t thread, void **value_ptr);

int avs3_pthread_mutex_init(avs3_pthread_mutex_t *mutex, const avs3_pthread_mutexattr_t *attr);
int avs3_pthread_mutex_destroy(avs3_pthread_mutex_t *mutex);
int avs3_pthread_mutex_lock(avs3_pthread_mutex_t *mutex);
int avs3_pthread_mutex_unlock(avs3_pthread_mutex_t *mutex);

int avs3_pthread_cond_init(avs3_pthread_cond_t *cond, const avs3_pthread_condattr_t *attr);
int avs3_pthread_cond_destroy(avs3_pthread_cond_t *cond);
int avs3_pthread_cond_broadcast(avs3_pthread_cond_t *cond);
int avs3_pthread_cond_wait(avs3_pthread_cond_t *cond, avs3_pthread_mutex_t *mutex);
int avs3_pthread_cond_signal(avs3_pthread_cond_t *cond);

#define avs3_pthread_attr_init(a) 0
#define avs3_pthread_attr_destroy(a) 0

int  avs3_win32_threading_init(void);
void avs3_win32_threading_destroy(void);

int avs3_pthread_num_processors_np(void);


// ==========================================================================
// semaphore
// ==========================================================================

typedef void *sem_t;   // semaphore

static __inline int sem_init(sem_t *sem, int pshared, unsigned int value) 
{
    pshared = 0; // un-used
    *sem = CreateSemaphore(NULL, value, (1 << 30), NULL);
    return (*sem == NULL) ? 0 : -1;
}

static __inline int sem_wait(sem_t *sem) 
{
    return WaitForSingleObject(*sem, INFINITE);
}

static __inline int sem_post(sem_t *sem) 
{
    return ReleaseSemaphore(*sem, 1, NULL);
}

static __inline int sem_destroy(sem_t *sem) 
{
    CloseHandle(*sem);
    *sem = NULL;
    return 0;
}

#endif

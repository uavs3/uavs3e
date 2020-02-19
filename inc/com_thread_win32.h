#ifndef _WIN32THREAD_H__
#define _WIN32THREAD_H__

#include <windows.h>

/* the following macro is used within cavs */
#undef ERROR

typedef struct uavs3e_pthread_t {
    void *handle;
    void *(*func)(void *arg);
    void *arg;
    void *ret;
} uavs3e_pthread_t;
#define uavs3e_pthread_attr_t int

/* the conditional variable api for windows 6.0+ uses critical sections and not mutexes */
typedef CRITICAL_SECTION uavs3e_pthread_mutex_t;

#define uavs3e_pthread_mutexattr_t int
#define pthread_exit(a)
/* This is the CONDITIONAL_VARIABLE typedef for using Window's native conditional variables on kernels 6.0+.
 * MinGW does not currently have this typedef. */
typedef struct uavs3e_pthread_cond_t {
    void *ptr;
} uavs3e_pthread_cond_t;
#define uavs3e_pthread_condattr_t int

int uavs3e_pthread_create(uavs3e_pthread_t *thread, const uavs3e_pthread_attr_t *attr,
                          void *(*start_routine)(void *), void *arg);
int uavs3e_pthread_join(uavs3e_pthread_t thread, void **value_ptr);

int uavs3e_pthread_mutex_init(uavs3e_pthread_mutex_t *mutex, const uavs3e_pthread_mutexattr_t *attr);
int uavs3e_pthread_mutex_destroy(uavs3e_pthread_mutex_t *mutex);
int uavs3e_pthread_mutex_lock(uavs3e_pthread_mutex_t *mutex);
int uavs3e_pthread_mutex_unlock(uavs3e_pthread_mutex_t *mutex);

int uavs3e_pthread_cond_init(uavs3e_pthread_cond_t *cond, const uavs3e_pthread_condattr_t *attr);
int uavs3e_pthread_cond_destroy(uavs3e_pthread_cond_t *cond);
int uavs3e_pthread_cond_broadcast(uavs3e_pthread_cond_t *cond);
int uavs3e_pthread_cond_wait(uavs3e_pthread_cond_t *cond, uavs3e_pthread_mutex_t *mutex);
int uavs3e_pthread_cond_signal(uavs3e_pthread_cond_t *cond);

#define uavs3e_pthread_attr_init(a) 0
#define uavs3e_pthread_attr_destroy(a) 0

int  uavs3_win32_threading_init(void);
void uavs3_win32_threading_destroy(void);

int uavs3e_pthread_num_processors_np(void);

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
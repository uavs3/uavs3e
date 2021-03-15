#ifdef _WIN32

#include "win32thread.h"
#include <process.h>


/**
 * ===========================================================================
 * type defines
 * ===========================================================================
 */

/* number of times to spin a thread about to block on a locked mutex before retrying and sleeping if still locked */
#define AVS3_SPIN_COUNT 0

#pragma warning(disable: 4100) 

/* GROUP_AFFINITY struct */
typedef struct {
    ULONG_PTR mask; // KAFFINITY = ULONG_PTR
    USHORT group;
    USHORT reserved[3];
} avs3_group_affinity_t;

typedef void (WINAPI *cond_func_t)(avs3_pthread_cond_t *cond);
typedef BOOL (WINAPI *cond_wait_t)(avs3_pthread_cond_t *cond, avs3_pthread_mutex_t *mutex, DWORD milliseconds);

typedef struct {
    /* global mutex for replacing MUTEX_INITIALIZER instances */
    avs3_pthread_mutex_t static_mutex;

    /* function pointers to conditional variable API on windows 6.0+ kernels */
    cond_func_t cond_broadcast;
    cond_func_t cond_init;
    cond_func_t cond_signal;
    cond_wait_t cond_wait;
} avs3_win32thread_control_t;

static avs3_win32thread_control_t thread_control;


/**
 * ===========================================================================
 * function defines
 * ===========================================================================
 */

/* _beginthreadex requires that the start routine is __stdcall */
static unsigned __stdcall avs3_win32thread_worker(void *arg)
{
    avs3_pthread_t *h = arg;
    h->ret = h->func(h->arg);
    return 0; 
}

int avs3_pthread_create(avs3_pthread_t *thread, const avs3_pthread_attr_t *attr,
                        void *(*start_routine)(void *), void *arg)
{
    thread->func   = start_routine;
    thread->arg    = arg;
    thread->handle = (void *)_beginthreadex(NULL, 0, avs3_win32thread_worker, thread, 0, NULL);
    return !thread->handle;
}

int avs3_pthread_join(avs3_pthread_t thread, void **value_ptr)
{
    DWORD ret = WaitForSingleObject(thread.handle, INFINITE);
    if (ret != WAIT_OBJECT_0) {
        return -1;
    }
    if (value_ptr) {
        *value_ptr = thread.ret;
    }
    CloseHandle(thread.handle);
    return 0;
}

int avs3_pthread_mutex_init(avs3_pthread_mutex_t *mutex, const avs3_pthread_mutexattr_t *attr)
{
    return !InitializeCriticalSectionAndSpinCount(mutex, AVS3_SPIN_COUNT);
}

int avs3_pthread_mutex_destroy(avs3_pthread_mutex_t *mutex)
{
    DeleteCriticalSection(mutex);
    return 0;
}

int avs3_pthread_mutex_lock(avs3_pthread_mutex_t *mutex)
{
    static avs3_pthread_mutex_t init = AVS3_PTHREAD_MUTEX_INITIALIZER;
    if (!memcmp(mutex, &init, sizeof(avs3_pthread_mutex_t))) {
        *mutex = thread_control.static_mutex;
    }
    EnterCriticalSection(mutex);
    return 0;
}

int avs3_pthread_mutex_unlock(avs3_pthread_mutex_t *mutex)
{
    LeaveCriticalSection(mutex);
    return 0;
}

/* for pre-Windows 6.0 platforms we need to define and use our own condition variable and api */
typedef struct {
    avs3_pthread_mutex_t mtx_broadcast;
    avs3_pthread_mutex_t mtx_waiter_count;
    int waiter_count;
    HANDLE semaphore;
    HANDLE waiters_done;
    int is_broadcast;
} avs3_win32_cond_t;

int avs3_pthread_cond_init(avs3_pthread_cond_t *cond, const avs3_pthread_condattr_t *attr)
{
    avs3_win32_cond_t *win32_cond;
    if (thread_control.cond_init) {
        thread_control.cond_init(cond);
        return 0;
    }

    /* non native condition variables */
    win32_cond = calloc(1, sizeof(avs3_win32_cond_t));
    if (!win32_cond) {
        return -1;
    }
    cond->ptr = win32_cond;
    win32_cond->semaphore = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
    if (!win32_cond->semaphore) {
        return -1;
    }

    if (avs3_pthread_mutex_init(&win32_cond->mtx_waiter_count, NULL)) {
        return -1;
    }
    if (avs3_pthread_mutex_init(&win32_cond->mtx_broadcast, NULL)) {
        return -1;
    }

    win32_cond->waiters_done = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!win32_cond->waiters_done) {
        return -1;
    }

    return 0;
}

int avs3_pthread_cond_destroy(avs3_pthread_cond_t *cond)
{
    avs3_win32_cond_t *win32_cond;
    /* native condition variables do not destroy */
    if (thread_control.cond_init) {
        return 0;
    }

    /* non native condition variables */
    win32_cond = cond->ptr;
    CloseHandle(win32_cond->semaphore);
    CloseHandle(win32_cond->waiters_done);
    avs3_pthread_mutex_destroy(&win32_cond->mtx_broadcast);
    avs3_pthread_mutex_destroy(&win32_cond->mtx_waiter_count);
    free(win32_cond);

    return 0;
}

int avs3_pthread_cond_broadcast(avs3_pthread_cond_t *cond)
{
    avs3_win32_cond_t *win32_cond;
    int have_waiter = 0;
    if (thread_control.cond_broadcast) {
        thread_control.cond_broadcast(cond);
        return 0;
    }

    /* non native condition variables */
    win32_cond = cond->ptr;
    avs3_pthread_mutex_lock(&win32_cond->mtx_broadcast);
    avs3_pthread_mutex_lock(&win32_cond->mtx_waiter_count);

    if (win32_cond->waiter_count) {
        win32_cond->is_broadcast = 1;
        have_waiter = 1;
    }

    if (have_waiter) {
        ReleaseSemaphore(win32_cond->semaphore, win32_cond->waiter_count, NULL);
        avs3_pthread_mutex_unlock(&win32_cond->mtx_waiter_count);
        WaitForSingleObject(win32_cond->waiters_done, INFINITE);
        win32_cond->is_broadcast = 0;
    } else {
        avs3_pthread_mutex_unlock(&win32_cond->mtx_waiter_count);
    }
    return avs3_pthread_mutex_unlock(&win32_cond->mtx_broadcast);
}

int avs3_pthread_cond_signal(avs3_pthread_cond_t *cond)
{
    avs3_win32_cond_t *win32_cond;
    int have_waiter;
    if (thread_control.cond_signal) {
        thread_control.cond_signal(cond);
        return 0;
    }

    /* non-native condition variables */
    win32_cond = cond->ptr;
    avs3_pthread_mutex_lock(&win32_cond->mtx_waiter_count);
    have_waiter = win32_cond->waiter_count;
    avs3_pthread_mutex_unlock(&win32_cond->mtx_waiter_count);

    if (have_waiter) {
        ReleaseSemaphore(win32_cond->semaphore, 1, NULL);
    }
    return 0;
}

int avs3_pthread_cond_wait(avs3_pthread_cond_t *cond, avs3_pthread_mutex_t *mutex)
{
    avs3_win32_cond_t *win32_cond;
    int last_waiter;
    if (thread_control.cond_wait) {
        return !thread_control.cond_wait(cond, mutex, INFINITE);
    }

    /* non native condition variables */
    win32_cond = cond->ptr;

    avs3_pthread_mutex_lock(&win32_cond->mtx_broadcast);
    avs3_pthread_mutex_unlock(&win32_cond->mtx_broadcast);

    avs3_pthread_mutex_lock(&win32_cond->mtx_waiter_count);
    win32_cond->waiter_count++;
    avs3_pthread_mutex_unlock(&win32_cond->mtx_waiter_count);

    // unlock the external mutex
    avs3_pthread_mutex_unlock(mutex);
    WaitForSingleObject(win32_cond->semaphore, INFINITE);

    avs3_pthread_mutex_lock(&win32_cond->mtx_waiter_count);
    win32_cond->waiter_count--;
    last_waiter = !win32_cond->waiter_count && win32_cond->is_broadcast;
    avs3_pthread_mutex_unlock(&win32_cond->mtx_waiter_count);

    if (last_waiter) {
        SetEvent(win32_cond->waiters_done);
    }

    // lock the external mutex
    return avs3_pthread_mutex_lock(mutex);
}

int avs3_win32_threading_init(void)
{
    /* find function pointers to API functions, if they exist */
    HMODULE kernel_dll = GetModuleHandle(TEXT("kernel32"));
    thread_control.cond_init = (cond_func_t)GetProcAddress(kernel_dll, "InitializeConditionVariable");
    if (thread_control.cond_init) {
        /* we're on a windows 6.0+ kernel, acquire the rest of the functions */
        thread_control.cond_broadcast = (cond_func_t)GetProcAddress(kernel_dll, "WakeAllConditionVariable");
        thread_control.cond_signal = (cond_func_t)GetProcAddress(kernel_dll, "WakeConditionVariable");
        thread_control.cond_wait = (cond_wait_t)GetProcAddress(kernel_dll, "SleepConditionVariableCS");
    }
    return avs3_pthread_mutex_init(&thread_control.static_mutex, NULL);
}

void avs3_win32_threading_destroy(void)
{
    avs3_pthread_mutex_destroy(&thread_control.static_mutex);
    memset(&thread_control, 0, sizeof(avs3_win32thread_control_t));
}

int avs3_pthread_num_processors_np()
{
    DWORD_PTR system_cpus, process_cpus = 0;
    int cpus = 0;
    DWORD_PTR bit;

    /* GetProcessAffinityMask returns affinities of 0 when the process has threads in multiple processor groups.
     * On platforms that support processor grouping, use GetThreadGroupAffinity to get the current thread's affinity instead. */
#if ARCH_X86_64
    /* find function pointers to API functions specific to x86_64 platforms, if they exist */
    HANDLE kernel_dll = GetModuleHandle(TEXT("kernel32.dll"));
    BOOL (*get_thread_affinity)(HANDLE thread, avs3_group_affinity_t * group_affinity) = (void *)GetProcAddress(kernel_dll, "GetThreadGroupAffinity");
    if (get_thread_affinity) {
        /* running on a platform that supports >64 logical cpus */
        avs3_group_affinity_t thread_affinity;
        if (get_thread_affinity(GetCurrentThread(), &thread_affinity)) {
            process_cpus = thread_affinity.mask;
        }
    }
#endif
    if (!process_cpus) {
        GetProcessAffinityMask(GetCurrentProcess(), &process_cpus, &system_cpus);
    }
    for (bit = 1; bit; bit <<= 1) {
        cpus += !!(process_cpus & bit);
    }

    return cpus ? cpus : 1;
}

#endif
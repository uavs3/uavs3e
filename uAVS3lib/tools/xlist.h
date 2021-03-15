#include "threadpool.h"

typedef struct node_t node_t;

struct node_t {
    node_t *next;
};

/* ---------------------------------------------------------------------------
 * xlist_t
 */
typedef struct xlist_t {
    node_t               *p_list_head;         /* pointer to head of node list */
    node_t               *p_list_tail;         /* pointer to tail of node list */
    int                   i_node_num;           /* node number in the list */
    uavs3e_sem_t          list_sem;             /* semaphore */
    avs3_pthread_mutex_t  list_lock;  /* list lock */
} xlist_t;

int   xl_init(xlist_t *const xlist);
void  xl_destroy(xlist_t *const xlist);
void  xl_append(xlist_t *const xlist, void *node);
void *xl_remove_head(xlist_t *const xlist, const int wait);

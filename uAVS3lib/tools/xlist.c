#include "xlist.h"

/**
 * ===========================================================================
 * xlist
 * ===========================================================================
 */

/**
 * ---------------------------------------------------------------------------
 * Function   : initialize a list
 * Parameters :
 *      [in ] : xlist    - pointer to the node list
 *      [out] : none
 * Return     : zero for success, otherwise failed
 * Remarks    : also create 2 synchronous objects, but without any node
 * ---------------------------------------------------------------------------
 */
int xl_init(xlist_t *const xlist)
{
    /* set list empty */
    xlist->p_list_head = NULL;
    xlist->p_list_tail = NULL;

    /* set node number */
    xlist->i_node_num = 0;

    /* create semaphore */
    uavs3e_sem_init(&xlist->list_sem, 0, 0);

    /* init list lock */
    avs3_pthread_mutex_init(&xlist->list_lock, NULL);

    return 0;
}

/**
 * ---------------------------------------------------------------------------
 * Function   : destroy a list
 * Parameters :
 *      [in ] : xlist - the list, pointer to struct xlist_t
 *      [out] : none
 * Return     : none
 * ---------------------------------------------------------------------------
 */
void xl_destroy(xlist_t *const xlist)
{
    /* destroy the spin lock */
    avs3_pthread_mutex_destroy(&xlist->list_lock);

    /* close handles */
    uavs3e_sem_destroy(&(xlist->list_sem));

    /* clear */
    memset(xlist, 0, sizeof(xlist_t));
}

/**
 * ---------------------------------------------------------------------------
 * Function   : append data to the tail of a list
 * Parameters :
 *      [in ] : xlist - the node list, pointer to struct xlist
 *            : data  - the data to append
 *      [out] : none
 * Return     : none
 * ---------------------------------------------------------------------------
 */
void xl_append(xlist_t *const xlist, void *node)
{
    node_t *new_node = (node_t *)node;

    new_node->next = NULL;            /* set NULL */

    /* append this node */
    avs3_pthread_mutex_lock(&xlist->list_lock);

    if (xlist->p_list_tail != NULL) {
        /* append this node at tail */
        xlist->p_list_tail->next = new_node;
    } else {
        xlist->p_list_head = new_node;
    }

    xlist->p_list_tail = new_node;    /* point to the tail node */
    xlist->i_node_num++;              /* increase the node number */
    avs3_pthread_mutex_unlock(&xlist->list_lock);

    /* all is done, release a semaphore */
    uavs3e_sem_post(&xlist->list_sem);
}

/**
 * ---------------------------------------------------------------------------
 * Function   : remove one node from the list's head position
 * Parameters :
 *      [in ] : xlist - the node list, pointer to struct xlist_t
 *            : wait  - wait the semaphore?
 *      [out] : none
 * Return     : node pointer for success, or NULL for failure
 * ---------------------------------------------------------------------------
 */
void *xl_remove_head(xlist_t *const xlist, const int wait)
{
    node_t *node = NULL;

    if (wait) {
        uavs3e_sem_wait(&xlist->list_sem);
    }

    /* remove the header node */
    if (xlist->i_node_num > 0) {
        avs3_pthread_mutex_lock(&xlist->list_lock);
        node = xlist->p_list_head;    /* point to the header node */

        /* modify the list */
        xlist->p_list_head = node->next;

        if (xlist->p_list_head == NULL) {
            /* there are no any node in this list, reset the tail pointer */
            xlist->p_list_tail = NULL;
        }

        xlist->i_node_num--;          /* decrease the number */
        avs3_pthread_mutex_unlock(&xlist->list_lock);
    }

    return node;
}

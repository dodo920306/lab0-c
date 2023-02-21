#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "report.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    // allocate memory for a list_head pointer called head.
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    // Check if malloc() failed.
    if (head) {
        INIT_LIST_HEAD(head);
        return head;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    // check if list valid and not empty
    if (!l) {
        return;
    }

    // free element
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        q_release_element(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    // check if head is valid.
    if (!head)
        return false;

    // allocate an element
    element_t *new = malloc(sizeof(element_t));
    // check if allocation is successful.
    if (!new)
        return false;

    size_t len = strlen(s) + 1;
    // allocate a string for the element->value
    new->value = malloc(len);
    if (!new->value) {
        free(new);
        return false;
    }
    // copy the string into the element
    memcpy(new->value, s, len);

    // insert this at head of queue
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    // check if head is valid.
    if (!head)
        return false;

    // allocate an element
    element_t *new = malloc(sizeof(element_t));
    // check if allocation is successful.
    if (!new)
        return false;

    size_t len = strlen(s) + 1;
    // allocate a string for the element->value
    new->value = malloc(len);
    if (!new->value) {
        free(new);
        return false;
    }
    // copy the string into the element
    memcpy(new->value, s, len);

    // insert this at tail of queue
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // check if head is valid.
    if (!head || list_empty(head))
        return NULL;

    element_t *entry = list_first_entry(head, element_t, list);
    list_del(&entry->list);

    if (sp && bufsize) {
        memcpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
        return entry;
    }

    return NULL;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // check if head is valid.
    if (!head || list_empty(head))
        return NULL;

    element_t *entry = list_last_entry(head, element_t, list);
    list_del(&entry->list);

    if (sp && bufsize) {
        memcpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
        return entry;
    }

    return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *rabbit = head->next, *turtle = head->next;
    while (rabbit != head && rabbit->next != head) {
        rabbit = rabbit->next->next;
        turtle = turtle->next;
    }
    list_del(turtle);
    q_release_element(list_entry(turtle, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    struct list_head *node, *safe;
    element_t *el;
    list_for_each_safe (node, safe, head) {
        if (safe == head)
            break;
        if (!strcmp(list_entry(node, element_t, list)->value,
                    list_entry(safe, element_t, list)->value)) {
            int len = strlen(list_entry(node, element_t, list)->value) + 1;
            char *dup = (char *) malloc(len);
            strncpy(dup, list_entry(node, element_t, list)->value, len);
            while (safe != head &&
                   !strcmp(dup, list_entry(safe, element_t, list)->value)) {
                el = list_entry(node, element_t, list);
                list_del(node);
                q_release_element(el);
                node = safe;
                safe = safe->next;
            }
            list_del(node);
            q_release_element(list_entry(node, element_t, list));
            free(dup);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || !head->next || !head->next->next)
        return;
    struct list_head *x, *y, *z = NULL;
    x = head->next;
    y = head->next->next;
    if (head->next->next->next != NULL) {
        z = head->next->next->next;
    }
    head->next = y;
    y->prev = head;
    while (z != head && z->next != head) {
        y->next = x;
        x->prev = y;
        x->next = z->next;
        z->next->prev = x;

        x = z;
        y = x->next;
        z = y->next;
    }
    y->next = x;
    x->prev = y;
    x->next = z;
    z->prev = x;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (list_is_singular(head))
        return;
    struct list_head *tmp;
    for (struct list_head *node = head->next, *safe; node != head;
         node = safe) {
        safe = node->next;
        tmp = node->prev;
        node->prev = node->next;
        node->next = tmp;
    }
    tmp = head->prev;
    head->prev = head->next;
    head->next = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    struct list_head *tmp, *node = head->next, *safe = node->next, *last = head;
    int len = q_size(head);
    for (int j = 0; j < (len / k); j++) {
        for (int i = 0; i < k; i++) {
            safe = node->next;
            tmp = node->prev;
            node->prev = node->next;
            node->next = tmp;
            if (i < k - 1)
                node = safe;
        }
        tmp = last->next;
        last->next = node;
        node->prev = last;
        tmp->next = safe;
        safe->prev = tmp;
        last = tmp;
        node = safe;
    }
}

struct list_head *q_mergeTwo(struct list_head *q1, struct list_head *q2)
{
    struct list_head *prehead = NULL, **indirect = &prehead;
    while (q1 && q2) {
        element_t *e1 = list_entry(q1, element_t, list);
        element_t *e2 = list_entry(q2, element_t, list);
        if (strcmp(e1->value, e2->value) <= 0) {
            *indirect = q1;
            q1 = q1->next;
        } else {
            *indirect = q2;
            q2 = q2->next;
        }
        indirect = &(*indirect)->next;
    }
    *indirect = (struct list_head *) ((uintptr_t) q1 | (uintptr_t) q2);
    return prehead;
}

struct list_head *q_mergeSort(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *fast = head->next;
    struct list_head *slow = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // slow is in the middle of the queue when the loop broke.
    fast = slow->next;  // assign the latter half of the queue to fast
    slow->next = NULL;  // split the queue into two

    head = q_mergeSort(head);
    fast = q_mergeSort(fast);

    return q_mergeTwo(head, fast);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *tail = head->prev;
    head->prev = NULL;
    tail->next = NULL;  // decyclize
    head->next = q_mergeSort(head->next);
    // retake the tail now
    struct list_head *node = head->next;
    while (node) {
        tail = node;
        node = node->next;
    }
    tail->next = head;
    head->prev = tail;

    struct list_head *it = head, *cur = head->next;
    while (cur != head) {
        if (!cur)
            return;
        cur->prev = it;
        cur = cur->next;
        it = it->next;
    }
    cur->prev = it;

    return;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        if (safe == head)
            break;
        if (safe != head &&
            strcmp(list_entry(node, element_t, list)->value,
                   list_entry(safe, element_t, list)->value) >= 0) {
            int len = strlen(list_entry(node, element_t, list)->value) + 1;
            char *dup = (char *) malloc(len);
            strncpy(dup, list_entry(node, element_t, list)->value, len);
            while (safe != head &&
                   strcmp(dup, list_entry(safe, element_t, list)->value) >= 0) {
                safe = safe->next;
            }
            if (safe != head) {
                while (node != safe) {
                    struct list_head *tmp = node;
                    element_t *e = list_entry(tmp, element_t, list);
                    node = node->next;
                    list_del(tmp);
                    q_release_element(e);
                }
            }
            free(dup);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    queue_contex_t *entry,
        *safe = list_entry(head->next, queue_contex_t, chain);
    // element_t *el;
    struct list_head *node, *it;

    for (entry = list_entry(head->next->next, queue_contex_t, chain);
         &entry->chain != head;
         entry = list_entry(entry->chain.next, queue_contex_t, chain)) {
        list_for_each_safe (node, it, entry->q) {
            // report(1, "%s", list_entry(node, element_t, list)->value);
            list_add_tail(node, safe->q);
        }
        list_del_init(entry->q);
    }

    q_sort(safe->q);

    it = safe->q;
    struct list_head *cur = safe->q->next;
    while (cur != safe->q) {
        if (!cur)
            return false;
        cur->prev = it;
        cur = cur->next;
        it = it->next;
    }
    cur->prev = it;
    cur = safe->q->prev;
    while (cur != safe->q) {
        if (!cur)
            return false;
        cur = cur->prev;
    }

    return q_size(safe->q);
}

// /* Merge all the queues into one sorted queue, which is in ascending order */
// int q_merge(struct list_head *head)
// {
//     // https://leetcode.com/problems/merge-k-sorted-lists/
//     queue_contex_t *entry = list_entry(head->next, queue_contex_t, chain),
//     *safe = list_entry(head->next->next, queue_contex_t, chain); element_t
//     *el; while (!list_is_singular(head)) {
//         while (true) {
//             struct list_head *tail = entry->q->prev;
//             entry->q->prev = NULL;
//             tail->next = NULL;

//             tail = safe->q->prev;
//             safe->q->prev = NULL;
//             tail->next = NULL;

//             safe->q->next = q_mergeTwo(entry->q->next, safe->q->next);

//             struct list_head *node = safe->q->next;
//             while (node) {
//                 tail = node;
//                 node = node->next;
//             }
//             tail->next = safe->q;
//             safe->q->prev = tail;

//             struct list_head *it = safe->q, *cur = safe->q->next;
//             while (cur != safe->q) {
//                 if (!cur)
//                     return 0;
//                 cur->prev = it;
//                 cur = cur->next;
//                 it = it->next;
//             }
//             cur->prev = it;

//             list_del_init(&entry->chain);
//             report(1, "gg");
//             list_for_each_entry(el, safe->q, list) {
//                 report(1, "%s", el->value);
//             }
//             if (safe->chain.next != head && safe->chain.next->next != head) {
//                 entry = list_entry(safe->chain.next, queue_contex_t, chain);
//                 safe = list_entry(entry->chain.next, queue_contex_t, chain);
//             }
//             else {
//                 break;
//             }
//         }
//     }
//     report(1, "gg");
//     q_sort(safe->q);

//     return q_size(safe->q);
// }

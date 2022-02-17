#include <stdlib.h>
#include <stdio.h>
#include "list.h"

#ifdef __cplusplus
extern "C" {
    #include "zmalloc.h"
}
#endif

/* Create a new list. The created list can be freed with
 * listRelease(), but private value of every node need to be freed
 * by the user before to call listRelease(), or by setting a free method using
 * listSetFreeMethod.
 *
 * On error, NULL is returned. Otherwise the pointer to the new list. */
List *listCreate() {
    List *list = (List *)zmalloc(sizeof(List));

    if (list == NULL) return NULL;
    list->len = 0;
    list->head = list->tail = NULL;
    list->dup = NULL;
    list->free = NULL;
    list->match = NULL;
    return list;
}

/* Remove all the elements from the list without destroying the list itself. */
void listEmpty(List *list) {
    listNode *current, *next;

    current = list->head;
    while (current) {
        next = current->next;
        if (list->free) list->free(current->value);
        zfree(current);
        current = next;
    }
    list->head = list->tail = NULL;
    list->len = 0;
}

/* Free the whole list. */
void listRelease(List *list) {
    listEmpty(list);
    zfree(list);
}

bool listInsertNode(List *list, listNode *old_node, void *value, int after) {
    if (list == NULL || value == NULL) return false;
    listNode *node = (listNode *)zmalloc(sizeof(listNode));
    if (node == NULL) return false;

    node->value = value;
    if (old_node == NULL) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else if (after) {
        node->prev = old_node;
        node->next = old_node->next;
        if (list->tail == old_node) {
            list->tail = node;
        }
    } else {
        node->next = old_node;
        node->prev = old_node->prev;
        if (list->head == old_node) {
            list->head = node;
        }
    }
    if (node->prev != NULL) node->prev->next = node;
    if (node->next != NULL) node->next->prev = node;
    list->len++;
    return true;
}

/* Remove the specified node from the specified list.
 * It's up to the caller to free the private value of the node.
 *
 * This function can't fail. */
void listDelNode(List *list, listNode *node) {
    if (list == NULL || node == NULL) return;
    if (node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;
    if (list->free) list->free(node->value);
    zfree(node);
    list->len--;
}

/* Returns a list iterator 'iter'. After the initialization every
 * call to listNext() will return the next element of the list.
 *
 * This function can't fail. */
listIter *listGetIterator(List *list, int direction) {
    if (list == NULL) return NULL;
    listIter *iter = (listIter *)zmalloc(sizeof(listIter));
    if (iter == NULL) return NULL;
    if (direction == AL_START_HEAD)
        iter->next = list->head;
    else
        iter->next = list->tail;
    iter->direction = direction;
    return iter;
}

/* Release the iterator memory */
void listReleaseIterator(listIter *iter) {
    if (iter) zfree(iter);
}

/* Create an iterator in the list private iterator structure */
void listRewind(List *list, listIter *li) {
    if (list && li) {
        li->next = list->head;
        li->direction = AL_START_HEAD;
    }
}

void listRewindTail(List *list, listIter *li) {
    if (list && li) {
        li->next = list->tail;
        li->direction = AL_START_TAIL;
    }
}

/* Return the next element of an iterator.
 * It's valid to remove the currently returned element using
 * listDelNode(), but not to remove other elements.
 *
 * The function returns a pointer to the next element of the list,
 * or NULL if there are no more elements, so the classical usage
 * pattern is:
 *
 * iter = listGetIterator(list,<direction>);
 * while ((node = listNext(iter)) != NULL) {
 *     doSomethingWith(listNodeValue(node));
 * }
 *
 * */
listNode *listNext(listIter *iter) {
    if (iter == NULL) return NULL;
    
    listNode *current = iter->next;
    if (current != NULL) {
        if (iter->direction == AL_START_HEAD)
            iter->next = current->next;
        else
            iter->next = current->prev;
    }
    return current;
}

/* Duplicate the whole list. On out of memory NULL is returned.
 * On success a copy of the original list is returned.
 *
 * The 'Dup' method set with listSetDupMethod() function is used
 * to copy the node value. Otherwise the same pointer value of
 * the original node is used as value of the copied node.
 *
 * The original list both on success or error is never modified. */
List *listDup(List *orig) {
    List *copy = listCreate();
    if (copy == NULL) return NULL;
    listIter *iter = listGetIterator(orig, AL_START_HEAD);
    if (iter == NULL) return NULL;
    
    copy->dup = orig->dup;
    copy->free = orig->free;
    copy->match = orig->match;
    listNode *node;
    while ((node = listNext(iter)) != NULL) {
        void *value;

        if (copy->dup) {
            value = copy->dup(node->value);
            if (value == NULL) {
                listRelease(copy);
                return NULL;
            }
        } else
            value = node->value;
        if (listAddNodeTail(copy, value) == false) {
            listRelease(copy);
            return NULL;
        }
    }
    return copy;
}

/* Search the list for a node matching a given key.
 * The match is performed using the 'match' method
 * set with listSetMatchMethod(). If no 'match' method
 * is set, the 'value' pointer of every node is directly
 * compared with the 'key' pointer.
 *
 * On success the first matching node pointer is returned
 * (search starts from head). If no matching node exists
 * NULL is returned. */
listNode *listSearchKey(List *list, void *key) {
    if (list == NULL || key == NULL) return NULL;

    listIter *iter = listGetIterator(list, AL_START_HEAD);
    if (iter == NULL) return NULL;
    
    listNode *node;
    while ((node = listNext(iter)) != NULL) {
        if (list->match) {
            if (list->match(node->value, key))
                return node;
        } else if (key == node->value)
            return node;
    }
    return NULL;
}

/* Return the element at the specified zero-based index
 * where 0 is the head, 1 is the element next to head
 * and so on. Negative integers are used in order to count
 * from the tail, -1 is the last element, -2 the penultimate
 * and so on. If the index is out of range NULL is returned. */
listNode *listIndex(List *list, long index) {
    if (index >= list->len) return NULL;
    if (index < 0) index = list->len + index;
    
    listNode *n = list->head;
    while (index-- && n) n = n->next;

    return n;
}

/* Rotate the list removing the tail node and inserting it to the head. */
void listRotateTailToHead(List *list) {
    if (list->len <= 1) return;

    /* Detach current tail */
    listNode *tail = list->tail;
    list->tail = tail->prev;
    list->tail->next = NULL;
    /* Move it as head */
    list->head->prev = tail;
    tail->prev = NULL;
    tail->next = list->head;
    list->head = tail;
}

/* Rotate the list removing the head node and inserting it to the tail. */
void listRotateHeadToTail(List *list) {
    if (list->len <= 1) return;

    listNode *head = list->head;
    /* Detach current head */
    list->head = head->next;
    list->head->prev = NULL;
    /* Move it as tail */
    list->tail->next = head;
    head->next = NULL;
    head->prev = list->tail;
    list->tail = head;
}

/* Add all the elements of the list 'o' at the end of the
 * list 'l'. The list 'other' remains empty but otherwise valid. */
void listJoin(List *l, List *o) {
    if (l == NULL || o == NULL) return;
    if (o->len == 0) return;

    o->head->prev = l->tail;

    if (l->tail)
        l->tail->next = o->head;
    else
        l->head = o->head;

    l->tail = o->tail;
    l->len += o->len;

    /* Setup other as an empty list. */
    o->head = o->tail = NULL;
    o->len = 0;
}

#ifdef __TEST_LIST

#include <sys/time.h>

void *dup_intptr(void *ptr) {
    int *value = (int *)zmalloc(sizeof(int));
    if (value == NULL) return NULL;
    *value = *(int *)ptr;
    return value;
}

void free_intptr(void *ptr) {
    if (ptr == NULL) return;
    zfree(ptr);
}

int match_intptr(void *ptr, void *key) {
    return *(int *)ptr == *(int *)key;
}

long long timeInMilliseconds() {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

#define start_benchmark() start = timeInMilliseconds()
#define end_benchmark(msg) do { \
    elapsed = timeInMilliseconds()-start; \
    printf(msg ": %ld items in %lld ms\n", count, elapsed); \
} while(0);
#define end_benchmark_failure(msg) do { \
    elapsed = timeInMilliseconds()-start; \
    printf(msg ": %ld items in %lld ms\n", count, elapsed); \
    return -1; \
} while(0);

int main(int argc, char **argv) {
    long j;
    long long start, elapsed;
    long count = 0;
    bool signal;

    listIter *iter;
    listNode *node;
    List *list = listCreate();
    listSetDupMethod(list, dup_intptr);
    listSetFreeMethod(list, free_intptr);
    listSetMatchMethod(list, match_intptr);
    
    if (argc == 2) {
        count = strtol(argv[1], NULL, 10);
    } else {
        count = 5000000;
    }

    start_benchmark();
    for (j = 0; j < count; j++) {
        signal = listAddNodeTail(list, &j);
        if (!signal)
            end_benchmark_failure("fail to add node to list")
    }
    end_benchmark("add nodes to list")

    
    start_benchmark();
    iter = listGetIterator(list, AL_START_HEAD);
    if (iter == NULL)
        end_benchmark_failure("traverse end due to fail in allocating iterator")

    j = 0;
    while ((node = listNext(iter)) != NULL) {
        if (!list->match(node->value, &j))
            end_benchmark_failure("traverse end due to fail in matching items");
        j++;
    }
    listReleaseIterator(iter);
    end_benchmark("traverse from head to tail end success")

    start_benchmark();
    iter = listGetIterator(list, AL_START_TAIL);
    if (iter == NULL)
        end_benchmark_failure("traverse end due to fail in allocating iterator")

    j = count - 1;
    while ((node = listNext(iter)) != NULL) {
        if (!list->match(node->value, &j))
            end_benchmark_failure("traverse end due to fail in matching items");
        j--;
    }
    listReleaseIterator(iter);
    end_benchmark("traverse from tail to head end success")
}

#endif
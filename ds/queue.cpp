#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

#ifdef __cplusplus
extern "C" {
    #include "zmalloc.h"
}
#endif

Queue *queueCreate() {
    Queue *queue = (Queue *)zmalloc(sizeof(Queue));

    queue->len = 0;
    queue->head = queue->tail = NULL;
    queue->dup = NULL;
    queue->free = NULL;
    queue->match = NULL;

    return queue;
}

void queueRelease(Queue *queue) {
    if (queue) {
        queueEmpty(queue);
        zfree(queue);
    }
}

void queueEmpty(Queue *queue) {
    queueNode *current, *next;

    current = queue->head;
    while (current) {
        next = current->next;
        if (queue->free) queue->free(current->value);
        zfree(current);
        current = next;
    }
    queue->head = queue->tail = NULL;
    queue->len = 0;
}

bool enqueue(Queue *queue, void *value) {
    if (queue == NULL || value == NULL) return false;
    queueNode *node = (queueNode *)zmalloc(sizeof(queueNode));
    if (node == NULL) return false;

    node->value = value;
    if (queue->head == NULL) {
        queue->head = queue->tail = node;
        node->prev = node->next = NULL;
    }
    else {
        node->prev = queue->tail;
        node->next = NULL;
        queue->tail->next = node;
        queue->tail = node;
    }

    queue->len++;	
    return true;
}

queueNode *dequeue(Queue *queue) {
    if (queue == NULL) return NULL;
    else if (queue->len == 0) return NULL;
    else {
        queueNode *node = queue->head;
        queue->head = queue->head->next;
        if (queue->head) queue->head->prev = NULL;
        node->next = NULL;
        queue->len--;
        return node;
    }
}

queueIter *queueGetIterator(Queue *queue, int direction) {
    if (queue == NULL) return NULL;
    queueIter *iter = (queueIter *)zmalloc(sizeof(queueIter));
    if (iter == NULL) return NULL;

    if (direction == Q_START_HEAD)
        iter->next = queue->head;
    else
        iter->next = queue->tail;
    iter->direction = direction;
    return iter;
}

queueNode *queueNext(queueIter *iter) {
    if (iter == NULL) return NULL;
    
    queueNode *current = iter->next;
    if (current != NULL) {
        if (iter->direction == Q_START_HEAD)
            iter->next = current->next;
        else
            iter->next = current->prev;
    }
    return current;
}

void queueReleaseIterator(queueIter *iter) {
    if (iter) zfree(iter);
}

Queue *queueDup(Queue *orig) {
    return orig;
}

queueNode *queueSearchKey(Queue *queue, void *key) {
    if (queue == NULL || key == NULL) return NULL;

    queueIter *iter = queueGetIterator(queue, Q_START_HEAD);
    if (iter == NULL) return NULL;
    
    queueNode *node;
    while ((node = queueNext(iter)) != NULL) {
        if (queue->match) {
            if (queue->match(node->value, key))
                return node;
        } else if (key == node->value)
            return node;
    }
    return NULL;
}

queueNode *queueIndex(Queue *queue, long index) {
    if (index >= queue->len) return NULL;
    if (index < 0) index = queue->len + index;
    
    queueNode *n = queue->head;
    while (index-- && n) n = n->next;

    return n;
}

void queueRewind(Queue *queue, queueIter *iter) {
    if (queue && iter) {
        iter->next = queue->head;
        iter->direction = Q_START_HEAD;
    }
}

void queueRewindTail(Queue *queue, queueIter *iter) {
    if (queue && iter) {
        iter->next = queue->tail;
        iter->direction = Q_START_TAIL;
    }
}

void queueRotateTailToHead(Queue *queue) {
    if (queue->len <= 1) return;

    /* Detach current tail */
    queueNode *tail = queue->tail;
    queue->tail = tail->prev;
    queue->tail->next = NULL;
    /* Move it as head */
    queue->head->prev = tail;
    tail->prev = NULL;
    tail->next = queue->head;
    queue->head = tail;
}

void queueRotateHeadToTail(Queue *queue) {
    if (queue->len <= 1) return;

    queueNode *head = queue->head;
    /* Detach current head */
    queue->head = head->next;
    queue->head->prev = NULL;
    /* Move it as tail */
    queue->tail->next = head;
    head->next = NULL;
    head->prev = queue->tail;
    queue->tail = head;
}

void queueJoin(Queue *q, Queue *o) {
    if (q == NULL || o == NULL) return;
    if (o->len == 0) return;

    o->head->prev = q->tail;

    if (q->tail)
        q->tail->next = o->head;
    else
        q->head = o->head;

    q->tail = o->tail;
    q->len += o->len;

    /* Setup other as an empty queue. */
    o->head = o->tail = NULL;
    o->len = 0;
}

#ifdef __TEST_QUEUE

#include <sys/time.h>
#include "testhelp.h"

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

bool match_intptr(void *ptr, void *key) {
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

    queueIter *iter;
    queueNode *node;
    Queue *queue = queueCreate();
    Queue *queue2 = queueCreate();
    queueSetDupMethod(queue, dup_intptr);
    queueSetFreeMethod(queue, free_intptr);
    queueSetMatchMethod(queue, match_intptr);
    
    if (argc == 2) {
        count = strtol(argv[1], NULL, 10);
    } else {
        count = 5000000;
    }

    {
        start_benchmark();
        for (j = 0; j < count; j++) {
            signal = enqueue(queue, &j);
            if (!signal)
                end_benchmark_failure("fail to enqueue")
        }
        test_cond("enqueue", 
            queue->len == count)
        end_benchmark("enqueue finished")

        start_benchmark();
        for (j = 0; j < 500; j++) {
            node = dequeue(queue);
            if (!node)
                end_benchmark_failure("fail to dequeue")
            test_cond("dequeue", 
                queue->len == (count-1-j) && *(int *)node->value == j)
        }
        end_benchmark("dequeue finished")
    }
    test_report();
}

#endif
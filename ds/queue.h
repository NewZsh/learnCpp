#ifndef __QUEUE_H
#define __QUEUE_H

typedef struct queueNode {
    struct queueNode *prev;
    struct queueNode *next;
    void* value;
} queueNode;

typedef struct queueIter {
    queueNode *next;
    int direction;
} queueIter;

typedef struct Queue {
    queueNode* head;
    queueNode* tail;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    bool (*match)(void *ptr, void *key);
    size_t len;
} Queue;

/* Functions implemented as macros */
#define queueSetDupMethod(q,m) ((q)->dup = (m))
#define queueSetFreeMethod(q,m) ((q)->free = (m))
#define queueSetMatchMethod(q,m) ((q)->match = (m))
#define queueGetDupMethod(q) ((q)->dup)
#define queueGetFreeMethod(q) ((l)->free)
#define queueGetMatchMethod(q) ((q)->match)

/* Prototypes */
Queue *queueCreate();
void queueRelease(Queue *queue);
void queueEmpty(Queue *queue);
bool enqueue(Queue *queue, void *value);
queueNode *dequeue(Queue *queue);
queueIter *queueGetIterator(Queue *queue, int direction);
queueNode *queueNext(queueIter *iter);
void queueReleaseIterator(queueIter *iter);
Queue *queueDup(Queue *orig);
queueNode *queueSearchKey(Queue *queue, void *key);
queueNode *queueIndex(Queue *queue, long index);
void queueRewind(Queue *queue, queueIter *iter);
void queueRewindTail(Queue *queue, queueIter *iter);
void queueRotateTailToHead(Queue *queue);
void queueRotateHeadToTail(Queue *queue);
void queueJoin(Queue *q, Queue *o);

/* Directions for iterators */
#define Q_START_HEAD 0
#define Q_START_TAIL 0

#endif
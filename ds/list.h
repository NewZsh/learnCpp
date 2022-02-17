#ifndef __LIST_H__
#define __LIST_H__

typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;

typedef struct listIter {
    listNode *next;
    int direction;
} listIter;

typedef struct List {
    listNode *head;
    listNode *tail;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key);
    size_t len;
} List;

/* Functions implemented as macros */
#define listSetDupMethod(l,m) ((l)->dup = (m))
#define listSetFreeMethod(l,m) ((l)->free = (m))
#define listSetMatchMethod(l,m) ((l)->match = (m))
#define listGetDupMethod(l) ((l)->dup)
#define listGetFreeMethod(l) ((l)->free)
#define listGetMatchMethod(l) ((l)->match)

#define listAddNodeHead(l,v) (listInsertNode(l, (l)->head, v, 0))
#define listAddNodeTail(l,v) (listInsertNode(l, (l)->tail, v, 1))

/* Prototypes */
List *listCreate();
void listRelease(List *list);
void listEmpty(List *list);
bool listInsertNode(List *list, listNode *old_node, void *value, int after);
void listDelNode(List *list, listNode *node);
listIter *listGetIterator(List *list, int direction);
listNode *listNext(listIter *iter);
void listReleaseIterator(listIter *iter);
List *listDup(List *orig);
listNode *listSearchKey(List *list, void *key);
listNode *listIndex(List *list, long index);
void listRewind(List *list, listIter *li);
void listRewindTail(List *list, listIter *li);
void listRotateTailToHead(List *list);
void listRotateHeadToTail(List *list);
void listJoin(List *l, List *o);

/* Directions for iterators */
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif /* __LIST_H__ */
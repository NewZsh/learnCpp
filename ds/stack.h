#ifndef __STACK_H
#define __STACK_H

typedef struct stackItem {
    void *value;
    struct stackItem* next;
} stackItem;

typedef struct stackIter {
    stackItem *next;
} stackIter;

typedef struct Stack {
    stackItem* top;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    bool (*match)(void *ptr, void *key);
    size_t len;
} Stack;

/* Functions implemented as macros */
#define stackSetDupMethod(s,m) ((s)->dup = (m))
#define stackSetFreeMethod(s,m) ((s)->free = (m))
#define stackSetMatchMethod(s,m) ((s)->match = (m))
#define stackGetDupMethod(s) ((s)->dup)
#define stackGetFreeMethod(s) ((s)->free)
#define stackGetMatchMethod(s) ((s)->match)

/* Prototypes */
Stack *stackCreate();
void stackRelease(Stack *stack);
void stackEmpty(Stack *stack);
bool stackPush(Stack *stack, void *value);
stackItem *stackPop(Stack *stack);
stackIter *stackGetIterator(Stack *stack);
stackItem *stackNext(stackIter *iter);
void stackReleaseIterator(stackIter *iter);
Stack *stackDup(Stack *orig);
stackItem *stackSearchKey(Stack *stack, void *key);
stackItem *stackIndex(Stack *stack, long index);
void stackRewind(Stack *stack, stackIter *iter);
void stackJoin(Stack *s, Stack *o);

#endif
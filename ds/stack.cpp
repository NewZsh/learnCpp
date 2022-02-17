#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

#ifdef __cplusplus
extern "C" {
    #include "zmalloc.h"
}
#endif

Stack *stackCreate() {
    Stack *s = (Stack *)zmalloc(sizeof(Stack));
    if (s == NULL) return NULL;

    s->top = NULL;
    s->len = 0;
    return s;
}

void stackRelease(Stack *stack) {
    stackEmpty(stack);
    zfree(stack);
}

void stackEmpty(Stack *stack) {
    stackItem *current, *next;

    current = stack->top;
    while (current) {
        next = current->next;
        if (stack->free) stack->free(current->value);
        zfree(current);
        current = next;
    }
    stack->top = NULL;
    stack->len = 0;
}

bool stackPush(Stack *stack, void *value) {
    if (stack == NULL || value == NULL) return false;
    stackItem *item = (stackItem *)zmalloc(sizeof(stackItem));
    if (item == NULL) return false;

    item->value = value;
    item->next = stack->top;
    stack->top = item;
    stack->len++;
    return true;
}

stackItem *stackPop(Stack *stack) {
    if (stack == NULL) return NULL;
    else if (stack->top == NULL) return NULL;
    else {
        stackItem *tmp = stack->top;
        stack->top = stack->top->next;
        stack->len--;
        tmp->next = NULL;

        return tmp;
    }
}

stackIter *stackGetIterator(Stack *stack) {
    if (stack == NULL) return NULL;
    stackIter *iter = (stackIter *)zmalloc(sizeof(stackIter));
    if (iter == NULL) return NULL;
    iter->next = stack->top;
    return iter;
}

stackItem *stackNext(stackIter *iter) {
    if (iter == NULL) return NULL;

    stackItem *current = iter->next;
    if (current != NULL) iter->next = current->next;
    return current;
}

void stackReleaseIterator(stackIter *iter) {
    if (iter) zfree(iter);
}

Stack *stackDup(Stack *orig) {
    return orig;
}

stackItem *stackSearchKey(Stack *stack, void *key) {
    if (stack == NULL || key == NULL) return NULL;
    
    stackIter *iter = stackGetIterator(stack);
    if (iter == NULL) return NULL;

    stackItem *item;
    while ((item = stackNext(iter)) != NULL) {
        if (stack->match) {
            if (stack->match(item->value, key))
                return item;
        } else if (item->value == key)
            return item;
    }
    return NULL;
}

stackItem *stackIndex(Stack *stack, long index) {
    if (index >= stack->len) return NULL;
    if (index < 0) index = stack->len + index;

    stackItem *n = stack->top;
    while (index-- && n) n = n->next;

    return n;
}

void stackRewind(Stack *stack, stackIter *iter) {
    if (stack && iter) iter->next = stack->top;
}

void stackJoin(Stack *s, Stack *o) {
    if (s == NULL || o == NULL) return;
    if (o->len == 0) return;

    Stack *tmp = stackCreate();
    if (tmp == NULL) return;
    stackIter *iter;
    stackItem *item;
    iter = stackGetIterator(o);
    while ((item = stackNext(iter)) != NULL) {
        if (!stackPush(tmp, item->value))
            return;
    }
    zfree(iter);

    iter = stackGetIterator(tmp);
    while ((item = stackNext(iter)) != NULL) {
        if (!stackPush(s, item->value))
            return;
    }
    zfree(iter);

    /* Setup other as an stack. */
    o->top = NULL;
    o->len = 0;
}

#ifdef __TEST_STACK

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

    stackIter *iter;
    stackItem *item;
    Stack *stack1 = stackCreate();
    stackSetDupMethod(stack1, dup_intptr);
    stackSetFreeMethod(stack1, free_intptr);
    stackSetMatchMethod(stack1, match_intptr);
    Stack *stack2 = stackCreate();
    stackSetDupMethod(stack2, dup_intptr);
    stackSetFreeMethod(stack2, free_intptr);
    stackSetMatchMethod(stack2, match_intptr);
    
    if (argc == 2) {
        count = strtol(argv[1], NULL, 10);
    } else {
        count = 5000000;
    }

    {
        start_benchmark();
        bool signal;
        for (j = 0; j < count; j++) {
            signal = stackPush(stack1, &j);
            if (!signal)
                end_benchmark_failure("fail to push item to stack1")
        }
        end_benchmark("push items to stack1")

        start_benchmark();
        for (j = count; j < 2*count; j++) {
            signal = stackPush(stack2, &j);
            if (!signal)
                end_benchmark_failure("fail to push item to stack2")
        }
        end_benchmark("push items to stack2")

        start_benchmark();
        iter = stackGetIterator(stack1);
        if (iter == NULL)
            end_benchmark_failure("traverse end due to fail in allocating iterator")

        j = count-1;
        while ((item = stackNext(iter)) != NULL) {
            if (!stack1->match(item->value, &j))
                end_benchmark_failure("traverse end due to fail in matching items");
            j--;
        }
        stackReleaseIterator(iter);
        test_cond("traverse stack1",
            stack1->len == count && j == -1)
        end_benchmark("traverse stack1 end success")
        
        start_benchmark();
        iter = stackGetIterator(stack2);
        if (iter == NULL)
            end_benchmark_failure("traverse end due to fail in allocating iterator")

        j = 2*count-1;
        while ((item = stackNext(iter)) != NULL) {
            if (!stack2->match(item->value, &j))
                end_benchmark_failure("traverse end due to fail in matching items");
            j--;
        }
        stackReleaseIterator(iter);
        test_cond("traverse stack1",
            stack2->len == count && j == count-1)
        end_benchmark("traverse stack2 end success")

        start_benchmark();
        stackJoin(stack1, stack2);
        iter = stackGetIterator(stack1);
        if (iter == NULL)
            end_benchmark_failure("traverse end due to fail in allocating iterator")

        j = 2*count-1;
        while ((item = stackNext(iter)) != NULL) {
            if (!stack1->match(item->value, &j))
                end_benchmark_failure("traverse end due to fail in matching items");
            j--;
        }
        stackReleaseIterator(iter);
        test_cond("traverse combined stack",
            stack1->len == 2*count && j == -1)
        end_benchmark("traverse stack2 end success")
    }
    test_report();
}

#endif
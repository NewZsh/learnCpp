#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue.h"
#include "stack.h"

#ifdef __cplusplus
extern "C" {
    #include "zmalloc.h"
}
#endif

#ifdef __TEST_MATHEXP

char OP[4] = {'+', '-', '*', '/'};
bool match_char(void *ptr, void * key) {
    return *(char *)ptr == *(char *)key;
}

void printStack(Stack *s) {
    stackIter *iter = stackGetIterator(s);
    stackItem *node;
    while ((node = stackNext(iter)) != NULL)
        printf("%c --> ", *(char *)node->value);

    printf("\n");
}

void printStackMix(Stack *s) {
    stackIter *iter = stackGetIterator(s);
    stackItem *node;
    bool signal;
    short int i;
    while ((node = stackNext(iter)) != NULL) {
        signal = false;
        for (i = 0; i < 4; i++)
            if (*(char *)node->value == OP[i]) {
                signal = true;
                break;
            }
        if (signal)
            printf("%c --> ", *(char *)node->value);
        else
            printf("%lf --> ", *(double *)node->value);   
    }
    printf("\n");
}

void printQueueMix(Queue *q) {
    queueIter *iter = queueGetIterator(q, 0);
    queueNode *node;
    bool signal;
    short int i;
    printf("queue: ");
    while ((node = queueNext(iter)) != NULL) {
        signal = false;
        for (i = 0; i < 4; i++)
            if (*(char *)node->value == OP[i]) {
                signal = true;
                break;
            }
        if (signal)
            printf("%c --> ", *(char *)node->value);
        else
            printf("%lf --> ", *(double *)node->value);   
    }
    printf("\n");
}

double *get_number(Stack *stack, bool neg) {
    if (!stack) return NULL;
    double *num = (double *)zmalloc(sizeof(double));
    *num = 0;
    int fp_pos = 0;
    bool find = false;
    long int p = 1;
    stackItem *node;
    
    while (stack->len > 0) {
        node = stackPop(stack);
        if (*(char *)node->value == '.')
            find = true;
        else {
            *num += (p * (*(char *)node->value - 48));
            p *= 10;
        }

        if (!find)
            fp_pos++;
    }
    
    if (find) for (; fp_pos > 0; fp_pos--, *num /= 10);
    *num = neg ? -(*num) : *num;
    return num;
}

Queue *eliminate_num(Stack *stack) {
    if (!stack) return NULL;

    double *num;
    short int op = -1;
    bool num_in_char = false;
    short int i = 0;
    bool neg = false;
    bool start = true;
    stackItem *node;
    Queue *queue = queueCreate();
    queueSetMatchMethod(queue, match_char);
    Stack *num_stack = stackCreate();
    while (stack->len > 0) {
        node = stackPop(stack);

        op = -1;
        for (i = 0; i < 4; i++)
            if (stack->match(node->value, &OP[i])) {
                op = i;
                break;
            }

        if ((*(char *)node->value >= '0' && *(char *)node->value <= '9') || *(char *)node->value == '.')
            num_in_char = true;
        else
            num_in_char = false;

        if (op == -1) {
            if (num_in_char) stackPush(num_stack, node->value);
            else enqueue(queue, node->value);
        }
        else {
            if (op == 1 && start) neg = true;
            else if (op >= 2 && start) return NULL; // meaningless '+' before number, unexpected expression, '*' or '/' before number
            else {
                if (num_stack->len > 0) {
                    num = get_number(num_stack, neg);
                    neg = false;
                    enqueue(queue, num);
                }
                enqueue(queue, node->value);
            }
        }

        if (start) start = false;
    }
    
    if (num_stack->len > 0) {
        num = get_number(num_stack, neg);
        enqueue(queue, num);
    }

    stackRelease(stack);
    stackRelease(num_stack);
    return queue;
}

Queue *eliminate_high(Queue *queue) {
    if (!queue) return NULL;

    double tmp;
    double *num;
    short int op, last_op = -1;
    short int i = 0;
    queueNode *node;
    Queue *new_queue = queueCreate();
    queueSetMatchMethod(new_queue, match_char);
    while (queue->len > 0) {
        node = dequeue(queue);

        op = -1;
        for (i = 0; i < 4; i++)
            if (queue->match(node->value, &OP[i])) {
                op = i;
                break;
            }

        // number 
        if (op == -1) {
            num = (double *)node->value;
            if (last_op == 2) {
                *num *= tmp;
                last_op = -1;
            }
            else if (last_op == 3) {
                *num = tmp / *num;
                last_op = -1;
            }
        }
        // * or /
        else if (op >= 2) {
            last_op = op;
            tmp = *num;
        }
        // + or -
        else {
            enqueue(new_queue, num);
            enqueue(new_queue, node->value);
        }
    }

    enqueue(new_queue, num);
    queueRelease(queue);
    return new_queue;
}

double *eliminate_low(Queue *queue) {
    if (!queue) return NULL;

    queueNode *node;
    double *num = (double *)zmalloc(sizeof(double));
    *num = 0;
    short int i, last_op = -1, op = -1;
    while (queue->len > 0) {
        node = dequeue(queue);

        op = -1;
        for (i = 0; i < 2; i++)
            if (queue->match(node->value, &OP[i])) {
                op = i;
                break;
            }
        
        if (op == -1) {
            if (last_op == -1) *num = *(double *)node->value;
            else if (last_op == 0) *num += *(double *)node->value;
            else if (last_op == 1) *num -= *(double *)node->value;
        }
        else last_op = op;
    }

    queueRelease(queue);
    return num;
}

void format(char *op) {
    int i = strlen(op);
    op[i+2] = '\0';
    op[i+1] = ')';
    while (i > 0) {
        op[i] = op[i-1];
        i--;
    }
    op[0] = '(';
}

float parse_math_op(char *op) {
    int i = 0;
    double *num;
    Stack *main_stack = stackCreate();
    Stack *sub_stack = stackCreate();
    Queue *queue;
    stackSetMatchMethod(sub_stack, match_char);
    stackItem *node;

    while (op[i] != '\0') {
        if (op[i] != ')') stackPush(main_stack, &op[i]);
        else {
            while (1) {
                node = stackPop(main_stack);
                if (!node) {
                    printf("not a valid expression.\n");
                    return -1;
                }
                if (*(char *)node->value == '(')
                    break;
                
                stackPush(sub_stack, node->value);
            }

            // printStackMix(sub_stack);
            queue = eliminate_num(sub_stack);
            // printQueueMix(queue);
            queue = eliminate_high(queue);
            // printQueueMix(queue);
            num = eliminate_low(queue);
            stackPush(main_stack, num);
            // printStackMix(main_stack);
        }

        i++;
    }

    queue = queueCreate();
    queueSetMatchMethod(queue, match_char);
    while (main_stack->len > 0)
        enqueue(queue, stackPop(main_stack)->value);
    queue = eliminate_high(queue);
    num = eliminate_low(queue);

    return *num;
}

int main() {
    char op[102];
    scanf("%s", op);
    format(op);

    num = parse_math_op(op);
    printf("%lf\n", *num);
    return 0;
}

#endif
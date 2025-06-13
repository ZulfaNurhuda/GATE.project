#ifndef ADT_STACK_H
#define ADT_STACK_H

typedef struct
{
    void **items;
    int top;
    int capacity;
} Stack;

void stack_init(Stack *stack, int capacity);
void stack_push(Stack *stack, void *item);
void *stack_pop(Stack *stack);

#endif
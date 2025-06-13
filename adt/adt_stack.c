#include "adt_stack.h"
#include <stdlib.h>

void stack_init(Stack *stack, int capacity)
{
    stack->items = (void **)malloc(sizeof(void *) * capacity);
    stack->top = -1;
    stack->capacity = capacity;
}

void stack_push(Stack *stack, void *item)
{
    if (stack->top < stack->capacity - 1)
    {
        stack->items[++stack->top] = item;
    }
}

void *stack_pop(Stack *stack)
{
    if (stack->top >= 0)
    {
        return stack->items[stack->top--];
    }
    return NULL;
}
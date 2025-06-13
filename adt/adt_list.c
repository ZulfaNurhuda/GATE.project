#include "adt_list.h"
#include <stdlib.h>

void list_init(List *list)
{
    list->head = NULL;
}

void list_append(List *list, void *data)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->data = data;
    node->next = list->head;
    list->head = node;
}

void *list_get(List *list, int index)
{
    ListNode *current = list->head;
    for (int i = 0; i < index && current != NULL; i++)
    {
        current = current->next;
    }
    return current ? current->data : NULL;
}
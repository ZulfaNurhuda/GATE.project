#ifndef ADT_LIST_H
#define ADT_LIST_H

typedef struct ListNode
{
    void *data;
    struct ListNode *next;
} ListNode;

typedef struct
{
    ListNode *head;
} List;

void list_init(List *list);
void list_append(List *list, void *data);
void *list_get(List *list, int index);

#endif
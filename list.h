#if !defined LIST_INCLUDED
#define LIST_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#define LIST_NO_NEXT_OR_PREV_VAL 0
#define LIST_FIRST_ELEM_VAL      0
#define LIST_OVERFLOW           -1
#define PUSH_ERROR              -1
#define VERIFICATION_ERROR      -1
#define INCCORECT_INSERTION     -1
#define INIT_ERROR              -1
#define REMOVE_ERROR            -1
#define CONVERTATION_ERROR      -1
#define DESTRUCTION_ERROR       -1

#ifdef DEBUG
#define ON_DEBUG(code) code
#else
#define ON_DEBUG(code)
#endif

typedef int       list_t;
typedef long long indx_t;

struct List_node
{
    list_t data;
    
    indx_t next;
    indx_t prev;
};


struct List 
{
    List_node* elements;
    int        is_sorted;

    indx_t     tail;
    indx_t     head;
    indx_t     free;

    indx_t     capacity;
    indx_t     size;
    ON_DEBUG(
    hash_t     hash;
    )
};

#define PushFront(list, value)            \
InsertBefore(list, (list)->tail, value);

#define PushBack(list, value)             \
InsertBefore(list, (list)->head, value);

#define PopBack(list)                     \
RemoveFromList(list, (list)->tail);

#define PopFront(list)                    \
RemoveFromList(list, (list)->head);

int ListInit(List* list,  indx_t list_size);

int IsEmptyList(List* to_check);

indx_t InsertAfter(List* list, indx_t physical_index, list_t value);

int VerifyList(List* list);

void ListGraphicalDump(List* to_dump);

int RemoveFromList(List* list, indx_t physical_index);

indx_t InsertBefore(List* list, indx_t physical_index, list_t value);

indx_t ConvertLogicalToPhysicalIndex(List* list, indx_t logical_index);

int ListDtor(List* list);

#endif
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#define LIST_NO_NEXT_OR_PREV_VAL 0
#define LIST_FIRST_ELEM_VAL      0
#define LIST_OVERFLOW           -1
#define VERIFICATION_ERROR      -1
#define INCCORECT_INSERTION     -1

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

    indx_t     tail;
    indx_t     head;
    indx_t     free;

    indx_t     capacity;
    indx_t     size;
    ON_DEBUG(
    hash_t     hash;
    )
};


void ListInit(List* to_init, indx_t list_size);

int IsEmptyList(List* to_check);

int InsertToList(List* list, indx_t physical_index, list_t value);

int VerifyList(List* list);

void RemoveFromList(List* list, ssize_t physical_index);

void ListGraphicalDump(List* to_dump);
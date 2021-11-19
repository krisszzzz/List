#define DEBUG  // Remove it if you in release state, but then the responsibility lies with you and only you
               // Debug mode include hash-protection, visual dump and more deeper list verificator
               // Used before list.h and log.h because they're contain this define proccessing
#include "log.h"
#include "murmurhash.h"
#include "list.h"

#define RET_IF(condition, ret_val, ...)  \
if(condition) {                          \
    __PRINT_ALL_INFO__(__VA_ARGS__);     \
    return ret_val;                      \
}

#define NEXT(index)                 \
list->elements[index].next

#define PREV(index)                 \
list->elements[index].prev

#define DATA(index)                 \
list->elements[index].data

#define HASH     list->hash

#define SIZE     list->size

#define CAPACITY list->capacity

#define HEAD     list->head

#define TAIL     list->tail

#define SPACE    physical_index

#define FREE     list->free

/// DSL to simplify code

const list_t Empty_elem_sign = (list_t)MurmurHash("Danya + proccessor = dead inside", 32); 
const list_t Poison          = (list_t)MurmurHash("Poltorashka koshachiy zasranetc",  31);
/// Special constant to signify that the some element is free

int ListInit(List* list,  indx_t list_size)
{
    RET_IF(list == nullptr, INIT_ERROR, "Error: nullptr object\n");

    if(list_size <= 8) {
        __PRINT_ALL_INFO__("Warning list size in constructor at least should equal 8\n");
        list_size = 8;
    }

    List_node* elements    = (List_node*)calloc(list_size + 1, sizeof(List_node));
    list->elements         = elements;

    RET_IF(elements == nullptr, INIT_ERROR, "Inccorect memory allocation. Please write me on github: https://github.com/krisszzzz\n");

    NEXT(0) = LIST_NO_NEXT_OR_PREV_VAL;
    PREV(0) = LIST_NO_NEXT_OR_PREV_VAL;
    DATA(0) = LIST_FIRST_ELEM_VAL;
    /// First element settings (First element cannot be used to store value)
    
    for(indx_t iter_count = 1; iter_count < list_size; ++iter_count)
    {
        DATA(iter_count) = Empty_elem_sign;
        NEXT(iter_count) = iter_count + 1;
        PREV(iter_count) = iter_count - 1;
    }

    NEXT(list_size) = LIST_NO_NEXT_OR_PREV_VAL;
    PREV(list_size) = list_size - 1;
    DATA(list_size) = Empty_elem_sign;
    FREE            = 1; 
    CAPACITY        = list_size;
    list->is_sorted = 1;
    ON_DEBUG(
        HASH        = MurmurHash((char*)list, sizeof(List) - sizeof(hash_t));
    )
    return 0;
}

int IsEmptyList(List* to_check)
{
    return (to_check->size == 0) ? 1 : 0;
}

static void RemoveConnection(List* list, indx_t physical_index)                 /// Remove connection for the element with physical_index
{
    if(NEXT(SPACE) != 0) {
        PREV(NEXT(SPACE)) = PREV(SPACE);
    }

    if(PREV(SPACE) != 0) {
        NEXT(PREV(SPACE)) = NEXT(SPACE);
    }
    NEXT(SPACE) = 0;
    PREV(SPACE) = 0;
}


/// Adding new element AFTER PHYSICAL(!!!) index

indx_t InsertAfter(List* list, indx_t physical_index, list_t value) 
{
    RET_IF((SPACE > CAPACITY || SPACE <= 0) && !IsEmptyList(list),   INCCORECT_INSERTION, 
           "Inccorect physical index, index bounds is (%d, %lld]\n",          0,          CAPACITY);

    RET_IF(VerifyList(list) != 0, INCCORECT_INSERTION, "Verification error\n");

    RET_IF(DATA(SPACE) == Empty_elem_sign && !IsEmptyList(list), INCCORECT_INSERTION, 
           "Inccorect physical index, you cannot push new element after element with " 
           "this physical index, because this element is empty. Physical index is %lld\n", physical_index);
    
    RET_IF(CAPACITY == SIZE, INCCORECT_INSERTION, "List overflow\n");

    list->is_sorted      = (SPACE == HEAD) && (!IsEmptyList(list)) && (list->is_sorted != 0);
    indx_t current_free  = FREE;

    FREE                 = NEXT(FREE);

    RemoveConnection(list, current_free);

    if(NEXT(SPACE) != 0) {
        PREV(NEXT(SPACE))   = current_free;
        NEXT(current_free)  = NEXT(SPACE) * (!IsEmptyList(list));
    }

    if(!IsEmptyList(list)) {
        NEXT(SPACE)       = current_free;          
    } else {
        HEAD              = current_free; /// set list head as current free element, but only for empty list
    }

    PREV(current_free)    = SPACE * (!IsEmptyList(list));  /// 0 if list is empt
    DATA(current_free)    = value;
    FREE                  = FREE * (CAPACITY != SIZE);

    if(NEXT(current_free) == 0) {
        TAIL              = current_free;
    }
    SIZE++;

    ON_DEBUG(
        HASH              = MurmurHash((char*)list, sizeof(List) - sizeof(hash_t));
    )
    return current_free;
}

indx_t InsertBefore(List* list, indx_t physical_index, list_t value)
{
    RET_IF(VerifyList(list) != 0, INCCORECT_INSERTION, "Verification error\n");

    RET_IF((SPACE > CAPACITY || SPACE <= 0) && !IsEmptyList(list), INCCORECT_INSERTION,
          "Inccorect physical index, index bounds is (%d, %lld]\n",         0,          CAPACITY);

    RET_IF(SIZE == CAPACITY, LIST_OVERFLOW, "Cannot add another element, because list overflow\n");
    
    if(IsEmptyList(list)) {
        TAIL = FREE;
    } else {
        list->is_sorted = 0;
    }

    if(DATA(SPACE) != Empty_elem_sign || IsEmptyList(list)) {
        if(PREV(SPACE) == 0 || IsEmptyList(list)) {
            indx_t current_free     = FREE;
            FREE                    = NEXT(FREE);

            RemoveConnection(list, current_free);
            
            if(!IsEmptyList(list)) {
                NEXT(current_free)  = SPACE;
                PREV(SPACE)         = current_free;
                PREV(current_free)  = 0;
            }
            
            DATA(current_free)      = value;
            HEAD                    = current_free;
            SIZE++;

            ON_DEBUG(
                HASH                = MurmurHash((char*)list, sizeof(List) - sizeof(hash_t));
            )
            return current_free;    
        }
        ON_DEBUG(
                HASH                = MurmurHash((char*)list, sizeof(List) - sizeof(hash_t));
        )

        return InsertAfter(list, PREV(SPACE), value);

    } else {
        __PRINT_ALL_INFO__("Error: you cannot insert element before element with physical index = %lld, because this element is empty\n", 
                           physical_index);
        return INCCORECT_INSERTION;
    }
}

int RemoveFromList(List* list, indx_t physical_index)
{
    RET_IF(VerifyList(list) != 0, REMOVE_ERROR, "Verification error\n");

    RET_IF(IsEmptyList(list), REMOVE_ERROR, "Cannot remove element, because the list is empty\n");

    RET_IF(SPACE  <= 0 || SPACE > CAPACITY, REMOVE_ERROR, 
           "Inccorect physical index - he should be in range (%d, %lld]", 
           0, CAPACITY);
    
    RET_IF(DATA(SPACE) == Empty_elem_sign, REMOVE_ERROR,
           "Error: you want to remove empty object in list, physical index = %lld\n",
           SPACE);
    list->is_sorted = (SPACE == TAIL || SPACE == HEAD) && list->is_sorted;

    if(SPACE == TAIL) {
        if(PREV(TAIL) != 0) {
            TAIL = PREV(TAIL);
        } else {
            TAIL = 0;
        }
    }
    if(SPACE == HEAD) {
        if(NEXT(HEAD) != 0) {
            HEAD = NEXT(HEAD);
        } else {
            HEAD = 0;
        }
    }

    RemoveConnection(list, SPACE);
    if(FREE != 0) {
        PREV(FREE) = SPACE;
    }
    DATA(SPACE) = Empty_elem_sign;
    PREV(SPACE) = 0;
    NEXT(SPACE) = FREE;
    FREE        = SPACE;
    SIZE--;

    ON_DEBUG(
        HASH    = MurmurHash((char*)list, sizeof(List) - sizeof(hash_t));
    )
    return 0;
}

#define ADD_VERIFY_CONDITION(condition, ...)                        \
if(condition) {                                                     \
    __PRINT_ALL_INFO__(__VA_ARGS__);                                \
}


#define CHECK_CONNECTIONS(list, start_check_from_this_index, num_of_iteration_started, additional_condition)      \
while (start_check_from_this_index != 0)                                                                          \
{                                                                                                                 \
    ++num_of_iteration_started;                                                                                   \
    if(NEXT(start_check_from_this_index) == 0) {                                                                  \
        break;                                                                                                    \
    }                                                                                                             \
    if(start_check_from_this_index != PREV(NEXT(start_check_from_this_index))) {                                  \
        __PRINT_ALL_INFO__("Inccorect bounds. Watch the elements with indexes %lld and %lld\n",                   \
                           start_check_from_this_index, list->elements[start_check_from_this_index].next);        \
        break;                                                                                                    \
    }                                                                                                             \
    additional_condition                                                                                          \
    start_check_from_this_index = list->elements[start_check_from_this_index].next;                               \
}                                                                                                                 
//function or function + define

int VerifyList(List* list)
{
    RET_IF(list == nullptr, VERIFICATION_ERROR, "Fatal error: cannot verify nullptr object.\n");
    
    /// Check hash
    ON_DEBUG(
        RET_IF(HASH != MurmurHash((char*)list, sizeof(List) - sizeof(hash_t)), VERIFICATION_ERROR,
               "Fatal error: corrupted list, the hash was changed: hash value should be %lld, but he is %lld\n",
               MurmurHash((char*)list, sizeof(List) - sizeof(hash_t)), HASH);
    )    
    ADD_VERIFY_CONDITION(CAPACITY < 8,              "Error: capacity below zero\n");
    ADD_VERIFY_CONDITION(SIZE < 0,                  "Error: size below zero\n");
    ADD_VERIFY_CONDITION(TAIL == HEAD && SIZE > 1,  "Error: tail cannot coincide with head\n");
    ADD_VERIFY_CONDITION(FREE  < 0,                 "Error: negative free\n");
    ADD_VERIFY_CONDITION(HEAD  < 0,                 "Error: negative free\n");
    ADD_VERIFY_CONDITION(TAIL  < 0,                 "Error: negative free\n");
    ADD_VERIFY_CONDITION(PREV(0) != 0,              "Element with the index 0 cannot be used. The previous element have to be 0\n");
    ADD_VERIFY_CONDITION(NEXT(0) != 0,              "Element with the index 0 cannot be used. The next element have to be 0\n");

    /// Check prev/next bound for free and ocupied elements
    ON_DEBUG(
            indx_t free_elem_index      = FREE;
            indx_t occupied_elem_index  = HEAD;
            indx_t num_of_free_elem     = 0;
            indx_t num_of_occupied_elem = 0;
            if(SIZE > 1) {
                CHECK_CONNECTIONS(list, free_elem_index, num_of_free_elem, if(DATA(free_elem_index) != Empty_elem_sign) {
                                                                                __PRINT_ALL_INFO__("Error: the element with this index %lld "
                                                                                                   "should be free\n", free_elem_index);
                                                                                break;
                                                                            }
                )

                
                if(num_of_free_elem != CAPACITY - SIZE) {
                    __PRINT_ALL_INFO__("Error: number of free element is %lld, but the difference between capacity and size "
                                       "(capacity - size) is %lld\n", num_of_free_elem, CAPACITY - SIZE);
                }

                CHECK_CONNECTIONS(list, occupied_elem_index, num_of_occupied_elem, if(DATA(occupied_elem_index) == Empty_elem_sign) {
                                                                                        __PRINT_ALL_INFO__("Error: the element with this index %lld "
                                                                                                           "do not should be free\n", occupied_elem_index);                                                                                
                                                                                    }
                )
                
                if(SIZE > 1)  {         /// Useless to check bounds if list is free or only one object in list
                    ADD_VERIFY_CONDITION(num_of_occupied_elem != SIZE, 
                                         "Inccorect size or number of occupied elements is inccorect. "
                                         "Size is %lld, number of occupied elements is %lld\n", 
                                         SIZE, num_of_occupied_elem);
        
                    ADD_VERIFY_CONDITION(occupied_elem_index  != TAIL, "Inccorect value of tail. It's isn't the last element in the list\n");
                }
            }

            )
    return 0;

}

void ListGraphicalDump(List* to_dump)
{
    const int MAX_FILE_NAME_SIZE  = 32;
    const int MAX_SYSTEM_ARG_SIZE = 128; 

    static int num_of_calling = 0;
    num_of_calling++;

    if(to_dump == nullptr) {
        __PRINT_ALL_INFO__("Cannot dump a nullptr object\n");
        return;
    }
    if(to_dump->hash != MurmurHash((char*)to_dump, sizeof(List) - sizeof(hash_t))) {
        __PRINT_ALL_INFO__("Warning: the output can be inccorect, because hash was changed\n");
    }
    
    char file_name[MAX_FILE_NAME_SIZE] = {};

    sprintf(file_name, "gen.graphviz_code%d.gv", num_of_calling);
    
    FILE* to_gener_graphviz_code_for_debug = fopen(file_name, "w");
    
    
    if(to_gener_graphviz_code_for_debug == nullptr) {
        __PRINT_ALL_INFO__("Nullptr FILE*\n");
        return;
    }

    const char* red     = "#FF143C"; /// Red hex-code
    const char* green   = "#00FF7F"; /// Green hex-code
    char* current_color = nullptr;
    
    fprintf(to_gener_graphviz_code_for_debug, "digraph G{\n"
                                              "rankdir = LR\n"
                                              "dot_0_%d [shape = record, fontname = Helvetica, style = \"filled\", "
                                              "fillcolor = \"magenta\", label = \" index  | {<index> 0 }  | { {<prev> prev | %lld } | { data  | %d } | "
                                              "{<next> next | %lld } }\" ]\n", 
                                              num_of_calling,  to_dump->elements[0].prev, to_dump->elements[0].data, to_dump->elements[0].next);

    fprintf(to_gener_graphviz_code_for_debug,"info_tab_%d [shape = record, fontname = Helvetica, style = \"filled\", fillcolor = \"magenta\", "
                                             "label = \" <info> info  | { {<tail> tail | %lld } | {<head> head | %lld } | {<free> free | %lld } "
                                             "| {size | %lld } | {capacity | %lld} }\" ]",
                                              num_of_calling,  to_dump->tail,    to_dump->head,    to_dump->free, 
                                              to_dump->size,   to_dump->capacity);
    
    for(indx_t iter_count = 1; iter_count <= to_dump->capacity; ++iter_count)
    {
        if(to_dump->elements[iter_count].data == Empty_elem_sign) {
            current_color = (char*)green;
        } else {
            current_color = (char*)red;
        }

        fprintf(to_gener_graphviz_code_for_debug, "dot_%lld_%d [shape = record, fontname = Helvetica, style = \"filled\", "
                                                  "fillcolor = \"%s\", label = \" index  | {<index> %lld }  | { {<prev> prev | %lld } | { data  | %d } | "
                                                  "{<next> next | %lld } }\" ]\n", 
                                                  iter_count,                         num_of_calling,                     current_color,   
                                                  iter_count,                         to_dump->elements[iter_count].prev, to_dump->elements[iter_count].data, 
                                                  to_dump->elements[iter_count].next); 

        fprintf(to_gener_graphviz_code_for_debug, "dot_%lld_%d: index -> dot_%lld_%d: index[style = \"invis\"];\n"
                                                  "dot_%lld_%d: index -> dot_%lld_%d: index[style = \"invis\"];\n"
                                                  "dot_%lld_%d: next  -> dot_%lld_%d: index;\n"
                                                  "dot_%lld_%d: prev  -> dot_%lld_%d: index;\n", 
                                                  iter_count,                               num_of_calling, 
                                                  abs(to_dump->elements[iter_count].next),  num_of_calling,
                                                  iter_count,                               num_of_calling, 
                                                  abs(to_dump->elements[iter_count].prev),  num_of_calling,
                                                  iter_count,                               num_of_calling, 
                                                  abs(to_dump->elements[iter_count].next),  num_of_calling,
                                                  iter_count,                               num_of_calling, 
                                                  abs(to_dump->elements[iter_count].prev),  num_of_calling);

    }

    fprintf(to_gener_graphviz_code_for_debug, "}\n");
    fclose (to_gener_graphviz_code_for_debug);

    char system_call_arg[MAX_SYSTEM_ARG_SIZE] = {};

    sprintf(system_call_arg, "dot -Tpng %s -o Debug/image_%d.png", file_name, num_of_calling);
    system(system_call_arg);
    
}

indx_t ConvertLogicalToPhysicalIndex(List* list, indx_t logical_index)
{
    RET_IF(VerifyList(list) != 0,                                CONVERTATION_ERROR, "Verification error\n");
    RET_IF(IsEmptyList(list),                                    CONVERTATION_ERROR, "Nothing to convert in empty list");
    RET_IF(logical_index > list->capacity && logical_index <= 0, CONVERTATION_ERROR, "Inccorect logical index - %lld", logical_index);

    if(list->is_sorted) {
        return TAIL - 1 + logical_index;
    }    
    indx_t physical_index = HEAD;
    while(true) 
    {
        if(NEXT(physical_index) != 0 && logical_index > 0) {
            --logical_index;
            //printf("%d", logical_index);
            physical_index = NEXT(physical_index);
        } else {
            RET_IF(logical_index != 0, CONVERTATION_ERROR, "Physical index with this logical index doesn't exist. "
                                                           "Logical index = %lld\n", logical_index);
            return physical_index;
        }
    }

}

int ListDtor(List* list)
{
    RET_IF(VerifyList(list) != 0, DESTRUCTION_ERROR, "Verification error\n");
    SIZE = CAPACITY = FREE = HEAD = TAIL = Poison;
    ON_DEBUG(
        HASH = Poison;
    )
    list->is_sorted = Poison;
    free(list->elements);
    return 0;
}
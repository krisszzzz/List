#include "../proccessor/Proccessor_Lib/log.h"
//#include "../Proccessor/Proccessor_Lib/log.cpp"
//#include "../Proccessor/Proccessor_lib/murmurhash.cpp"

typedef long long ssize_t;

#define DEBUG  // Remove it if you in release state, but then the responsibility lies with you and only you
               // Debug mode include hash-protection, visual dump and more deeper list verificator
               // Used before list.h and log.h because they're contain this define proccessing
#include "../proccessor/Proccessor_Lib/murmurhash.h"
#include "list.h"



const list_t Empty_elem_sign = (list_t)MurmurHash("Danya + proccessor = dead inside", 32); 
/// Special constant to signify that the some element is free


void ListInit(List* to_init, indx_t list_size)
{

    if(to_init == nullptr) {
	    __PRINT_ALL_INFO__("Error: nullptr object\n");
        return;
    }

    if(list_size <= 8) {
        __PRINT_ALL_INFO__("Warning list size in constructor at least should equal 8\n");
        list_size = 8;
    }

    List null_list = {};

    if(memcmp((const void*)&null_list, (const void*)to_init, sizeof(List)) != 0) {
        __PRINT_ALL_INFO__("Unitialized list\n");
        return;
    }

    List_node* elements       = (List_node*)calloc(list_size, sizeof(List_node));
    to_init->elements         = elements;

    if(elements == nullptr) {
        __PRINT_ALL_INFO__("Inccorect memory allocation. Please write me on github: https://github.com/krisszzzz\n");
        return;
    }
    to_init->elements[0].next = LIST_NO_NEXT_OR_PREV_VAL;
    to_init->elements[0].next = LIST_FIRST_ELEM_VAL;
    to_init->elements[0].prev = LIST_NO_NEXT_OR_PREV_VAL;
    to_init->elements[0].prev = LIST_FIRST_ELEM_VAL;
    //to_init->next[0] = LIST_NO_NEXT_OR_PREV_VAL;        /// First element settings (First element cannot be used to store value)

    for(indx_t iter_count = 1; iter_count < list_size; ++iter_count)
    {
        to_init->elements[iter_count].data = Empty_elem_sign;
        to_init->elements[iter_count].next = iter_count + 1;
        to_init->elements[iter_count].prev = iter_count - 1;
    }

    to_init->elements[list_size].next = LIST_NO_NEXT_OR_PREV_VAL;
    to_init->elements[list_size].prev = list_size - 1;
    to_init->elements[list_size].data = Empty_elem_sign;

    to_init->tail                     = 0;
    to_init->head                     = 0;
    to_init->free                     = 0;
    to_init->size                     = 0;
    to_init->capacity                 = list_size;
    ON_DEBUG(
        to_init->hash                 = MurmurHash((char*)to_init, sizeof(List) - sizeof(hash_t));
    )
    
}

int IsEmptyList(List* to_check)
{
    
    return (to_check->tail == 0 && to_check->head == 0 && to_check->free == 0 && to_check->size == 0) ? 1 : 0;
}

/// Adding new element AFTER PHYSICAL(!!!) index

int InsertToList(List* list, indx_t physical_index, list_t value) 
{
   // printf("ok");
    if(physical_index > list->capacity || physical_index <= 0) {
        __PRINT_ALL_INFO__("Inccorect physical index, index bounds is (%d, %zd]\n", 0, list->capacity);
        return INCCORECT_INSERTION;
    }

    if(VerifyList(list) != 0) {
        return INCCORECT_INSERTION;
    }
    
    if(IsEmptyList(list)) {
        /// For empty list only
        /// physical_index turn into 0 if physical_index == list->capacity
        /// Rework
        physical_index                      -= physical_index * (physical_index == list->capacity); 
        indx_t to_insert_index               = physical_index + 1;

        list->elements[to_insert_index].data = value;
        list->elements[to_insert_index].next = 0;
        list->elements[to_insert_index].prev = 0;

        list->free                           = to_insert_index + 1; /// FREE = SPACE + 2, I just think that it should be easier to understand
        list->tail                           = to_insert_index;
        list->head                           = to_insert_index;

        list->size++;
        
        ON_DEBUG(
            list->hash                       = MurmurHash((char*)list, sizeof(List) - sizeof(hash_t));
        )

        return 0;
    }
    
    
    if(list->elements[physical_index].data == Empty_elem_sign) {
        __PRINT_ALL_INFO__("Inccorect physical index, you cannot push new element after element with this physical index, because "
                           "this element is empty. Physical index is %zd\n", physical_index);
        return INCCORECT_INSERTION;
    }

    if(list->capacity == list->size) {
        __PRINT_ALL_INFO__("List oveflow\n");
        return INCCORECT_INSERTION;
    }    

    
    if(list->elements[physical_index].next != 0) {                                   /// NEXT(SPACE) != 0
        indx_t next_elem_index               = list->elements[physical_index].next;
        list->elements[next_elem_index].prev = list->free;                           /// PREV(NEXT(SPACE)) = FREE
    } else {
        list->tail                           = list->free;
    }
    
    indx_t current_free                      = list->free;                           /// REWORK
    indx_t next_free                         = list->elements[list->free].next;    
    list->elements[list->free    ].prev      = physical_index;
    list->elements[list->free    ].next      = list->elements[physical_index].next;
    list->elements[physical_index].next      = list->free;
    list->elements[list->free    ].data      = value;    
    list->free                               = next_free;

    list->size++;

    ON_DEBUG(
        list->hash                           = MurmurHash((char*)list, sizeof(List) - sizeof(hash_t));
    )


    return current_free;
}


#define ADD_VERIFY_CONDITION(condition, ...)                        \
if(condition) {                                                     \
    __PRINT_ALL_INFO__(__VA_ARGS__);                                \
    return VERIFICATION_ERROR;                                      \
}


#define CHECK_CONNECTIONS(list, start_check_from_this_index, num_of_iteration_started, additional_condition)      \
while (list->elements[start_check_from_this_index].next != 0)                                                     \
{                                                                                                                 \
    ++num_of_iteration_started;                                                                                   \
    if(start_check_from_this_index != list->elements[list->elements[start_check_from_this_index].next].prev) {    \
        __PRINT_ALL_INFO__("Inccorect bounds. Watch the elements with indexes %d and %d\n",                       \
                           start_check_from_this_index, list->elements[start_check_from_this_index].next);        \
        return VERIFICATION_ERROR;                                                                                \
    }                                                                                                             \
    additional_condition                                                                                          \
    start_check_from_this_index = list->elements[start_check_from_this_index].next;                               \
                                                                                                                  \
}                                                                                                                 \
++num_of_iteration_started;


int VerifyList(List* list)
{
    ADD_VERIFY_CONDITION(list == nullptr,                             "Error: nullptr list\n");

    /// Check hash
    ON_DEBUG(
        ADD_VERIFY_CONDITION(list->hash != MurmurHash((char*)list, sizeof(List) - sizeof(hash_t)),              /// Condition
                             "Corrupted list, the hash was changed: hash value should be %zd, but he is %zd\n", /// error message format
                             MurmurHash((char*)list, sizeof(List) - sizeof(hash_t)), list->hash);               /// error message format arguments

        //if(list->hash != MurmurHash((char*)list, sizeof(List) - sizeof(hash_t))) {
            //__PRINT_ALL_INFO__("Corrupted list, the hash was changed: hash value should be %zd, but he is %zd\n",
              //                 MurmurHash((char*)list, sizeof(List) - sizeof(hash_t)), list->hash);
            //return VERIFICATION_ERROR;
        //}
    )

    ADD_VERIFY_CONDITION(list->capacity < 8,                          "Error: capacity below zero\n");
    ADD_VERIFY_CONDITION(list->size < 0,                              "Error: size below zero\n");
    ADD_VERIFY_CONDITION(list->tail == list->head && list->size > 1,  "Error: tail cannot coincide with head\n");

    ///In last condition, I wrote list->size != 1, this is because when in list only one element tail and head coinciding, in empty
    /// list also

    ADD_VERIFY_CONDITION(list->free  < 0,                             "Error: negative free\n");
    ADD_VERIFY_CONDITION(list->head  < 0,                             "Error: negative free\n");
    ADD_VERIFY_CONDITION(list->tail  < 0,                             "Error: negative free\n");


    /// Check prev/next bound for free and ocupied elements

    ON_DEBUG(
            indx_t free_elem_index      = list->free;
            indx_t occupied_elem_index  = list->head;
            indx_t num_of_free_elem     = 0;
            indx_t num_of_occupied_elem = 0;
            if(list->size > 1) {
                CHECK_CONNECTIONS(list, free_elem_index, num_of_free_elem, if(list->elements[free_elem_index].data != Empty_elem_sign) {
                                                                                __PRINT_ALL_INFO__("Error: the element with this index %zd "
                                                                                                   "should be free\n", free_elem_index);
                                                                                return VERIFICATION_ERROR; 
                                                                            }
                )

                
                if(num_of_free_elem != list->capacity - list->size) {
                    __PRINT_ALL_INFO__("Error: number of free element is %zd, but the difference between capacity and size "
                                       "(capacity - size) is %zd\n", num_of_free_elem, list->capacity - list->size);
                }

                CHECK_CONNECTIONS(list, occupied_elem_index, num_of_occupied_elem, if(list->elements[occupied_elem_index].data == Empty_elem_sign) {
                                                                                         __PRINT_ALL_INFO__("Error: the element with this index %zd "
                                                                                                             "do not should be free\n", occupied_elem_index);
                                                                                          return VERIFICATION_ERROR; 
                                                                                    }
                )
                
                if(list->size > 1)  {         /// Useless to check bounds if list is free or only one object in list
                    ADD_VERIFY_CONDITION(num_of_occupied_elem != list->size, 
                                         "Inccorect size or number of occupied elements is inccorect. "
                                         "Size is %zd, number of occupied elements is %zd\n", 
                                         list->size, num_of_occupied_elem);
        
                    ADD_VERIFY_CONDITION(occupied_elem_index  != list->tail, "Inccorect value of tail. It's isn't the last element in the list\n");
                }
            }

            )

    return 0;

}


void RemoveFromList(List* list, ssize_t physical_index)
{
    if(VerifyList(list)) {
        return;
    }
    if(IsEmptyList(list)) {
        __PRINT_ALL_INFO__("Cannot remove element, because the list is empty\n");
        return;
    }

    if(physical_index  <= 0 || physical_index > list->capacity) {
        __PRINT_ALL_INFO__("Inccorect physical index - he should be in range (%d, %d]", 0, list->capacity);
        return;
    }

    if(list->elements[physical_index].data == Empty_elem_sign) {
        __PRINT_ALL_INFO__("Error: you want to remove empty object in list\n");
        return;
    }

    if(list->elements[physical_index].next != 0) {
        indx_t next                     = list->elements[physical_index].next;
        list->elements[next].prev       = list->elements[physical_index].prev; /// PREV(NEXT) = PREV(SPACE)
    }

    if(list->elements[physical_index].prev != 0) {
        indx_t prev                     = list->elements[physical_index].prev;
        list->elements[prev].next       = list->elements[physical_index].next; ///  NEXT(PREV) = NEXT(SPACE)
    }

    list->elements[physical_index].data = Empty_elem_sign;
    list->elements[physical_index].next = list->free;
    list->elements[list->free    ].prev = physical_index;
    list->free                          = physical_index;

    list->size--;

    ON_DEBUG(
        list->hash                      = MurmurHash((char*)list, sizeof(List) - sizeof(hash_t));
    )
}


void ListGraphicalDump(List* to_dump)
{
    static int num_of_calling = 0;
    num_of_calling++;

    if(to_dump == nullptr) {
        __PRINT_ALL_INFO__("Cannot dump a nullptr object\n");
        return;
    }
    if(to_dump->hash != MurmurHash((char*)to_dump, sizeof(List) - sizeof(hash_t))) {
        __PRINT_ALL_INFO__("Warning: the output can be inccorect, because hash was changed\n");
    }
    

    char file_name[128] = {};

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
                                              "fillcolor = \"magenta\", label = \" index  | {<index> 0 }  | { {<prev> prev | %zd } | { data  | %d } | "
                                              "{<next> next | %zd } }\" ]\n", 
                                              num_of_calling,  to_dump->elements[0].prev, to_dump->elements[0].data, to_dump->elements[0].next);

    fprintf(to_gener_graphviz_code_for_debug,"info_tab_%d [shape = record, fontname = Helvetica, style = \"filled\", fillcolor = \"magenta\", "
                                             "label = \" <info> info  | { {<tail> tail | %zd } | {<head> head | %zd } | {<free> free | %zd } | {size | %zd } | {capacity | %zd} }\" ]",
                                              num_of_calling,  to_dump->tail,    to_dump->head,    to_dump->free, 
                                              to_dump->size,   to_dump->capacity);
    
    fprintf(to_gener_graphviz_code_for_debug, "info_tab_%d : info -> dot_%zd_%d : index[style = \"invis\"];\n"
                                              "info_tab_%d : info -> dot_%zd_%d : index[style = \"invis\"];\n"
                                              "info_tab_%d : info -> dot_%zd_%d : index[style = \"invis\"];\n"
                                              "info_tab_%d : tail -> dot_%zd_%d : index;\n"
                                              "info_tab_%d : head -> dot_%zd_%d : index;\n"
                                              "info_tab_%d : free -> dot_%zd_%d : index;\n",
                                              num_of_calling, abs(to_dump->free), num_of_calling,
                                              num_of_calling, abs(to_dump->free), num_of_calling,
                                              num_of_calling, abs(to_dump->free), num_of_calling,
                                              num_of_calling, abs(to_dump->tail), num_of_calling,
                                              num_of_calling, abs(to_dump->head), num_of_calling,
                                              num_of_calling, abs(to_dump->free), num_of_calling);
    
    for(ssize_t iter_count = 1; iter_count <= to_dump->capacity; ++iter_count)
    {
        if(to_dump->elements[iter_count].data == Empty_elem_sign) {
            current_color = (char*)green;
        } else {
            current_color = (char*)red;
        }

        fprintf(to_gener_graphviz_code_for_debug, "dot_%zd_%d [shape = record, fontname = Helvetica, style = \"filled\", "
                                                  "fillcolor = \"%s\", label = \" index  | {<index> %zd }  | { {<prev> prev | %zd } | { data  | %d } | "
                                                  "{<next> next | %zd } }\" ]\n", 
                                                  iter_count,   num_of_calling,            current_color,   
                                                  iter_count,   to_dump->elements[iter_count].prev, to_dump->elements[iter_count].data, to_dump->elements[iter_count].next); 

        fprintf(to_gener_graphviz_code_for_debug, "dot_%zd_%d: index -> dot_%zd_%d: index[style = \"invis\"];\n"
                                                  "dot_%zd_%d: index -> dot_%zd_%d: index[style = \"invis\"];\n"
                                                  "dot_%zd_%d: next  -> dot_%zd_%d: index;\n"
                                                  "dot_%zd_%d: prev  -> dot_%zd_%d: index;\n", 
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

    sprintf(file_name, "dot -Tpng %s -o Debug/image_%d.png", file_name, num_of_calling);
    system(file_name);
    
}

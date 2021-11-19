#include "akinator.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define NULLPTR_TREE 1

int TreeCtor(Tree* tree, int num_of_branches)
{
    //int ctor_errors = VerifyTree(tree, VERIFICATION_IN_CTOR);
    if(ctor_errors) {
        return ctor_errors;
    }    

}

#define ADD_ERROR_VERIFICATION(error_condition)                 \
verification_number++;                                          \
if(error_condition) {                                           \
    tree_errors |= var_to_write_errors_in_tree_errors;          \
    var_to_write_errors_in_tree_errors >> verification_number;  \
}

ON_DEBUG(
int VerifyTree(Tree* tree)  
{
    int var_to_write_errors_in_tree_errors = 1;
    int verification_number                = 0;

    if(tree == nullptr) {
	tree_errors 
        return 
    }
}

void TreeErrorsProccessing(Tree* tree)
{

    for(int bits_number = 0; bits_number < offset + number_of_errors; ++bits_number)
    {
        if((tree->tree_errors << bits_number) & 0x1) {               /// check first bit
            BitNumberToError(bits_number);
        }
        
    }
}

)

#define ADD_BIT_TO_SWITCH_ERROR(bit_number, ...)      \
case bit_number: {                                    \
    __PRINT_ALL_INFO__(__VA_ARGS__);                  \
}

ON_DEBUG(

static void BitNumberToError(int bit_number)
{
    switch(bit_number)
    {

        ///+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        ADD_BIT_TO_STORE_ERROR(0, "Fatal error: nullptr object cannot used in this function\n");
        /// Never use first bit to store error, it's only for nullptr-
              
        default: {
            __PRINT_ALL_INFO__("Cannot indentify error with this bit number - %d", bit_number);
        }
    }
}

)

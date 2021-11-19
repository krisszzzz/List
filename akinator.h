#include "log.h"
#define BINARY_TREE
#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(code) code
#else
#define ON_DEBUG(...)  
#endif

struct Tree_node
{
    char*      label;
    int        vertex_degree;
    Tree_node* next_nodes;
};


struct Tree
{
    Tree_node* nodes;
    int        num_of_branches;
    ON_DEBUG(
        int    tree_errors;
    )
};

int TreeCtor(Tree* tree, int num_of_branches);
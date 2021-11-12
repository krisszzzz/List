#include "list.cpp"

//List testunit = {};
//VerifyList(&testunit);


int main()
{
//    FILE* to_gener_graphviz_code_for_debug = fopen("gen.graphviz_code%d.gv", "w");
    
    List test = {};
    ListInit(&test, 10);
    InsertToList(&test, 10, 10);
    ListGraphicalDump(&test);
    //InsertToList(&test, 1, 15);
    //ListGraphicalDump(&test);
    //InsertToList(&test, 1, 15);
    VerifyList(&test);
    printf("SHIT");


    

}
#include <stdio.h>
#include "list.h"


int main()
{

    List test = {0};
    ListInit(&test, 10);

    PushFront(&test, 10);
    ListDtor(&test);
    // PushFront(test, 10);
    // PushFront(test, 10);

    // PushFront(test, 10);
    // RemoveFromList(test, 1);
    // RemoveFromList(test, 2);
    

    printf("physical index = %d, logical index = %d\n", ConvertLogicalToPhysicalIndex(&test, 4), 4);
    //ListGraphicalDump(&test);

    //PushFront(test, 10);   
    
    printf("Is sorted - %d\n", test.is_sorted);

    
    //RemoveFromList(&test, 5);

    ListGraphicalDump(&test);    
    VerifyList(&test);
    

}
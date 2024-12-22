#ifndef LIST_H
#define LIST_H

typedef struct Node Node;
typedef struct List List;

//functions
List *ListCreate();
int ListCount(List* list);
void *ListFirst(List* list);
void *ListLast(List* list);
void *ListNext(List* list);
void *ListPrev(List* list);
void *ListCurr(List* list);
int ListAdd(List* list, void* item);
int ListInsert(List* list, void* item);
int ListAppend(List* list, void* item);
int ListPrepend(List* list, void* item);
void *ListRemove(List* list);
void ListFree(List* list, void (*itemFree)(void*));
void *ListTrim(List* list);
void ListConcat(List* list1, List* list2);
void *ListSearch(List* list, int (*comparator)(void*, void*) , void* comparisonArg);
// Non Assignment Functions
void printState(int* results);

#endif
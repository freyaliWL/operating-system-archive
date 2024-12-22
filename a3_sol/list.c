#include "list.h"
#include <stdbool.h>
#include <stdio.h> 

#define nodeLimit 1000
#define listHeadLimit 10

// Variables so meeting node and head contrainsts are easy calculate
int currentTotalHeads = 0;
int currentTotalNodes = 0;

// Variables need to populate the node array on first run of ListCreate()
bool firstRun = true;
Node *NextEmptyNode;

// Declare the structs
struct Node {
    void* item;
    Node* next;
    Node* prev;
};

struct List {
    Node* head;
    Node* tail;
    Node* current;
    int indexOfCurrentNode;
    bool isActive;
    unsigned int indexOfListHeadsArray;
    int count;
};

// Define our static arrays
List globalArrayOfListHeads[listHeadLimit];
Node globalArrayOfNodes[nodeLimit];

///////////////////////////////////////////////////////////////////////////////////////
// helper functions
//////////////////////////////////////////////////////////////////////////////////////

// prints the golbal state for debugging purposes
void printState(int* results){
    // Assumption: items are only integers in the list for testing purposes
    // printf("Printing the entire state\n\n");
    printf("Number of Lists: %d\n", currentTotalHeads);
    printf("Number of Nodes: %d\n", currentTotalNodes);
    
    List list;
    Node* currentNode;
    
    for (int i = 0; i < currentTotalHeads; i++ ){
        printf("Linked List %d: [", i);
        list = globalArrayOfListHeads[i];
        currentNode = globalArrayOfListHeads[i].head;
        while (currentNode != NULL){
            if(currentNode->next == NULL){
                printf(" %d", *(int *)currentNode->item);
            }else{
                printf(" %d,", *(int *)currentNode->item);
            }
            currentNode = currentNode->next;
        }
        printf(" ], index of currentNode: %d, count: %d", list.indexOfCurrentNode, list.count);
        if(list.count != 0 ) printf(", current item: %d, head: %d, tail: %d\n\n", list.current ? *(int *)list.current->item : -1,  *(int *)list.head->item, *(int *)list.tail->item);
        else printf("\n\n");
    }

    results[0] = currentTotalHeads;
    results[1] = currentTotalNodes;
}

// checks if global list head limit is met
bool headConstraintsMet(){
    if(currentTotalHeads == listHeadLimit){
        return true;
    }else{
        return false;
    }
}

// checks if global node limit is met
bool nodeConstraintsMet(){
    if(currentTotalNodes == nodeLimit){
        return true;
    }else{
        return false;
    }
}

// Handles a situation here a new node is being added to the beginning of the list
void addNewNodeToStartOfList(List* list, Node* newNodeAddress){
    newNodeAddress->next = list->head;

    if(list->count > 0) list->head->prev = newNodeAddress;

    list->current = newNodeAddress;
    list->head = newNodeAddress;

    //check if list is empty and handle accordingly
    if(list->count == 0) list->tail = newNodeAddress;

    list->count += 1;
    list->indexOfCurrentNode = 0;
}

// Handles a situation here a new node is being added to the end of the list
void addNewNodeToEndOfList(List* list, Node* newNodeAddress){
    newNodeAddress->prev = list->tail;

    if(list->count > 0) {
        list->tail->next = newNodeAddress;
    }

    list->current = newNodeAddress;
    list->tail = newNodeAddress;

    if(list->count == 0){
        list->head = newNodeAddress;
    }

    list->count += 1;
    list->indexOfCurrentNode = list->count - 1;
}

// Function for handling a case where the node is remove from a list so then it becomes available again.
void RemoveNodeFromGlobalArray(Node* currentNode){
    Node* currentNextEmptyNode = NextEmptyNode;
    currentNode->prev = NULL;
    currentNode->next = currentNextEmptyNode;
    currentNode->item = NULL;
    NextEmptyNode = currentNode;
    currentTotalNodes -=1;
}

// Removes a list from the global array
void RemoveListFromGlobalArray(List* array, List* listToRemove, int* count){
    int indexOfCurrent = listToRemove->indexOfListHeadsArray;
    int indexOfLast = *count - 1;
    array[indexOfCurrent] = array[indexOfLast];
    array[indexOfCurrent].indexOfListHeadsArray = indexOfCurrent;
    *count -= 1;
}

// Takes the next availabale node from our static array and initializes it with our item.
Node *addNewNodeFromItem(void *item){
    
    // structure of new Node NULL <- Node -> NULL. Node.prev and Node.next is set by functions that add items to a List
    Node* newNode = NextEmptyNode;
    NextEmptyNode = newNode->next;
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->item = item;
    currentTotalNodes +=1;
    return newNode;
}

///////////////////////////////////////////////////////////////////////////////////////
//external functions
///////////////////////////////////////////////////////////////////////////////////////

// Function for creating a statically allocated linked list. Returns a pointer to that list
List *ListCreate(){
    // if this the first run of this function
    if(firstRun){
        // populated the static node array with empty nodes
        // each node points to the next empty node where the 0th position is labeled as the first "NextEmptyNode"
        // After initialization firstRun is set to "false"
        for (int i = 0; i < nodeLimit; i++){
            Node newNode;
            globalArrayOfNodes[i] = newNode;
            if (i > 0) globalArrayOfNodes[i-1].next = &globalArrayOfNodes[i];
        }
        NextEmptyNode = &globalArrayOfNodes[0];
        firstRun = false;

    }
    // if all heads are exhausted
    if(headConstraintsMet()){
        return NULL;
    }else{
        // if not, initialize a new List and return a pointer
        // newList.indexOfCurrentNode = -1; is the index of the current item in the array starting from zero. This is used to check if curent item is before or after the List
        // newList.isActive = true; is for setting the status of list in case a user was to malicously use the list
        List newList;
        newList.head = NULL;
        newList.tail = NULL;
        newList.current = NULL;
        newList.count = 0;
        newList.indexOfCurrentNode = -1;
        newList.indexOfListHeadsArray = currentTotalHeads;
        newList.isActive = true;

        globalArrayOfListHeads[currentTotalHeads] = newList;
 
        return &globalArrayOfListHeads[currentTotalHeads++];
    }
}

int ListCount(List* list){
    return list->count;
}

void *ListFirst(List* list){
    if(list->count == 0){
        return NULL;
    }
    list->current = list->head;
    list->indexOfCurrentNode = 0;
    return list->current->item;
}

void *ListLast(List* list){
    if(list->count == 0){
        return NULL;
    }
    list->current = list->tail;
    list->indexOfCurrentNode = list->count -1;
    return list->current->item;
}

/*




*/
void *ListNext(List* list){
    //Handling Empty List
    if(list->count == 0){
        return NULL;
    //If list is not empty
    }else{
        // If current node is not null current is not beyond or before the list
        if(list->current){
            list->current = list->current->next;
            list->indexOfCurrentNode+= 1;
            return list->current ? list->current->item : NULL;
        // current item is before or beyond the list
        }else{
            //If we are before make current head else make current NULL
            list->current = list->indexOfCurrentNode < 0 ? list->head : NULL;
            // if we change list->current from NULL to head that means we increment indexOfCurrentNode
            list->indexOfCurrentNode = list->current ? list->indexOfCurrentNode + 1 : list->indexOfCurrentNode;
            // Again if we change current from NULL to head we return head item else we return NULL;
            return list->current ? list->current->item : NULL;
        }
    }
}

// Same logic as ListNext but logic is inversed.
void *ListPrev(List* list){
    if(list->count == 0){
        return NULL;
    }else{
        
        if(list->current){
            list->current = list->current->prev;
            list->indexOfCurrentNode-= 1;
            return list->current ? list->current->item : NULL;
        }else{
            list->current = list->indexOfCurrentNode > list->count - 1 ? list->tail : NULL;
            list->indexOfCurrentNode = list->current ? list->indexOfCurrentNode - 1 : list->indexOfCurrentNode;
            return list->current ? list->current->item : NULL;
        }
    }
}

void *ListCurr(List* list){
    return list->current ? list->current->item : NULL;
}

int ListAdd(List* list, void* item){
    //return -1 if we have reached the node limit
    if(nodeConstraintsMet()){
        return -1;
    }else{
        // else pop a new node from the node array and initialize with item
        Node* newNodeAddress = addNewNodeFromItem(item);

        // Check if current item is before or after the list and handle accordingly
        if (list->indexOfCurrentNode < 0 ){
            addNewNodeToStartOfList(list, newNodeAddress);
        }else if (list->indexOfCurrentNode >= list->count - 1){
            addNewNodeToEndOfList(list, newNodeAddress);
        }else{
            // if we are in the middle of the list
            if(list->count > 0){
                newNodeAddress->next = list->current->next;
                newNodeAddress->prev = list->current;
                list->current->next = newNodeAddress;
            } else{
                list->head = newNodeAddress;
                list->tail = newNodeAddress;
            }

            list->current = newNodeAddress;
            list->indexOfCurrentNode += 1; 
            list->count += 1;
        }
        return 0;
    }    
}

int ListInsert(List* list, void* item){
    if(nodeConstraintsMet()){
        return -1;
    }else{
        Node* newNodeAddress = addNewNodeFromItem(item);
        if (list->indexOfCurrentNode <= 0 ){
            addNewNodeToStartOfList(list, newNodeAddress);
        }else if (list->indexOfCurrentNode > list->count - 1){
            addNewNodeToEndOfList(list, newNodeAddress);
        }else{

            if(list->count > 0){
                newNodeAddress->next = list->current;
                newNodeAddress->prev = list->current->prev;
                if(list->current->prev) list->current->prev->next = newNodeAddress;
            } else{
                list->head = newNodeAddress;
                list->tail = newNodeAddress;
            }

            list->current = newNodeAddress;
            list->count += 1;
        }
        return 0;
    }    
}

int ListAppend(List* list, void* item){
    if(nodeConstraintsMet()){
        return -1;
    }else{
        Node* newNodeAddress = addNewNodeFromItem(item);
        addNewNodeToEndOfList(list, newNodeAddress);
        return 0;    
    }    
}

int ListPrepend(List* list, void* item){
    if(nodeConstraintsMet()){
        return -1;
    }else{
        Node* newNodeAddress = addNewNodeFromItem(item);
        addNewNodeToStartOfList(list, newNodeAddress);
        return 0;    
    }    
}

void ListConcat(List* list1, List* list2) {
    list1->count = list1->count + list2->count;
    list1->tail->next = list2->head;
    list1->tail = list2->tail;
    list2->isActive= false;
    RemoveListFromGlobalArray(globalArrayOfListHeads, list2 , &currentTotalHeads);
}

void *ListRemove(List* list){

    // This also handles the case when the list is empty since list->indexOfCurrentNode is initialized to -1
    if(list->indexOfCurrentNode < 0 || list->indexOfCurrentNode > list->count - 1) return NULL;

    void *itemToReturn = list->current->item;
    Node* nodeToRemove = list->current;

    if(list->current->prev){
        list->current->prev->next = list->current->next;
    }else{
        list->head = list->count > 1 ? list->head->next : NULL;
    }

    if(list->current->next){
        list->current->next->prev = list->current->prev;
    }else{
        list->tail = list->count > 1 ? list->tail->prev : NULL;
    }

    list->current = list->current->next;
    RemoveNodeFromGlobalArray(nodeToRemove);

    list->count -= 1;
    list->indexOfCurrentNode = list->count == 0 ? list->indexOfCurrentNode -1 : list->indexOfCurrentNode;
    return itemToReturn;
}

void ListFree(List* list, void (*itemFree)(void*)){
    if(list->isActive){
        Node*  currentNode = list->head;
        int index;
        while(currentNode != NULL){
            (*itemFree)(currentNode->item);
            Node* nodeToRemove =  currentNode;
            currentNode = currentNode->next;
            RemoveNodeFromGlobalArray(nodeToRemove);
        }
        RemoveListFromGlobalArray(globalArrayOfListHeads, list , &currentTotalHeads);
    }
    list->isActive = false;
    
}

void *ListTrim(List* list){

    // Return NULL pointer if the array index is out of bounds
    if(list->count == 0) return NULL;

    void *itemToReturn = list->tail->item;
    Node* nodeToRemove = list->tail;

    if(list->tail->prev){
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    }else{
        list->tail = NULL;
        list->head = NULL;
    }

    RemoveNodeFromGlobalArray(nodeToRemove);

    list->current = list->tail;
    list->count -= 1;
    list->indexOfCurrentNode = list->count - 1;
    return itemToReturn;
}

void *ListSearch(List* list, int (*comparator)(void*, void*) , void* comparisonArg){
    Node* curNode = list->current;
    while(curNode != NULL){
        if (comparator(curNode->item, comparisonArg) == 1) {
            list->current = curNode;
            return curNode->item;
        }
        curNode = curNode->next;
    }
    return NULL;
}
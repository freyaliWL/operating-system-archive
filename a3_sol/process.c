// Declare the PCB structure for the PCB for each process
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "list.h"
#include "process.h"
#include <string.h>
/*
    ############################################
    Structures and Globals
    ############################################
*/

int pidCounter = 0;

struct Semaphore {
    List* Queue;
    int value;
    bool created;
};

int percentageCalculator[100];
// Declare the Process Structure
struct Process {
    int id;
    int timesScheduled;
    bool waitRecieve;
    bool waitReply;
    bool hasRecieved;
    int idOfSem;
    char* procMessage;
    int sourceOfMessage;
    int priority;
};

// Declare structure to hold the entire state
struct State {
    List* highPriorityQueue;
    List* normPriorityQueue;
    List* lowPriorityQueue;
    List* blockedProcesses;
    Process* init;
    Process* currentProcess;
    Semaphore sem1;
    Semaphore sem2;
    Semaphore sem3;
    Semaphore sem4;
};

//initalize global state;
State state;

/*
    ############################################
    Helper Functions
    ############################################
*/
// returns a semapgore given the id
Semaphore* returnSemaphoregivenId(State* state, int id){
    if(id == 1 ){
        return &state->sem1;
    }else if(id == 2){
        return &state->sem2;
    }else if(id == 3){
        return &state->sem3;
    }else if(id == 4){
        return &state->sem4;
    }
}

// Prints procMessages and reinitializes
void printMessage(Process* p){
    printf("Message from process with id %d: %s\n",  p->sourceOfMessage,  p->procMessage);
    p->procMessage = "";
}

// Creates the init process
Process* createInitProcess(){
    // Create the init process. The init process is it's own member variabel in the global state because it is special. ID of init will always be 0. Remember to de-allocate when terminating.
    Process* initProcess = malloc(sizeof(Process));
    *initProcess = (struct Process){.id = pidCounter};
    pidCounter=pidCounter + 1;
    return initProcess;
}

// Return the next process to schedule. This is where the logic to choose the next process will be encapsulated
Process* chooseNextProcess(State* state){
    List* arrayOfLists[3] = {state->highPriorityQueue, state->normPriorityQueue, state->lowPriorityQueue};
    for(int i = 0; i< 3; i++){
        if(ListCount(arrayOfLists[i])!= 0){
            Process* item;
            void* result = ListTrim(arrayOfLists[i]);
            item = (Process*)result;
            return item;
        }
    }
}

// This function adds a process to the correct queue given the Process and it's priority
void addProcessToCorrectQueue(State* state, Process* p, char* source){
    int returnValue;
    Process* item;
    void* result;

    if (p->priority == 0){
        returnValue = ListPrepend(state->highPriorityQueue, p);
    }else if(p->priority == 1){
        returnValue = ListPrepend(state->normPriorityQueue, p);
    }else if (p->priority == 2){
        returnValue = ListPrepend(state->lowPriorityQueue, p);
    }





    // We do error handling in this function because this function is called in multiple other places. Kill lots of birds with one stone.
    if(returnValue < 0){
        printf("Failed: No more room to add more processes to ready queue.\n");
    }else{
        printf("Success: Added process with id: %d to ready queue. (%s)\n", p->id, source);
    }
}

// This function moves a process up in priority 
void moveCurrentProcessUpInPriority(State* state, Process* p){
    int currentPriority = p->priority;

    if(currentPriority == 0){
        // if process is already in high priority queue return.
        return;
    }else if (currentPriority == 1){
        // if process is in norm priority queue add to high priority Queue.chooseNextProcess
    }
    addProcessToCorrectQueue(state, p, "Quantam Expired");

}

// This function moves a process down in priority 
void moveCurrentProcessDownInPriority(State* state, Process* p){
    int currentPriority = p->priority;

    if(currentPriority == 2){
        // if process is already in low priority queue return.
        return;
    }else if (currentPriority == 1){
        // if process is in norm priority Queue add to low priority queue.
        p->priority++;
    }else if (currentPriority == 0){
        // if process is in high priority Queue add to norm priority queue.
        p->priority++;
    }
    addProcessToCorrectQueue(state, p, "Quantam Expired");
}

// When switching process this function encapsulates the logic that decides what queue the current process will be added too (not when a process is killed or exited).
void handleCurrentProcess(State* state, Process* p){
    
    int timesScheduled = p->timesScheduled ++;

    if (timesScheduled < 1 ) {
        // if process has been sceduled less than 5 times leave alone done change priority
        addProcessToCorrectQueue(state, p, "Quantam Expired");
        return;
    }else if (timesScheduled <= 3){
        // if process has been sceduled less or equal to 20 but greater than 5 times, lower the priority of the process.
        moveCurrentProcessDownInPriority(state, p);
    }else if (timesScheduled > 7){
        // if process has been sceduled greater than 20 but greater than 5 times, increase the priority of the process.
        moveCurrentProcessUpInPriority(state, p);
    }
}

// This function will be run when we need to scedule the next process.
void schedule(State* state){

    // check to see if there is a process waiting on any queue.
    if(ListCount(state->highPriorityQueue) == 0 && ListCount(state->normPriorityQueue) == 0 && ListCount(state->lowPriorityQueue) == 0){
        // if there is not, make the init process the current process.
        state->currentProcess = state->init;
        printf("\n");
        printf("*************************************\n");
        printf("Current Process has changed! Running now is the init process.\n");
        printf("*************************************\n");
        printf("\n");
    }else{

        Process* currentProcess = state->currentProcess;

        // Choose the next process
        state->currentProcess = chooseNextProcess(state);
        printf("\n");
        printf("*************************************\n");
        printf("Current Process has changed! Running now is process with id: %d\n", state->currentProcess->id);

        // if the process was blocked because it recieving display the message
        if(state->currentProcess->procMessage != "" && state->currentProcess->waitRecieve){
            printMessage(state->currentProcess);
            state->currentProcess->waitRecieve = false;
            state->currentProcess->hasRecieved = true;
        }

        // if the process was blocked because it sending display the message
        if(state->currentProcess->procMessage != "" && state->currentProcess->waitReply){
            printMessage(state->currentProcess);
            state->currentProcess->waitReply = false;
        }

        printf("*************************************\n");
        printf("\n");
        // this means the currentProcess was not exited or killed (ie. time quantam expired). Why: I set currentProcess to NULL when exiting or killing it.
        if(currentProcess && currentProcess != state->init) handleCurrentProcess(state, currentProcess);
        
    }

}

// comarator function used for ListSearch
int comparator(void* process, void* id){
    Process* processFromList = (Process *)process;
    int itemToReturn = processFromList->id == *(int*) id ? 1: 0; 
    return itemToReturn;
}

// Return true if id is in the specified list
bool searchForProcess(List* list, int id){
    ListFirst(list);
    void* result = ListSearch(list, comparator, &id);
    Process * item = (Process *) result;
    
    if (item != NULL) {
        return true;
    }
    else return false;
}

// helper to print out the list in a nice format
void displayList(List* q, char* name){
    void* voidProcess;
    Process* process;
    printf("%s", name);
    int count = ListCount(q);
    if (count == 0){
        printf("{Nothing}\n");
    }else{
        printf("{ ");
        voidProcess = ListFirst(q);
        Process* process =(Process *) voidProcess;
        printf("Process with id: %d ", process->id);
        for(int i =1; i <count; i++){
             voidProcess = ListNext(q);
             process =(Process *) voidProcess;
             printf(", Process with id: %d ", process->id);
        }
        printf("}\n");
    }
    return;
}

// search for a process accross all queues
Process* searchAllForProcess(State* state, int id){
    List* AllQueues[4] = {state->highPriorityQueue, state->normPriorityQueue, state->lowPriorityQueue, state->blockedProcesses};
    void* voidp;
    Process* p;
    for(int i = 0; i< 4; i++){
        if(ListCount(AllQueues[i]) != 0){
            if( searchForProcess( AllQueues[i], id) ){
                voidp = ListCurr(AllQueues[i]);
                p = (Process*) voidp;
                return p;
            }

        }
    }
    return NULL;
}

// check to see if a semaphore has been created or not
bool semaphoreinUse(State* state,int id){
    if(id == 1 ){
        return state->sem1.created == true;
    }else if(id == 2){
        return state->sem2.created == true;
    }else if(id == 3){
        return state->sem3.created == true;
    }else if(id == 4){
        return state->sem4.created == true;
    }
}

// funtion to create a semaphores
void createSemaphore(State* state,int id, int init){
    if(id == 1 ){
        state->sem1.created = true;
        state->sem1.Queue = ListCreate();
        state->sem1.value = init;
    }else if(id == 2){
        state->sem2.created = true;
        state->sem2.Queue = ListCreate();
        state->sem2.value = init;
    }else if(id == 3){
        state->sem3.created = true;
        state->sem3.Queue = ListCreate();
        state->sem3.value = init;
    }else if(id == 4){
        state->sem4.created = true;
        state->sem4.Queue = ListCreate();
        state->sem4.value = init;
    }
}
/*
    ############################################
    Entrypoint Functions
    ie. first functions called when user provides input
    ############################################
*/

// Entrypoint to display info for a given process id.
void processInfo(State* state){
    //get id from user
    int parameter;
    printf("You have chosen to call info for a specific process. Please enter the id of the process\n");
    scanf("%d", &parameter);

    // put all queues that are not queues related to semaphores in a list
    List* AllQueues[4] = {state->highPriorityQueue, state->normPriorityQueue, state->lowPriorityQueue, state->blockedProcesses};
    char* name;
    int count;
    void * voidProcess;
    Process* process;
    for(int i=0; i<4; i++){
        if(searchForProcess(AllQueues[i], parameter)){
            // initialize messages to each queue
            if(i == 0) name = "High Priority Queue";
            if(i == 1) name = "Normal Priority Queue";
            if(i == 2) name = "Low Priority Queue";
            if(i == 3) name = "Blocked by Send Recieve";
            if (i < 3){
                // if there are in a priority queue and not blocked we care about the position so we search for the position
                count = ListCount(AllQueues[i]);
                voidProcess = ListFirst(AllQueues[i]);
                Process* process =(Process *) voidProcess;
                if(process->id == parameter){
                    printf("Process with id %d is in the %s in position %d", parameter,name, 1);
                    return;
                }
                for(int j =1; j <count; j++){
                    if(process->id == parameter){
                        printf("Process with id %d is in the %s in position %d", parameter,name, j-1);
                        return;
                    }
                    voidProcess = ListNext(AllQueues[j]);
                    process =(Process *) voidProcess;
                }
            }else{
                // if they are blocked we don't care was position they are in
                printf("Process with id %d is blocked currently with send/recieve", parameter);
                return;
            }
        }
    }

    // do the same thing for semaphore if we didn't find in priotity queues
    Semaphore allSems[4] =  {state->sem1, state->sem2, state->sem3, state->sem4};

    for(int l = 0; l<4; l++){
        if(l == 0) name = "Process blocked on Semaphore 1"; 
        if(l == 1) name = "Process blocked on Semaphore 2";
        if(l == 2) name = "Process blocked on Semaphore 3";
        if(l == 3) name = "Process blocked on Semaphore 4";
        if(semaphoreinUse(state, l+1)){
            if(searchForProcess(allSems[l].Queue, parameter)){
                count = ListCount(allSems[l].Queue);
                voidProcess = ListFirst(allSems[l].Queue);
                Process* process =(Process *) voidProcess;
                if(process->id == parameter){
                    printf("Process with id %d is %s in position %d", parameter,name, 1);
                    return;
                }
                for(int k =1; k <count; k++){
                    printf("%d\n", parameter);
                    printf("%d\n", process->id);
                    if(process->id == parameter){
                        printf("Process with id %d is %s in position %d", parameter,name, k-1);
                        return;
                    }
                    voidProcess = ListNext(AllQueues[k]);
                    process =(Process *) voidProcess;
                }
            }
        }
    }
    printf("Failure: No process with that id available\n");
}

void p(State* state){
    
    if(state->currentProcess == state->init){
        printf("Failed: Cannot call P() from init process\n");
        return;
    }
    //get id from user
    int parameter;
    printf("You have chosen to call P() on a semaphore. Please enter the id of the semaphore(1-4).\n");
    scanf("%d", &parameter);

    // Error handling
    if(parameter > 4 || parameter < 1){
        printf("Failure: You cannot call P() on a semaphore with that id\n");
        return;
    }

    if(semaphoreinUse(state, parameter) != true){
        printf("Failure: A semaphore with that id does not exists\n");
        return;
    }
    // get the semaphore with the id provided
    Semaphore* sem = returnSemaphoregivenId(state, parameter);

    if (sem->value < 0){
        // if there is something using P, then we block
        ListPrepend(sem->Queue, state->currentProcess);
        state->currentProcess->idOfSem = parameter;
        state->currentProcess = NULL;
        schedule(state);
    }else{
        // else we don't
        sem->value = sem->value - 1;
    }
}

void v(State* state){

    if(state->currentProcess == state->init){
        printf("Failed: Cannot call V() from init process\n");
        return;
    }

    //get id from user
    int parameter;
    printf("You have chosen to call P() on a semaphore. Please enter the id of the semaphore(1-4).\n");
    scanf("%d", &parameter);

    // Error handling
    if(parameter > 4 || parameter < 1){
        printf("Failure: You cannot call P() on a semaphore with that id\n");
        return;
    }

    if(!semaphoreinUse(state, parameter)){
        printf("Failure: A semaphore with that id does not exists\n");
        return;
    }

    // get the semaphore with the id provided
    Semaphore* sem = returnSemaphoregivenId(state,parameter);

    
    if (sem->value < 0){
        // if their are process blocked on the semaphore, we give one of them access to the semaphore
        Process* ProcessToAdd = ListTrim(sem->Queue);
        ProcessToAdd->idOfSem = -1;
        addProcessToCorrectQueue(state, ProcessToAdd, "Unblocked");
    }else{
        // else we increment the semaphore
        sem->value = sem->value + 1;
    }
    
}

// Entrypoint for making a new semaphore
void newSemaphore(State* state){
    // get input from user
    int parameter;
    int initialValue;
    printf("You have chosen to create a new semaphore. Please enter the id of the semaphore(1-4).\n");
    scanf("%d", &parameter);

    if(parameter > 4 || parameter < 1){
        printf("Failure: You cannot create a semaphore with that id\n");
        return;
    }

    if(semaphoreinUse(state, parameter)){
        printf("Failure: A semaphore with that id already exists\n");
        return;
    }

    printf("What would like to inital the semaphore too?( value => 0 )\n");
    scanf("%d", &initialValue);

    if(initialValue < 0){
        printf("Failure: You cannot create a semaphore with that inital value\n");
        return;
    }

    createSemaphore(state, parameter, initialValue);
    printf("bool: %d\n", state->sem1.created);
}

// Entrypoint into the reply function
void reply(State* state){
    char* stringToSend;
    char currentChar;
    int bufferSpot = 0;
    int parameter;

    // Do some error handling
    if(state->currentProcess == state->init){
        printf("Failed: Cannot reply from init process\n");
        return;
    }

    if (!state->currentProcess->hasRecieved){
        printf("Failed: Cannot reply from process that has not recieved a message\n");
        return;
    }

    Process* processToSend;
    printf("You have chosen to reply to a process. Please enter the id of the process.\n");
    scanf("%d", &parameter);

    if(parameter == 0){
        printf("Failed: Cannot send to init process\n");
        return;
    }

    printf("Searching for process!\n");
    processToSend = searchAllForProcess(state, parameter);
    if(processToSend == NULL){
       printf("Failed: No process with that id\n");
       return;
    }

    if(!processToSend->waitReply){
       printf("Failed: Process is not waiting for a reply\n");
       return;
    }

    printf("Now please enter a message:\n");
    fflush(stdin);
    currentChar = getchar();

    // get message from the user to send
    while(1){
        currentChar = getchar();
        if(currentChar==0x0A){
            stringToSend[bufferSpot] = '\0';
            break;
        }else{
            stringToSend[bufferSpot++] = currentChar;
        }
        
    }
    // we add the sender back into the ready queue since it is no longer blocked
    addProcessToCorrectQueue(state, processToSend, "Unblocked");
    searchForProcess(state->blockedProcesses, processToSend->id);
    ListRemove(state->blockedProcesses);

    // we give the process the message to disply next time it is scheduled
    processToSend->procMessage = stringToSend;
    processToSend->sourceOfMessage = state->currentProcess->id;

}

void recieve(State* state){

    // error handling
    if(state->currentProcess == state->init){
        printf("Failed: Cannot recieve from init process\n");
        return;
    }

    if(state->currentProcess->procMessage != ""){
        // if we have't recieved a message from anyone, we block
        printMessage(state->currentProcess);
        state->currentProcess->waitRecieve = false;
        state->currentProcess->hasRecieved = true;
    }else{
        // else we display the message
        state->currentProcess->waitRecieve = true;
        ListAdd(state->blockedProcesses, state->currentProcess);
        state->currentProcess = NULL;
        schedule(state);
    }
}

void send(State* state){
    char* stringToSend;
    char currentChar;
    int bufferSpot = 0;
    int parameter;

    // error handling
    if(state->currentProcess == state->init){
        printf("Failed: Cannot send from init process\n");
        return;
    }

    Process* processToSend;
    printf("You have chosen to send a process a message. Please enter the id of the process.\n");
    scanf("%d", &parameter);

    if(parameter == 0){
        printf("Failed: Cannot send to init process\n");
        return;
    }

    printf("Searching for process!\n");
    processToSend = searchAllForProcess(state, parameter);
    if(processToSend == NULL){
       printf("Failed: No process with that id\n");
       return;
    }
    printf("Now please enter a message:\n");
    fflush(stdin);
    currentChar = getchar();
    while(1){
        currentChar = getchar();
        if(currentChar==0x0A){
            stringToSend[bufferSpot] = '\0';
            break;
        }else{
            stringToSend[bufferSpot++] = currentChar;
        }
        
    }
    if(processToSend->waitRecieve){
        addProcessToCorrectQueue(state, processToSend, "Unblocked");
        searchForProcess(state->blockedProcesses, processToSend->id);
        ListRemove(state->blockedProcesses);
    }
    state->currentProcess->waitReply = true;
    processToSend->procMessage = stringToSend;
    processToSend->sourceOfMessage = state->currentProcess->id;
    ListAdd(state->blockedProcesses, state->currentProcess);
    state->currentProcess = NULL;
    
    schedule(state);
}

// function for displaying all the info for the entire state
void totalInfo(State* state){
    List* AllQueues[4] = {state->highPriorityQueue, state->normPriorityQueue, state->lowPriorityQueue, state->blockedProcesses};
    char* name;
    printf("\n");
    printf("*************************************\n");
    printf("TOTAL INFO\n");
    printf("*************************************\n");
    for(int i = 0; i<4; i++){
        if(i == 0) name = "High Priority Queue: ";
        if(i == 1) name = "Normal Priority Queue: ";
        if(i == 2) name = "Low Priority Queue: ";
        if(i == 3) name = "Blocked by Send Recieve: ";
        displayList(AllQueues[i], name);
    }
    Semaphore allSems[4] =  {state->sem1, state->sem2, state->sem3, state->sem4};
    for(int i = 0; i<4; i++){
        if(i == 0) name = "Process blocked on Semaphore 1: "; 
        if(i == 1) name = "Process blocked on Semaphore 2: ";
        if(i == 2) name = "Process blocked on Semaphore 3: ";
        if(i == 3) name = "Process blocked on Semaphore 4: ";
        if(semaphoreinUse(state, i+1)) displayList(allSems[i].Queue, name);
    }
}

// Entrypoint to skip to the next quantum
void nextQuantum(State* state){
    schedule(state);
}

// Entrypoint function called when exiting the current process ("E" user input)
void exitProcess(State* state){
    printf("You have chosen to kill the current process\n");
    printf("Killing the current process with id: %d\n", state->currentProcess->id);
    if(state->currentProcess == state->init){
        free(state->init);
        exit(0);
    }else{
        free(state->currentProcess);
        state->currentProcess = NULL;
        schedule(state);
    }
}

// Entrypoint function called when killing a user specified process ("K" user input)
void killProcess(State* state){
    int parameter;
    printf("You have chosen to kill a Process. Please enter the id of the process.\n");
    scanf("%d", &parameter);
    if (parameter == 0){
        printf("Failed: Cannot kill the init process!\n");
        return;
    }
    if (state->currentProcess->id == parameter) exitProcess(state);
    else{
        bool inList;
        void* voidProcessToKill;
        Process* processToKill;

        List* ArrayOfList[4] = {state->highPriorityQueue, state->normPriorityQueue, state->lowPriorityQueue, state->blockedProcesses};
        for(int i = 0; i<4; i++){
            inList = searchForProcess(ArrayOfList[i], parameter);
            if (inList){
                voidProcessToKill = ListRemove(ArrayOfList[i]);
                processToKill = (Process *) voidProcessToKill;
                if(processToKill != NULL){
                    printf("Success: Kill Process with id: %d\n", processToKill->id);
                    free(processToKill);
                    return;
                }
                return;
            }
        }

        printf("Failed: No process with that id\n");
        return;
    }
}

// Entrypoint function called when forking a process ("F" user input)
void forkProcess(State* state){
    if(state->init == state->currentProcess){
        printf("Failed: You have tried to for the init process.\n");
    }else{
        Process* newProcess = malloc(sizeof(Process));
        *newProcess = (struct Process){.id = pidCounter, .timesScheduled = 0, .sourceOfMessage = -1,.waitRecieve= false, .waitReply = false, .procMessage = NULL, .priority = state->currentProcess->priority};
        pidCounter = pidCounter +1;
        addProcessToCorrectQueue(state, newProcess, "Fork");
    }
}

// Entrypoint function called when creating a new Process ("C" user input)
void createProcess(State* state){
    
    // Asked user for priority parameter and convert parameter to int.
    // TODO: verify input from user.
    int parameter;
    printf("You have chosen to create a process. Please enter a priority (0 = high, 1 = norm, 2 = low) \n");
    scanf("%d", &parameter);
    // Create a new process using malloc and initialize. Make sure the de-allocated when kill or exit.
    Process* newProcess = malloc(sizeof(Process));
    *newProcess = (struct Process){.id = pidCounter, .timesScheduled = 0, .procMessage = "", .priority = parameter, .idOfSem = -1};
    pidCounter = pidCounter + 1;

    // if the current process is the init process we can switch to the new process
    if(state->currentProcess == state->init) state->currentProcess = newProcess;
    // Add the process to appropriate queue
    else addProcessToCorrectQueue(state, newProcess, "Create");
}

// This function recieves input from the user and calls the appropriate entry point functions given the user input.
void startRecievingInput(State* state){
    char input;
    int parameter;
    while(1){
        printf("\n");
        printf("*************************************\n");
        printf("Current Process running is process with id: %d\n", state->currentProcess->id);
        printf("*************************************\n");
        printf("\n");
        printf("Please provide input: \n");
        scanf(" %c", &input);

        if (input == 0x43){
            // Create Process
            createProcess(state);
        }else if (input == 0x46){
            // Fork Process
            forkProcess(state);
        }else if (input == 0x4B){
            // Kill pocess
            killProcess(state);
        }else if (input == 0x45){
            // Exit 
            exitProcess(state);
        }else if (input == 0x51){
            // Next time quantum
            nextQuantum(state);
        }else if (input == 0x53){
            // send
            send(state);
        }else if (input == 0x52){
            // recieve
            recieve(state);
        }else if (input == 0x59){
            // reply
            reply(state);
        }else if (input == 0x4E){
            // New Semaphore
            newSemaphore(state);
        }else if (input == 0x50){
            // P
            p(state);
        }else if (input == 0x56){
            // V
            v(state);
        }else if (input == 0x54){
            // total info
            totalInfo(state);
        }else if (input == 0x49){
            // total info
            processInfo(state);
        }else{
            printf("Failed: Please type a correct command\n");
        }
    }
}

// Entry point to the process. Called in main.c
void initFunction(){

    for(int i =0; i <100; i++){
        percentageCalculator[i] = i;
    }
    state.init = createInitProcess();
    state.highPriorityQueue = ListCreate();
    state.normPriorityQueue = ListCreate();    
    state.lowPriorityQueue = ListCreate();
    state.blockedProcesses = ListCreate();

    // TODO: Error handling
    schedule(&state);

    printf("Simulation Started!\n");
    startRecievingInput(&state);
    
}
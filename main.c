#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// #define INIT_CAPACITY 3

//CONFIG.txt variables
struct Config{
        int SEED ;
        int INIT_TIME;
        int FIN_TIME ;
        int ARRIVE_MIN ;
        int ARRIVE_MAX;
        double QUIT_PROB;
        double NETWORK_PROB;
        int CPU_MIN ;
        int CPU_MAX ;
        int DISK1_MIN;
        int DISK1_MAX;
        int DISK2_MIN;
        int DISK2_MAX ;
        int NETWORK_MIN ;
        int NETWORK_MAX;
};

//struct for the event type

struct event{
    int ProcessID;
    enum types{ PROCESS_ARRIVAL, PROCESS_ARRIVE_CPU, PROCESS_FINISH_CPU,
                PROCESS_EXIT_SYSTEM, PROCESS_ARRIVE_DISK1, PROCESS_ARRIVE_DISK2,
                PROCESS_FINISH_DISK1, PROCESS_FINISH_DISK2, PROCESS_ARRIVE_NETWORK,
                PROCESS_FINISH_NETWORK, PROCESS_FINSIH}eventType;
    int Time;   //what is this??
    char *Process; //what is a process?
    struct event *prev;
    struct event *next;
}event;

struct queue{
    int count;
    struct event *head;
    struct event *tail;
}queue;


//global variables
int eventid = 1; //incremented when event is created
// int time = 0; //is this even needed?
bool cpuOccupied = false;
bool diskOccupied = false;
int eQsize = 5;
int cpuSize = 0;
int disk1Size = 0;
int disk2Size = 0;
int networkSize = 0;

// is time real?

//methods
void initConfigFile(struct Config *configVars);
void initQueue(struct queue *que);
void initEvent(struct event *event, struct queue *que);
int isEmpty(struct queue *que);
void push(struct event *event, struct queue *que);
struct event pop(struct queue *que);
void sortQueue(struct queue *que);


// void push(struct Event queue[], struct Event *newEvent); //what if the stuct that I pass is dynamic?
// struct Event pop(struct Event *queue);

int randomNumberGenerator();
int getTime();

int main(int argc, char* argv[]){
    //how can I make this not hardcoded?
    initQueue(&queue);
    printf("Main: Size of EventQueue: %lu\n", sizeof(event));
    struct event newEvent;
    // struct event newEvent2;
    // struct event newEvent3;
    // struct event newEvent4;
    initEvent(&newEvent, &queue);
    // initEvent(&newEvent2, &queue);
    // initEvent(&newEvent3, &queue);
    // initEvent(&newEvent4, &queue);
    push(&newEvent, &queue);
    // push(&newEvent2, &queue);
    // push(&newEvent3, &queue);
    // push(&newEvent4, &queue);
    // printf("Main: tail of queue: %d\n", queue.tail->ProcessID);
    // printf("Main: Popped Event: %d\n", queue.head->ProcessID);
    // struct event poppedEvent = pop(&queue);
    // printf("Main: Popped Event(test): %d\n", poppedEvent.ProcessID);
    // struct event poppedEvent2 = pop(&queue);
    // printf("Main: Popped Event(test): %d\n", poppedEvent2.ProcessID);
    // printf("Main: Head Event(test): %d\n", queue.head->ProcessID);
    int count = 0;
    while(count < 100){
        struct event newEvent2;
        initEvent(&newEvent2, &queue);
        push(&newEvent2, &queue);
        printf("Main: Tail of queue: %d\n", queue.tail->ProcessID);
        if(count % 10 == 0){
            struct event poppedEvent = pop(&queue);
            printf("Main: Popped Event: %d\n", poppedEvent.ProcessID);
        }
        count++;
    }
}

//parses config file and sets variables
//DONE
void initConfigFile(struct Config *configVars){
    FILE *file;
    char *configFile = "CONFIG.txt";
    file = fopen(configFile, "r");
    if(!configFile){
        printf("File not found");
    }
    char variable[BUFSIZ];
    fscanf(file, "%s %d", variable, &configVars->SEED);
    fscanf(file, "%s %d", variable, &configVars->INIT_TIME);
    fscanf(file, "%s %d", variable, &configVars->ARRIVE_MIN);
    fscanf(file, "%s %d", variable, &configVars->ARRIVE_MAX);
    fscanf(file, "%s %lf", variable, &configVars->QUIT_PROB);
    fscanf(file, "%s %lf", variable, &configVars->NETWORK_PROB);
    fscanf(file, "%s %d", variable, &configVars->CPU_MIN);
    fscanf(file, "%s %d", variable, &configVars->CPU_MAX);
    fscanf(file, "%s %d", variable, &configVars->DISK1_MIN);
    fscanf(file, "%s %d", variable, &configVars->DISK1_MAX);
    fscanf(file, "%s %d", variable, &configVars->DISK2_MIN);
    fscanf(file, "%s %d", variable, &configVars->DISK2_MAX);
    fscanf(file, "%s %d", variable, &configVars->NETWORK_MIN);
    fscanf(file, "%s %d", variable, &configVars->NETWORK_MAX);
    fclose(file);
}

//Linked list queue
void initQueue(struct queue *que){
    que->count = 0;
    que->head = NULL;
    que->tail = NULL;
}

//if empty return 1
int isEmpty(struct queue *que){
    if(que->tail == NULL){
        return 1;
    }else{
        return 0;
    }
}

//works but needs time and type and process worked on
void initEvent(struct event *newEvent, struct queue *que){
    newEvent->ProcessID = eventid++;
    newEvent->eventType = PROCESS_ARRIVAL; 
    newEvent->Time = getTime();//I don't understand how the time works?
    newEvent->Process = ""; //what even is this??
    char *types[11] = {"PROCESS_ARRIVAL", "PROCESS_ARRIVE_CPU", "PROCESS_FINISH_CPU","PROCESS_EXIT_SYSTEM", 
                    "PROCESS_ARRIVE_DISK1", "PROCESS_ARRIVE_DISK2","PROCESS_FINISH_DISK1", "PROCESS_FINISH_DISK2",
                    "PROCESS_ARRIVE_NETWORK", "PROCESS_FINISH_NETWORK", "PROCESS_FINISH"};
    // printf("initEvent: Event ID: %d\n", newEvent->ProcessID);
    int index = newEvent->eventType;
    // printf("initEvent: Event Type: %s\n", types[index]);//how do you get this to be a string?
    // printf("initEvent: Process arrival: %s\n", newEvent->Process);
}

//works
void push(struct event *newEvent, struct queue *que){
    que->count++;
    if(isEmpty(que) == 1){
        newEvent->prev = NULL;
        newEvent->next = NULL;
        que->head = newEvent;
        que->tail = newEvent;
    }else{
        newEvent->prev = que->tail;
        newEvent->next = NULL;
        que->tail->next = newEvent;
        que->tail = newEvent;
        que->tail->next = newEvent;
        que->tail = newEvent;
        newEvent = NULL;
    }
    //newevent next = null
    //oldtail.next = new event 
    //que-tail = newEvent
}

struct event pop(struct queue *que){
    struct event poppedEvent = *que->head;
    struct event *newHead;
    newHead = que->head->next;
    poppedEvent.next = NULL;
    poppedEvent.prev = NULL;
    newHead->prev = NULL;
    que->head->next = NULL;
    que->head = newHead;
    que->count--;
    return poppedEvent;
}

//start of priority queue methods
//TO_DO
void sortEventQueue(struct queue *que){
    printf("hi");
}

// TO_DO
int randomNumberGenerator(){
    return 1;
}

//TO_DO
int getTime(){
    return 1;
}



// #define RANDSIZE 256
// static char randstate[RANDSIZE];
// initstate(seed, randstate, RANDSIZE);

// int myrandom(int low, int high){/* It returns a random number in the interval [low,high]*/
//     return (low + random()%(high-low+1)); 
//     /* this assumes that all the bits of the number generated by 
//     random are equally random, which is not true in old implementations 
//     of random */
// }
// int main() {
//     /* It generates and prints SIZE random numbers in the interval [10,20]. 
//     * Then prints out their average (it should be close to 15).*/
//     int i;
//     int table[SIZE];
//     int sum = 0;
//     initrandom(1357);
//     for (i=0; i<SIZE; i++) {
//         table[i] = myrandom(10,20);
//         sum += table[i];
//         printf("%2d\n", table[i]);
//     }
//     printf("the average is %f\n", sum/(float)SIZE);
//     exit(0);
// }
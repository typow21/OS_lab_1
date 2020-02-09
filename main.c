#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// #define INIT_CAPACITY 3

//CONFIG.txt variables
struct Config{
        int SEED ;
        int INIT_TIME;
        int FIN_TIME ;
        int ARRIVE_MIN;
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
}config;

//struct for the event type

struct event{
    int ProcessID;
    enum types{ SIM_START, PROCESS_ARRIVAL, PROCESS_ARRIVE_CPU, PROCESS_FINISH_CPU,
                PROCESS_EXIT_SYSTEM, PROCESS_ARRIVE_DISK1, PROCESS_ARRIVE_DISK2,
                PROCESS_FINISH_DISK1, PROCESS_FINISH_DISK2, PROCESS_ARRIVE_NETWORK,
                PROCESS_FINISH_NETWORK, PROCESS_FINSIH, SIM_END}eventType;
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
int clock = 0; //is this even needed?
bool cpuOccupied = false;
bool diskOccupied = false;
// is time real?

//methods
void initConfigFile();
void initQueue(struct queue *que);
int randomNumberGenerator(int high, int low);
void initEvent(struct event *event, struct queue *que);
int isEmpty(struct queue *que);
void push(struct event *event, struct queue *que);
struct event pop(struct queue *que);
void sortEventQueue(struct queue *que);
void swap(struct event *first, struct event *second);

// void push(struct Event queue[], struct Event *newEvent); //what if the stuct that I pass is dynamic?
// struct Event pop(struct Event *queue);

int randomNumberGenerator();
int getTime();

int main(int argc, char* argv[]){
    struct queue eventQueue;
    initQueue(&eventQueue);
    printf("Main: Size of queue: %lu\n", sizeof(event));
    initConfigFile();
    srand(config.SEED); //random seed
    printf("Main: arrive max: %d\n", config.ARRIVE_MAX);
    int random = randomNumberGenerator(config.ARRIVE_MAX, config.ARRIVE_MIN); //test random number generator
    printf("Main: Random: %d\n", random);
    // while(eventQueue.count < 0){

    // }
    //how can I make this not hardcoded?
    // struct queue *eventQueue;
    // struct event newEvent2;
    // initEvent(&newEvent2, &eventQueue);
    // push(&newEvent2, &eventQueue);
    // struct event newEvent3;
    // initEvent(&newEvent3, &eventQueue);
    // push(&newEvent3, &eventQueue);
    // int count = 0;
    // while(count < 100){
    //     struct event newEvent2;
    //     initEvent(&newEvent2, &eventQueue);
    //     push(&newEvent2, &eventQueue);
    //     // printf("Main: Tail of queue: %d\n", eventQueue.tail->ProcessID);
    //     // printf("Main: Head of queue: %d\n", eventQueue.head->Time);
    //     // if(count % 10 == 0){
    //     //     struct event poppedEvent = pop(&eventQueue); //this is making things act weird.
    //     //     printf("Main: Popped Event: %d\n", poppedEvent.ProcessID);
    //     //     printf("Main: Head(test): %d\n", eventQueue.head->ProcessID);
    //     // }
    //     count++;
    // }
    int count = 0;
    while(count <= 100){
        struct event *newEvent;
        newEvent = malloc(sizeof(struct event));
        initEvent(newEvent, &eventQueue);
        push(newEvent, &eventQueue); 
        // printf("Main: Tail of queue: %d\n", eventQueue.tail->ProcessID);
        // printf("Main: Head of queue: %d\n", eventQueue.head->ProcessID);
        if(count % 10 == 0){
            // struct event poppedEvent = pop(&eventQueue); //this is making things act weird.
            // printf("Main: Popped Event: %d\n", poppedEvent.ProcessID);
        }
        count ++;
    }
    // printf("Main: Head(test): %d\n", eventQueue.head->ProcessID);
    // printf("Main: Head next(test): %d\n", eventQueue.head->next->ProcessID);

    // printf("Main: Head next prev(test): %d\n", eventQueue.head->next->prev->ProcessID);
    // printf("Main: Head next next(test): %d\n", eventQueue.head->next->next->ProcessID);

    // printf("Main: Tail(test): %d\n", eventQueue.tail->ProcessID);
    // printf("Main: Tail prev(test): %d\n", eventQueue.tail->prev->ProcessID);
    // printf("Main: Tail prev next(test): %d\n", eventQueue.tail->prev->next->ProcessID);
    
    
    sortEventQueue(&eventQueue);
    struct event *curr = eventQueue.head;
    while(curr->next != NULL){
        printf("Sort check: Current time: %d | ID: %d\n", curr->Time, curr->ProcessID);
        if(curr->next != NULL){
            curr = curr->next;
        }
        count++;
    }
    printf("Main: Size of EventQueue memory: %lu\n", sizeof(event)*eventQueue.count);
    // //I don't malloc any memory so do I have to free it?
}

//parses config file and sets variables
//DONE passed test
void initConfigFile(){
    FILE *file;
    char *configFile = "CONFIG.txt";
    file = fopen(configFile, "r");
    if(!configFile){
        printf("File not found.");
    }
    char variable[BUFSIZ];
    fscanf(file, "%s %d", variable, &config.SEED);
    // printf("%s %d\n", variable, config.SEED);

    fscanf(file, "%s %d", variable, &config.INIT_TIME);
    // printf("%s %d\n", variable, config.INIT_TIME);

    fscanf(file, "%s %d", variable, &config.FIN_TIME);
    // printf("%s %d\n", variable, config.FIN_TIME);

    fscanf(file, "%s %d", variable, &config.ARRIVE_MIN);
    // printf("%s %d\n", variable, config.ARRIVE_MIN);

    fscanf(file, "%s %d", variable, &config.ARRIVE_MAX);
    // printf("%s %d\n", variable, config.ARRIVE_MAX);

    fscanf(file, "%s %lf", variable, &config.QUIT_PROB);
    // printf("%s %f\n", variable, config.QUIT_PROB);

    fscanf(file, "%s %lf", variable, &config.NETWORK_PROB);
    // printf("%s %f\n", variable, config.NETWORK_PROB);

    fscanf(file, "%s %d", variable, &config.CPU_MIN);
    // printf("%s %d\n", variable, config.CPU_MIN);

    fscanf(file, "%s %d", variable, &config.CPU_MAX);
    // printf("%s %d\n", variable, config.CPU_MAX);

    fscanf(file, "%s %d", variable, &config.DISK1_MIN);
    // printf("%s %d\n", variable, config.DISK1_MIN);

    fscanf(file, "%s %d", variable, &config.DISK1_MAX);
    // printf("%s %d\n", variable, config.DISK1_MAX);

    fscanf(file, "%s %d", variable, &config.DISK2_MIN);
    // printf("%s %d\n", variable, config.DISK2_MIN);

    fscanf(file, "%s %d", variable, &config.DISK2_MAX);
    printf("%s %d\n", variable, config.DISK2_MAX);

    fscanf(file, "%s %d", variable, &config.NETWORK_MIN);
    // printf("%s %d\n", variable, config.NETWORK_MIN);

    fscanf(file, "%s %d", variable, &config.NETWORK_MAX);
    // printf("%s %d\n", variable, config.NETWORK_MAX);
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

void startingEvent(struct event *newEvent, struct queue *que){
    newEvent->ProcessID = -1;
    newEvent->eventType = PROCESS_ARRIVAL;
    newEvent->Time = config.INIT_TIME;//I don't understand how the time works?
    newEvent->Process = ""; //what even is this??
    char *types[13] = {"SIM_START","ARRIVAL", "ARRIVE_CPU", "FINISH_CPU","EXIT_SYSTEM", 
                    "ARRIVE_DISK1", "ARRIVE_DISK2","FINISH_DISK1", "FINISH_DISK2",
                    "ARRIVE_NETWORK", "FINISH_NETWORK", "FINISH", "SIM_END"};
    
    // printf("initEvent: Event ID: %d\n", newEvent->ProcessID);

    // int index = newEvent->eventType; //what is this and what does it do?

    // printf("initEvent: Event Type: %s\n", types[index]);//how do you get this to be a string?
    // printf("initEvent: Process arrival: %s\n", newEvent->Process);
}

void endEvent(struct event *newEvent, struct queue *que){
    newEvent->ProcessID = -1;
    newEvent->eventType = PROCESS_ARRIVAL;
    newEvent->Time = config.FIN_TIME;//I don't understand how the time works?
    newEvent->Process = ""; //what even is this??
    char *types[13] = {"SIM_START","ARRIVAL", "ARRIVE_CPU", "FINISH_CPU","EXIT_SYSTEM", 
                    "ARRIVE_DISK1", "ARRIVE_DISK2","FINISH_DISK1", "FINISH_DISK2",
                    "ARRIVE_NETWORK", "FINISH_NETWORK", "FINISH", "SIM_END"};
    // printf("initEvent: Event ID: %d\n", newEvent->ProcessID);

    // int index = newEvent->eventType; //what is this and what does it do?

    // printf("initEvent: Event Type: %s\n", types[index]);//how do you get this to be a string?
    // printf("initEvent: Process arrival: %s\n", newEvent->Process);
}

//works but needs time and type and process worked on
void initEvent(struct event *newEvent, struct queue *que){
    newEvent->ProcessID = eventid++;
    newEvent->eventType = PROCESS_ARRIVAL; 
    newEvent->Time = getTime();//I don't understand how the time works
    newEvent->Process = ""; //what even is this??
    char *types[13] = {"SIM_START","ARRIVAL", "ARRIVE_CPU", "FINISH_CPU","EXIT_SYSTEM", 
                    "ARRIVE_DISK1", "ARRIVE_DISK2","FINISH_DISK1", "FINISH_DISK2",
                    "ARRIVE_NETWORK", "FINISH_NETWORK", "FINISH", "SIM_END"};
    // printf("initEvent: Event ID: %d\n", newEvent->ProcessID);
    int index = newEvent->eventType;
    // printf("initEvent: Event Type: %s\n", types[index]);//how do you get this to be a string?
    // printf("initEvent: Process arrival: %s\n", newEvent->Process);
}

//Done and tested
void push(struct event *newEvent, struct queue *que){
    que->count++;
    if(isEmpty(que) == 1){
        newEvent->prev = NULL;
        newEvent->next = NULL;
        que->head = newEvent;
        que->tail = newEvent;
    }else{
        newEvent->next = NULL;
        newEvent->prev = que->tail;
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
//Done and partially tested
void sortEventQueue(struct queue *que){
    //get first node
    //compare to second node
    //if second < first then swap
    int swapped, i;
    struct event *first = que->head;
    struct event *second = NULL;

    if(first == NULL){
        return;
    }
    do{
        swapped = 0;
        first = que->head;
        while(first->next != second){
            if(first->Time > first->next->Time){
                swap(first, first->next);
                swapped = 1;
            }
            first = first->next;
        }
        second = first;
    }while(swapped);
}

void swap(struct event *larger, struct event *smaller){
    struct event temp = *larger; //create temp

    //move all small data into large data
    larger->ProcessID = smaller->ProcessID;
    larger->eventType = smaller->eventType;
    larger->Time = smaller->Time;

    //move all temp(large) data into small data
    larger->Process = smaller->Process;
    smaller->ProcessID = temp.ProcessID;
    smaller->eventType = temp.eventType;
    smaller->Time = temp.Time;
}

// Done passed test
int randomNumberGenerator(int min, int max){
    int random = rand() % (max - min + 1) + max;
    // (highrange-lowrange)+1
    return random;
}

//TO_DO
int getTime(){
    int time = randomNumberGenerator(100, 0);
    return time;
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
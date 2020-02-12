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
    enum types{ ARRIVAL, ARRIVE_CPU, FINISH_CPU,
                EXIT_SYSTEM, ARRIVE_DISK1, ARRIVE_DISK2,
                FINISH_DISK1, FINISH_DISK2, ARRIVE_NETWORK,
                FINISH_NETWORK, FINISH}eventType;
    int Time;
    struct event *prev;
    struct event *next;
}event;

struct queue{
    int count;
    struct event *head;
    struct event *tail;
}queue;

//Event Handlers
void jobArrival(struct event *event, struct queue *queue);
void cpuArrive(struct event *event, struct queue *queue);
void cpuFinish(struct event *event, struct queue *queue);
// int quitProb();
void exitSystem(struct event *event, struct queue *queue);
void arriveDisk1(struct event *event, struct queue *queue);
void arriveDisk2(struct event *event, struct queue *queue);
void finishDisk1(struct event *event, struct queue *queue);
void finishDisk2(struct event *event, struct queue *queue);
void arriveNetwork(struct event *event, struct queue *queue);
void finishNetwork(struct event *event, struct queue *queue);

//global variables
int eventid = 0; //incremented when event is created
int clock = 0; //is this even needed?
bool cpuOccupied = false;
bool disk1Occupied = false;
bool disk2Occupied = false;
bool networkOccupied = false;
char *types[13] = {"enters system", "enters CPU", "exits CPU", "exits the system", 
                    "begins I/O on Disk 1", "begins I/O on Disk 2","finishes on DISK 1", "finishes on Disk 1",
                    "contacts the network", "finishes with network", "closes the program"};
// is time real?
 int count = 0;
int totalCPU = 0;
int totalDisk1 = 0;
int totalDisk2 = 0;
int totalNetwok = 0;

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

//init device queues
struct queue cpuQueue;
struct queue disk1Queue;
struct queue disk2Queue;
struct queue networkQueue;

int main(int argc, char* argv[]){
    struct queue eventQueue;
    initQueue(&eventQueue);
    // printf("Main: Size of queue: %lu\n", sizeof(event));

    initConfigFile();
    srand(config.SEED); //random seed

    struct event *startEvent;
    startEvent = malloc(sizeof(struct event));
    // initEvent(startEvent, &eventQueue);
    startEvent->Time = config.INIT_TIME;
    startEvent->eventType = ARRIVAL;
    startEvent->ProcessID = 0;
    eventid++;
    push(startEvent, &eventQueue);

    struct event *finishEvent;
    finishEvent = malloc(sizeof(struct event));
    // initEvent(finishEvent, &eventQueue);
    finishEvent->Time = config.FIN_TIME;
    finishEvent->eventType = FINISH;
    finishEvent->ProcessID = 100000;
    push(finishEvent, &eventQueue);
    
    while(isEmpty(&eventQueue) != 1){
        sortEventQueue(&eventQueue);

        if(cpuQueue.count>0){
            struct event curr = *cpuQueue.head;
            while(true){
                // printf("\n####ARRIVAL : CPU Queue check: Event ID: %d |Event Time: %d | Event Type: %s####\n", 
                //                     curr.ProcessID, curr.Time, types[curr.eventType]);
                if(curr.next == NULL){
                    break;
                }
                curr = *curr.next;
            }
        }

        struct event *curr = eventQueue.head;
        while(true){
        // printf("\nMain loop: Queue check: Current ID: %d | Time: %d | Type: %s\n",
        //                                 curr->ProcessID, curr->Time, types[curr->eventType]);
        if(curr->next == NULL){
            break;
        }
        curr = curr->next;
        }
        // printf("\n");
        struct event popEvent = pop(&eventQueue);
        printf("At time %d, process %d %s\n", 
                            popEvent.Time, popEvent.ProcessID, types[popEvent.eventType]);
        switch(popEvent.eventType){
            case ARRIVAL:
                jobArrival(&popEvent, &eventQueue);
                break;
            case ARRIVE_CPU:
                cpuArrive(&popEvent, &eventQueue);
                break;
            case FINISH_CPU:
                cpuFinish(&popEvent, &eventQueue);
                break;
            case EXIT_SYSTEM:
                // printf("Job complete...exiting");
                break;
            case ARRIVE_DISK1:
                arriveDisk1(&popEvent, &eventQueue);
                break;
            case ARRIVE_DISK2:
                arriveDisk2(&popEvent, &eventQueue);
                break;
            case FINISH_DISK1:
                finishDisk1(&popEvent, &eventQueue);
                break;
            case FINISH_DISK2:
                finishDisk2(&popEvent, &eventQueue);
                break;
            case ARRIVE_NETWORK:
                arriveNetwork(&popEvent, &eventQueue);
                break;
            case FINISH_NETWORK:
                finishNetwork(&popEvent, &eventQueue);
                break;
            case FINISH:
                // printf("Program finished");
                exit(0);
                break;
        }
        totalCPU += cpuQueue.count;
        totalDisk1 += disk1Queue.count;
        totalDisk2 += disk2Queue.count;
        totalNetwork += networkQueue.count;
        count++;
    }
    //Tests below
    {
        // struct event *head = eventQueue.head;
        // while(head != NULL){
        //     printf("Main: Event ID: %d | Time: %d\n", head->ProcessID, head->Time);
        //     head = head->next;
        // }
        // int count2 = 0;
        // struct event *tail;
        // while(count2 <= 100){
        //     tail = eventQueue.tail;
        //     struct event *newEvent;
        //     newEvent = malloc(sizeof(struct event));
        //     initEvent(newEvent, &eventQueue);
        //     push(newEvent, &eventQueue);
        //     sortEventQueue(&eventQueue);
        //     // printf("Main: Tail of queue: %d\n", eventQueue.tail->ProcessID);
        //     // printf("Main: Event ID: %d | Time: %d\n", tail->ProcessID, tail->Time);
        //     if(count2 % 10 == 0){
        //         struct event poppedEvent = pop(&eventQueue); //this is making things act weird.
        //         printf("Main: Popped Event: %d\n", poppedEvent.ProcessID);
        //     }
        //     count2 ++;
        // }
        // printf("Main: Head(test): %d\n", eventQueue.head->ProcessID);
        // printf("Main: Head next(test): %d\n", eventQueue.head->next->ProcessID);

        // printf("Main: Head next prev(test): %d\n", eventQueue.head->next->prev->ProcessID);
        // printf("Main: Head next next(test): %d\n", eventQueue.head->next->next->ProcessID);

        // printf("Main: Tail(test): %d\n", eventQueue.tail->ProcessID);
        // printf("Main: Tail prev(test): %d\n", eventQueue.tail->prev->ProcessID);
        // printf("Main: Tail prev next(test): %d\n", eventQueue.tail->prev->next->ProcessID);
    }
    statistics();
}
//Done needs more testing
void jobArrival(struct event *popEvent, struct queue *currQueue){
    // printf("Job arrival: CALLLED\n");
    struct event *newEvent2;
    newEvent2 = malloc(sizeof(event));
    newEvent2->eventType = ARRIVAL;
    newEvent2->ProcessID = eventid;
    eventid++;
    newEvent2->Time = getTime(config.ARRIVE_MIN, config.ARRIVE_MAX)+popEvent->Time;//getTime(config.ARRIVE_CPU, config.FINISH_CPU); //time is fucking up??
    push(newEvent2, currQueue);
    if(cpuOccupied ==false && cpuQueue.count == 0){ //works
        struct event *newEvent;
        newEvent = malloc(sizeof(event));
        newEvent->Time = popEvent->Time;
        newEvent->ProcessID = popEvent->ProcessID;
        // printf("Job arrival: newEvent Time: %d | newEvent Process ID: %d\n",newEvent->Time, newEvent->ProcessID);
        newEvent->eventType = ARRIVE_CPU;
        push(newEvent, currQueue);
        cpuOccupied = true;
    }
    else if(cpuOccupied == false && cpuQueue.count == 0){
            struct event *newEvent;
            newEvent = malloc(sizeof(event));
            newEvent->Time = popEvent->Time;
            newEvent->eventType = ARRIVAL;
            newEvent->ProcessID = cpuQueue.head->ProcessID;
            push(newEvent, &cpuQueue);
    }

    //Dead code
    {
            // if(cpuOccupied == false && cpuQueue.count == 0){ //works
            //     struct event *newEvent;
            //     newEvent = malloc(sizeof(event));
            //     newEvent->Time = getTime(config.CPU_MIN, config.CPU_MAX)+popEvent->Time;
            //     newEvent->ProcessID = popEvent->ProcessID;
            //     // printf("Job arrival: newEvent Time: %d | newEvent Process ID: %d\n",newEvent->Time, newEvent->ProcessID);
            //     newEvent->eventType = ARRIVE_CPU;
            //     push(newEvent, currQueue);
            //     cpuOccupied = true;
            // }
            //     else if(cpuOccupied == true){
            //     struct event *newEvent;
            //     newEvent = malloc(sizeof(event));
            //     newEvent->Time = getTime(config.CPU_MIN, config.CPU_MAX)+popEvent->Time;
            //     newEvent->ProcessID = popEvent->ProcessID;
            //     // printf("Job arrival: newEvent Time: %d | newEvent Process ID: %d\n",newEvent->Time, newEvent->ProcessID);
            //     newEvent->eventType = popEvent->eventType;
            //     push(newEvent, &cpuQueue);
            //     // struct event curr = *cpuQueue.head;
            //     //     while(true){
            //     //         printf("####ARRIVAL : CPU Queue check: Event ID: %d |Event Time: %d | Event Type: %s####\n", 
            //     //                             curr.ProcessID, curr.Time, types[curr.eventType]);
            //     //         if(curr.next == NULL){
            //     //             break;
            //     //         }
            //     //         curr = *curr.next;
            //     //     }
            //     }
            //     else if(cpuOccupied == false && cpuQueue.count != 0){
            //             printf("Popping events \n");
            //             struct event *newEvent;
            //             newEvent = malloc(sizeof(event));
            //             newEvent->Time = getTime(config.CPU_MIN, config.CPU_MAX)+popEvent->Time;
            //             newEvent->eventType = FINISH_CPU;
            //             newEvent->ProcessID = cpuQueue.head->ProcessID;
            //             cpuOccupied = true;
            //             push(newEvent, currQueue);
            //             struct event blank = pop(&cpuQueue);
            //     }
            // struct event *newEvent2;
            // newEvent2 = malloc(sizeof(event));
            // newEvent2->eventType = ARRIVAL;
            // newEvent2->ProcessID = eventid;
            // eventid++;
            // newEvent2->Time = getTime(config.ARRIVE_MIN, config.ARRIVE_MAX)+popEvent->Time;//getTime(config.ARRIVE_CPU, config.FINISH_CPU); //time is fucking up??
            // {
                // if(cpuOccupied == false){
                //     newEvent->eventType = ARRIVE_CPU;
                //     newEvent->Time = event->Time;
                //     push(event, queue);
                //     cpuOccupied = true;
                // }
                // // printf("this works");
                // // push(event, &cpuQueue);
                // // printf("got this far");
                // // // sortEventQueue(queue);
                // struct event *newEvent2;
                // newEvent = malloc(sizeof(event));
                // initEvent(newEvent2, queue);
                // push(newEvent2, queue);
                // sortEventQueue(queue);
                // tests below
                // {
                //     // printf("Job Arrival: New Event:  Event ID: %d |Event Time: %d | Event Type: %s\n", 
                //     //                         newEvent->ProcessID, newEvent->Time, types[newEvent->eventType]);
                    
                //     // printf("Job Arrival: Queue Test: Head(test): %d | %s\n", 
                //     //                 queue->head->ProcessID, types[queue->head->eventType]);
                //     // printf("Job Arrival: Queue Test: Head next(test): %d | %s\n", 
                //     //                 queue->head->next->ProcessID, types[queue->head->next->eventType]);
                //     // // printf("Job Arrival: Queue Test: Head next next(test): %d | %s\n", 
                //     // //                 queue->head->next->next->ProcessID, types[queue->head->next->next->eventType]);
                //     // printf("Job Arrival: Queue Test: Tail(test): %d | %s\n", 
                //     //                 queue->tail->ProcessID, types[queue->tail->eventType]);

                //     // struct event curr = *queue->head;
                //     // while(true){
                //     //     printf("Job Arrival:  Event ID: %d |Event Time: %d | Event Type: %s\n", 
                //     //                         curr.ProcessID, curr.Time, types[curr.eventType]);
                //     //     if(curr.next == NULL){
                //     //         break;
                //     //     }
                //     //     curr = *curr.next;
                //     // }
                // }
        }
    }

void cpuArrive(struct event *popEvent, struct queue *currQueue){
    struct event *newEvent;
    newEvent = malloc(sizeof(event));
    newEvent->Time = getTime(config.CPU_MIN, config.CPU_MAX)+popEvent->Time;
    newEvent->eventType = FINISH_CPU;
    newEvent->ProcessID = popEvent->ProcessID;
    push(newEvent, currQueue);
}
void cpuFinish(struct event *popEvent, struct queue *currQueue){
    double quitProb = config.QUIT_PROB * 100;
    double networkProb = config.NETWORK_PROB * 100;
    int random = getTime(0, 100);
    // printf("RANDOM: %f | %d", quitProb, random);
    if(random <= quitProb){
        cpuOccupied = false;
        struct event *newEvent2;
        newEvent2 = malloc(sizeof(event));
        newEvent2->ProcessID = popEvent->ProcessID;
        newEvent2->eventType = EXIT_SYSTEM; 
        newEvent2->Time = popEvent->Time;
        push(newEvent2, currQueue);
    }else if(random <= networkProb){
        if(networkQueue.count == 0 && networkOccupied == false){
            cpuOccupied = false;
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = ARRIVE_NETWORK; 
            newEvent2->Time = popEvent->Time;
            networkOccupied = true;
            push(newEvent2, currQueue);
        }else{
            push(popEvent, &networkQueue);
        }
    }else{
        if(disk1Occupied == false){
            cpuOccupied = false;
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = ARRIVE_DISK1; 
            newEvent2->Time = popEvent->Time;
            push(newEvent2, currQueue);
        }else if(disk2Occupied == false){
            cpuOccupied = false;
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = ARRIVE_DISK2; 
            newEvent2->Time = popEvent->Time;
            push(newEvent2, currQueue);
        }else if(disk1Queue.count < disk2Queue.count){
            cpuOccupied = false;
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = ARRIVE_DISK1; 
            newEvent2->Time = popEvent->Time;
            push(newEvent2, currQueue);
        }else{
            cpuOccupied = false;
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = ARRIVE_DISK2; 
            newEvent2->Time = popEvent->Time;
            push(newEvent2, currQueue);
        }
    }
    if(cpuQueue.count > 0){
        // printf("This is called but nothing happed?\n");
        struct event *newEvent;
        newEvent = malloc(sizeof(event));
        newEvent->eventType = ARRIVE_CPU;
        newEvent->Time = cpuQueue.head->Time;
        newEvent->ProcessID = cpuQueue.head->ProcessID;
        struct event blank = pop(&cpuQueue);
        push(newEvent, currQueue);
    }
}

//1 to exit -- 0 to stay
// int quitProb(){
//     double quitProb = config.QUIT_PROB;
//     return 1;
// }

void exitSystem(struct event *popEvent, struct queue *ueue){
    printf("Exit System works");
}
void arriveDisk1(struct event *popEvent, struct queue *currQueue){
    if(disk1Occupied == false && disk1Queue.count == 0 ){
       struct event *newEvent;
        newEvent = malloc(sizeof(event));
       newEvent->eventType = FINISH_DISK1;
       newEvent->Time = getTime(config.DISK1_MIN, config.DISK1_MAX)+popEvent->Time;
       newEvent->ProcessID = popEvent->ProcessID;
       disk1Occupied = true;
       push(newEvent, currQueue);
    }else if(disk1Occupied == false && disk1Queue.count > 0){
        push(popEvent,&disk1Queue);
        struct event popDiskEvent;
        popDiskEvent = pop(&disk1Queue);
        popDiskEvent.eventType = FINISH_DISK1;
        popDiskEvent.Time = getTime(config.DISK1_MIN, config.DISK1_MAX)+popEvent->Time;
        push(&popDiskEvent, &disk1Queue);
        disk1Occupied = true;
    }
}
void arriveDisk2(struct event *popEvent, struct queue *currQueue){
    if(disk2Occupied == false && disk2Queue.count == 0 ){
       struct event *newEvent;
        newEvent = malloc(sizeof(event));
       newEvent->eventType = FINISH_DISK2;
       newEvent->Time = getTime(config.DISK2_MIN, config.DISK2_MAX)+popEvent->Time;
       newEvent->ProcessID = popEvent->ProcessID;
       disk2Occupied = true;
       push(newEvent, currQueue);
    }else if(disk2Occupied == false && disk2Queue.count > 0){
        push(popEvent,&disk2Queue);
        struct event popDiskEvent;
        popDiskEvent = pop(&disk2Queue);
        popDiskEvent.eventType = FINISH_DISK2;
        popDiskEvent.Time = getTime(config.DISK2_MIN, config.DISK2_MAX)+popEvent->Time;
        push(&popDiskEvent, &disk2Queue);
        disk2Occupied = true;
    }
}
void finishDisk1(struct event *popEvent, struct queue *currQueue){
    disk1Occupied = false; 
    struct event *newEvent;
    newEvent = malloc(sizeof(event));
    newEvent->Time = getTime(config.ARRIVE_MIN, config.ARRIVE_MAX)+popEvent->Time;;
    newEvent->eventType = ARRIVAL;
    newEvent->ProcessID = popEvent->ProcessID;
    push(newEvent, currQueue);
    if(disk1Queue.count > 0){
        struct event *newEvent2;
        newEvent2 = malloc(sizeof(event));
        newEvent2->Time = disk1Queue.head->Time;
        newEvent2->eventType = ARRIVE_DISK1;
        newEvent2->ProcessID = disk1Queue.head->ProcessID;
        push(newEvent2, currQueue);
    }
}
void finishDisk2(struct event *popEvent, struct queue *currQueue){
    disk2Occupied = false; 
    struct event *newEvent;
    newEvent = malloc(sizeof(event));
    newEvent->Time = getTime(config.ARRIVE_MIN, config.ARRIVE_MAX)+popEvent->Time;
    newEvent->eventType = ARRIVAL;
    newEvent->ProcessID = popEvent->ProcessID;
    push(newEvent, currQueue);
    if(disk2Queue.count > 0){
        struct event *newEvent2;
        newEvent2 = malloc(sizeof(event));
        newEvent2->Time = disk2Queue.head->Time;
        newEvent2->eventType = ARRIVE_DISK2;
        newEvent2->ProcessID = disk2Queue.head->ProcessID;
        push(newEvent2, currQueue);
    }
}
void arriveNetwork(struct event *popEvent, struct queue *currQueue){
    if(networkOccupied == false && networkQueue.count == 0 ){
       struct event *newEvent;
        newEvent = malloc(sizeof(event));
       newEvent->eventType = FINISH_NETWORK;
       newEvent->Time = getTime(config.NETWORK_MIN, config.NETWORK_MAX)+popEvent->Time;
       newEvent->ProcessID = popEvent->ProcessID;
        networkOccupied = true;
       push(newEvent, currQueue);
    }else if(networkOccupied == false && networkQueue.count > 0){
        push(popEvent,&networkQueue);
        struct event popNetworkEvent;
        popNetworkEvent = pop(&networkQueue);
        popNetworkEvent.eventType = FINISH_NETWORK;
        popNetworkEvent.Time = getTime(config.NETWORK_MIN, config.NETWORK_MAX)+popEvent->Time;
        push(&popNetworkEvent, &networkQueue);
        networkOccupied = true;
    }
}
void finishNetwork(struct event *popEvent, struct queue *currQueue){
    networkOccupied = false;
    struct event *newEvent;
    newEvent = malloc(sizeof(event));
    newEvent->eventType = ARRIVAL;
    newEvent->Time = popEvent->Time;
    newEvent->ProcessID = popEvent->ProcessID;
    push(newEvent, currQueue);
    if(networkQueue.count > 0){
        // printf("sick");
    }
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

//works but needs time and type and process worked on
void initEvent(struct event *newEvent, struct queue *que){
    newEvent->ProcessID = eventid++;
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
        // newEvent = NULL;
    }
    //newevent next = null
    //oldtail.next = new event 
    //que-tail = newEvent
}

struct event pop(struct queue *que){
    // if(que->count == 1){
    //     printf("Pop: Program complete. Exiting...");
    //     exit(0);
    // }
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
    larger->ProcessID = smaller->ProcessID;
    smaller->ProcessID = temp.ProcessID;
    smaller->eventType = temp.eventType;
    smaller->Time = temp.Time;
}

// Done passed test
int randomNumberGenerator(int min, int max){
    int random = rand() % (max + min);
    return random;
}

//TO_DO
int getTime(int min, int max){
    int time = randomNumberGenerator(min, max);
    return time;
}


// Test code


// int random = randomNumberGenerator(config.ARRIVE_MAX, config.ARRIVE_MIN); //test random number generator
    // printf("Main: Random: %d\n", random);

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



     // sortEventQueue(&eventQueue);
    // struct event *curr = eventQueue.head;
    // while(curr->next != NULL){
    //     printf("Sort check: Current time: %d | ID: %d\n", curr->Time, curr->ProcessID);
    //     if(curr->next != NULL){
    //         curr = curr->next;
    //     }
    //     count++;
    // }
    // printf("Main: Size of EventQueue memory: %lu\n", sizeof(event)*eventQueue.count);
    // //I don't malloc any memory so do I have to free it?
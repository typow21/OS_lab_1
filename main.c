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
void statistics();

//global variables
int eventid = 1; //incremented when event is created
int clock = 0; //is this even needed?
bool cpuOccupied = false;
bool disk1Occupied = false;
bool disk2Occupied = false;
bool networkOccupied = false;
char *types[13] = {"enters system", "enters CPU", "exits CPU", "exits the system", 
                    "begins I/O on Disk 1", "begins I/O on Disk 2","finishes on DISK 1", "finishes on Disk 2",
                    "contacts the network", "finishes with network", "closes the program"};
// is time real?

//stats file variables

    //queues
    int count = 0;
    double totalCPU = 0;
    int maxCPU = 0;
    double totalDisk1 = 0;
    int maxDisk1 = 0;
    double totalDisk2 = 0;
    int maxDisk2 = 0;
    double totalNetwork = 0;
    int maxNetwork = 0;

    //Utilization
    int cpuTimeIn = 0;
    int cpuTimeOut = 0;
    int cpuTotal = 0;

    int disk1TimeIn = 0;
    int disk1TimeOut = 0;
    int disk1Total = 0;

    int disk2TimeIn = 0;
    int disk2TimeOut = 0;
    int disk2Total = 0;

    int networkTimeIn = 0;
    int networkTimeOut = 0;
    int networkTotal = 0;

    //Response Time
    int cpuCount = 0;
    int cpuSum = 0;
    int cpuMax = 0;
    double cpuAvg = 0;

    int disk1count = 0;
    int disk1sum = 0;
    int disk1max = 0;
    double disk1Avg = 0 ;

    int disk2count = 0;
    int disk2sum =0;
    int disk2max = 0;
    double disk2Avg = 0;

    int networkCount = 0;
    int networkSum = 0;
    int networkMax = 0;
    double networkAvg = 0;


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
int randomNumberGenerator();
int getTime();

//init device queues
struct queue cpuQueue;
struct queue disk1Queue;
struct queue disk2Queue;
struct queue networkQueue;

int main(int argc, char* argv[]){
    FILE *file = fopen("Log.txt","w");
    struct queue eventQueue;
    initQueue(&eventQueue);
    initQueue(&cpuQueue);
    // printf("Main: Size of queue: %lu\n", sizeof(event));

    //load global vari from file 
    initConfigFile();
    srand(config.SEED); //random seed

    //first event 
    struct event *startEvent;
    startEvent = malloc(sizeof(struct event));
    startEvent->Time = config.INIT_TIME;
    startEvent->eventType = ARRIVAL;
    startEvent->ProcessID = 0;
    push(startEvent, &eventQueue);

    //last event
    struct event *finishEvent;
    finishEvent = malloc(sizeof(struct event));
    finishEvent->Time = config.FIN_TIME;
    finishEvent->eventType = FINISH;
    finishEvent->ProcessID = 100000;
    push(finishEvent, &eventQueue);
    
    while(isEmpty(&eventQueue) != 1){
        sortEventQueue(&eventQueue); //sorts priority queue

        struct event popEvent = pop(&eventQueue);
        fprintf(file, "At time %d, process %d %s\n", 
                    popEvent.Time, popEvent.ProcessID, types[popEvent.eventType]); 
        // printf("At time %d, process %d %s\n", 
        //             popEvent.Time, popEvent.ProcessID, types[popEvent.eventType]); 
        totalCPU += cpuQueue.count;
        if(cpuQueue.count > maxCPU){
            maxCPU = cpuQueue.count;
        }
        totalDisk1 += disk1Queue.count;
        if(disk1Queue.count > maxDisk1){
            maxDisk1 = disk1Queue.count;
        }
        totalDisk2 += disk2Queue.count;
        if(disk2Queue.count > maxDisk2){
            maxDisk2 = disk2Queue.count;
        }
        totalNetwork += networkQueue.count;
        if(networkQueue.count > maxNetwork){
            maxNetwork = networkQueue.count;
        }
        count++;
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
                statistics();
                exit(0);
                break;
        }
    }
    count++;
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
    fclose(file);
}

void statistics(){
    FILE *file = fopen("STAT.txt","w");
    fprintf(file, "----------Queues----------\n");
    fprintf(file, "Average size of CPU Queue: %f\n", totalCPU/count);
    fprintf(file, "Max size of Queue: %d\n", maxCPU);
    fprintf(file, "Average size of Disk 1 Queue: %f\n", totalDisk1/count);
    fprintf(file, "Max size of Disk 1 Queue: %d\n",maxDisk1);
    fprintf(file, "Average size of Disk 2 Queue: %f\n", totalDisk2/count);
    fprintf(file, "Max size of Disk 2 Queue: %d\n",maxDisk2);
    fprintf(file, "Average size of Network Queue: %f\n", totalNetwork/count);
    fprintf(file, "Max size of Network Queue: %d\n",maxNetwork);
    fprintf(file, "\n");

    fprintf(file, "----------Utilization----------\n");
    //cpuTotal is  cpuTotal += newEvent->Time - popEvent->Time and is done in cpu Arrive
        //counts how long the cpu is used throughout the program 
    fprintf(file, "Utilization of CPU: %f\n", (double)cpuTotal/(double)config.FIN_TIME - config.INIT_TIME);
    
    fprintf(file, "Utilization of Disk1: %f\n", (double)disk1Total/(double)config.FIN_TIME - config.INIT_TIME);
    
    fprintf(file, "Utilization of Disk 2: %f\n", (double)disk2Total/(double)config.FIN_TIME - config.INIT_TIME);

    fprintf(file, "Utilization of network: %f\n", (double)networkTotal/(double)config.FIN_TIME - config.INIT_TIME);
    
    fprintf(file, "\n");

    fprintf(file, "----------Response Time----------\n");
    fprintf(file, "CPU max response time: %d\n", cpuMax);
    fprintf(file, "DISK 1 max response time: %d\n",disk1max);
    fprintf(file, "DISK 2 max response time: %d\n", disk2max);
    fprintf(file, "NETWORK max response time: %d\n", networkMax);
    fprintf(file, "CPU average response time: %f\n", (double)cpuTotal/(double)cpuCount);
    fprintf(file, "DISK 1 average response time: %f\n", (double)disk1Total/(double)disk1count);
    fprintf(file, "DISK 2 average response time: %f\n", (double)disk2Total/(double)disk2count);
    fprintf(file, "NETWORK average response time: %f\n", (double)networkTotal/(double)networkCount);
    fprintf(file, "\n");

    fprintf(file, "----------Throughput----------\n");
    double throughCpu = (double)cpuCount/(double)config.FIN_TIME;
    fprintf(file, "Throughput of CPU: %f\n", throughCpu);
    double throughDisk1 = (double)disk1count/(double)config.FIN_TIME;
    fprintf(file, "Throughput of DISK 1: %f\n", throughDisk1);
    double throughDisk2 = (double)disk2count/(double)config.FIN_TIME;
    fprintf(file, "Throughput of DISK 2: %f\n", throughDisk2);
    double throughNetwork = (double)networkCount/(double)config.FIN_TIME;
    fprintf(file, "Throughput of NETWORK: %f\n", throughNetwork);
    fclose(file);
}

//Done needs more testing
void jobArrival(struct event *popEvent, struct queue *currQueue){
    // printf("Job arrival: CALLLED\n");
    //checked works
    struct event *newEvent2;
    newEvent2 = malloc(sizeof(event));
    newEvent2->eventType = ARRIVAL; 
    newEvent2->ProcessID = eventid;
    eventid++;
    newEvent2->Time = getTime(config.ARRIVE_MIN, config.ARRIVE_MAX)+popEvent->Time;
    // newEvent2->Time = 80 + popEvent->Time;
    // printf("Time: %d\n", newEvent2->Time - popEvent->Time);
    // printf("Time: %d\n", getTime(config.ARRIVE_MIN, config.ARRIVE_MAX));
    push(newEvent2, currQueue);

    if(cpuOccupied == true || cpuQueue.count > 0){
        struct event *newEvent;
        newEvent = malloc(sizeof(event));
        newEvent->Time = popEvent->Time;
        newEvent->eventType = ARRIVAL;
        newEvent->ProcessID = popEvent->ProcessID;
        // printf("\nReached\n");
        push(newEvent, &cpuQueue);
    }else{
        struct event *newEvent;
        newEvent = malloc(sizeof(event));
        newEvent->Time = popEvent->Time;
        newEvent->ProcessID = popEvent->ProcessID;
        newEvent->eventType = ARRIVE_CPU;
        push(newEvent, currQueue);
    }
}

void cpuArrive(struct event *popEvent, struct queue *currQueue){
    struct event *newEvent; 
    newEvent = malloc(sizeof(event));
    newEvent->Time = getTime(config.CPU_MIN, config.CPU_MAX)+popEvent->Time;
    int cpuTime = newEvent->Time - popEvent->Time;
    cpuTotal +=  cpuTime;//for stats file
    if(cpuTime > cpuMax){
        cpuMax = cpuTime;
    }
    cpuCount++;
    newEvent->eventType = FINISH_CPU;
    newEvent->ProcessID = popEvent->ProcessID;
    cpuOccupied = true;
    push(newEvent, currQueue);
}

void cpuFinish(struct event *popEvent, struct queue *currQueue){
    //declaring probabilities
    cpuOccupied = false;
    double quitProb = config.QUIT_PROB * 100;
    int random = getTime(0, 100);
    int newRandom = getTime(0, (100-quitProb));
    double networkProb = config.NETWORK_PROB * 100-quitProb;
    printf("CPU COUNT: %d\n", cpuQueue.count);
    if(cpuQueue.count > 0){
        struct event *popCpuEvent;
        popCpuEvent = malloc(sizeof(event));
        popCpuEvent->Time =  popEvent->Time;
        popCpuEvent->eventType = ARRIVE_CPU;
        popCpuEvent->ProcessID = cpuQueue.head->ProcessID;
        printf("PID: %d\n", popCpuEvent->ProcessID);
        push(popCpuEvent, currQueue); 
        pop(&cpuQueue);
    }
    if(random <= quitProb){
        cpuOccupied = false; 
        struct event *newEvent2;
        newEvent2 = malloc(sizeof(event));
        newEvent2->ProcessID = popEvent->ProcessID;
        newEvent2->eventType = EXIT_SYSTEM; 
        newEvent2->Time = popEvent->Time;
        push(newEvent2, currQueue);
    }
    else if(newRandom <= networkProb){
        if(networkQueue.count == 0 && networkOccupied == false){
            cpuOccupied = false;
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = ARRIVE_NETWORK; 
            newEvent2->Time = popEvent->Time;
            networkOccupied = true;
            push(newEvent2, currQueue);
        }
        else{
            push(popEvent, &networkQueue);
        }
    }
    else{
        if(disk1Occupied == false && disk1Queue.count == 0){
            // printf("\n\nDisk 1 reached\n\n");
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = ARRIVE_DISK1; 
            newEvent2->Time = popEvent->Time;
            disk1Occupied = true;
            push(newEvent2, currQueue);
        }
        else if(disk2Occupied == false && disk2Queue.count == 0){
            // printf("\nDisk 2 reached\n\n");
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = ARRIVE_DISK2; 
            newEvent2->Time = popEvent->Time;
            disk2Occupied = true;
            push(newEvent2, currQueue);
        }
        else if(disk1Queue.count < disk2Queue.count){
            //goes to disk 1 queue
            // printf("\n\nDisk 1 queue reached\n\n");
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = popEvent->eventType;
            newEvent2->Time = popEvent -> Time;
            push(newEvent2, &disk1Queue);
        }
        else{//goes to disk 2 queue
            struct event *newEvent2;
            newEvent2 = malloc(sizeof(event));
            newEvent2->ProcessID = popEvent->ProcessID;
            newEvent2->eventType = popEvent->eventType;
            newEvent2->Time = popEvent -> Time;
            push(newEvent2, &disk2Queue);
        }
    }
}

void exitSystem(struct event *popEvent, struct queue *ueue){
    printf("Exit System works");
}

void arriveDisk1(struct event *popEvent, struct queue *currQueue){
    struct event *newEvent;
    newEvent = malloc(sizeof(event));
    newEvent->eventType = FINISH_DISK1;
    newEvent->Time = getTime(config.DISK1_MIN, config.DISK1_MAX)+popEvent->Time;
    int disk1Time = newEvent->Time - popEvent->Time;
    disk1Total += disk1Time;
    if(disk1Time > disk1max){
        disk1max = disk1Time;
    }
    disk1count++;
    newEvent->ProcessID = popEvent->ProcessID;
    disk1Occupied = true;
    push(newEvent, currQueue);
}

void arriveDisk2(struct event *popEvent, struct queue *currQueue){
    struct event *newEvent;
    newEvent = malloc(sizeof(event));
    newEvent->eventType = FINISH_DISK2;
    newEvent->Time = getTime(config.DISK2_MIN, config.DISK2_MAX)+popEvent->Time;
    int disk2Time = newEvent->Time - popEvent->Time;
    disk2Total += disk2Time;
    if(disk2Time > disk2max){
       disk2max = disk2Time;
    }
    disk2count++;
    newEvent->ProcessID = popEvent->ProcessID;
    disk2Occupied = true;
    push(newEvent, currQueue);
}

void finishDisk1(struct event *popEvent, struct queue *currQueue){
    disk1Occupied = false; 
    if(disk1Queue.count > 0){
        struct event *newEvent2;
        newEvent2 = malloc(sizeof(event));
        newEvent2->Time = popEvent->Time;
        // newEvent2->Time = getTime(config.DISK1_MIN, config.DISK1_MAX)+popEvent->Time;
        newEvent2->eventType = ARRIVE_DISK1;
        newEvent2->ProcessID = disk1Queue.head->ProcessID;
        push(newEvent2, currQueue);
        pop(&disk1Queue);
    }
}

void finishDisk2(struct event *popEvent, struct queue *currQueue){
    disk2Occupied = false; 
    if(disk2Queue.count > 0){
        struct event *newEvent2;
        newEvent2 = malloc(sizeof(event));
        newEvent2->Time = popEvent->Time;
        newEvent2->eventType = ARRIVE_DISK2;
        newEvent2->ProcessID = disk2Queue.head->ProcessID;
        push(newEvent2, currQueue);
        pop(&disk2Queue);
    }
}

void arriveNetwork(struct event *popEvent, struct queue *currQueue){
    struct event *newEvent;
    newEvent = malloc(sizeof(event));
    newEvent->eventType = FINISH_NETWORK;
    newEvent->Time = getTime(config.NETWORK_MIN, config.NETWORK_MAX)+popEvent->Time;
    int networkTime = newEvent->Time - popEvent->Time;  
    printf("network time \n %d", networkTime);
    networkTotal += networkTime;
    if(networkTime > networkMax){
        networkMax = networkTime;
    }
    networkCount++;
    newEvent->ProcessID = popEvent->ProcessID;
    push(newEvent, currQueue);
}

void finishNetwork(struct event *popEvent, struct queue *currQueue){
    networkOccupied = false;
    if(networkQueue.count > 0){
        struct event *newEvent2;
        newEvent2 = malloc(sizeof(event));
        newEvent2->Time = popEvent->Time;
        newEvent2->eventType = ARRIVE_NETWORK;
        newEvent2->ProcessID = networkQueue.head->ProcessID;
        networkOccupied = true;
        push(newEvent2, currQueue);
        printf("net queue size: %d",networkQueue.count);
        // pop(&networkQueue);
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
    // printf("%s %d\n", variable, config.DISK2_MAX);

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
    if(que->count > 1){
        // printf("Pop: Program complete. Exiting...");
        // exit(0);
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
    }else if(que->count == 1){
        struct event poppedEvent = *que->head;
        initQueue(que);
        return poppedEvent;
    }else{
        printf("Cannot pop an event from an empty queue");
        exit(0);
    }
    
}

//start of priority queue methods
//Done and partially tested
void sortEventQueue(struct queue *que){
    //get first node
    //compare to second node
    //if second < first then swap
    int ind;
    int i;

    struct event *first = que->head;
    struct event *next = NULL;

    if(first == NULL){
        return;
    }

    do{
        ind = 0; //indicator of swap
        first = que->head;
        while(first->next != next){
            if(first->Time > first->next->Time){
                swap(first, first->next);
                ind = 1;
            }
            first = first->next;
        }
        next = first;
    }while(ind);
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
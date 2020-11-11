#include <stdlib.h> 
#include <stdio.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h>
#include <string.h>
#include <stdlib.h> 
#include <limits.h>

//........................................................
//...qqqqqqqq.uuuu...uuu...eeeeee..euuu...uuu...eeeeee....
//..qqqqqqqqq.uuuu...uuu..eeeeeeee.euuu...uuu..eeeeeeee...
//.qqqqqqqqqq.uuuu...uuu.eeeeeeeeeeeuuu...uuu.eeeeeeeeee..
//.qqqqq.qqqq.uuuu...uuu.eeee..eeeeeuuu...uuu.eeee..eeee..
//.qqqq...qqq.uuuu...uuu.eeeeeeeeeeeuuu...uuu.eeeeeeeeee..
//.qqqq...qqq.uuuu...uuu.eeeeeeeeeeeuuu...uuu.eeeeeeeeee..
//.qqqq..qqqq..uuu..uuuu.eeee.......uuu..uuuu.eeee........
//.qqqqqqqqqq..uuuuuuuuu.eeeeeeeeee.uuuuuuuuu.eeeeeeeeee..
//..qqqqqqqqq..uuuuuuuuu.eeeeeeeee..uuuuuuuuu.eeeeeeeee...
//..qqqqqqqqq..uuuuuuuuu..eeeeeeee..uuuuuuuuu..eeeeeeee...
//....qqq.qqq....uuu........eeee......uuu........eeee.....
//........qqq.............................................
//........qqq.............................................
//........qqq.............................................
//........................................................

struct Queue { 
    struct QNode *front, *rear; // start and end of queue
}; 

struct QNode { 
    int key; 
    struct QNode* next; 
};

struct QNode* newNode(int k) 
{ 
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode)); 
    temp->key = k; 
    temp->next = NULL; 
    return temp; 
} 

struct Queue* createQueue() 
{ 
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue)); 
    q->front = q->rear = NULL; 
    return q; 
} 

void enQueue(struct Queue* q, int k) 
{ 
    // Create a new LL node 
    struct QNode* temp = newNode(k); 
  
    // If queue is empty, then new node is front and rear both 
    if (q->rear == NULL) { 
        q->front = q->rear = temp; 
        return; 
    } 
    // Add the new node at the end of queue and change rear 
    q->rear->next = temp; 
    q->rear = temp; 
} 

void deQueue(struct Queue* q) 
{ 
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
        return; 
  
    // Store previous front and move front one node ahead 
    struct QNode* temp = q->front; 
  
    q->front = q->front->next; 
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
        q->rear = NULL; 
    
    free(temp); 
} 

///////////CEILING FUNCTION////////////

int ceiling_half(int num){
    if (num % 2 == 0) {
        return num/2;
    }
    return (num+1)/2; 
}

//..................................................
//............lll.........................000000....
//............lll........................00000000...
//............lll........................00000000...
//...aaaaaaa..lll...gggggggg...ooooooo..o000..0000..
//..aaaaaaaa..lll..ggggggggg..ooooooooo.o000..0000..
//.aaaaaaaaaa.lll.lggggggggg.gooooooooooo000..0000..
//.aaaa.aaaaa.lll.lggg..gggg.gooo...ooooo000..0000..
//..aaaaaaaaa.lll.lggg..gggg.gooo...ooooo000..0000..
//.aaaaaaaaaa.lll.lggg...ggg.gooo...ooooo000..0000..
//.aaaaa.aaaa.lll.lggg..gggg.gooo...ooooo000..0000..
//.aaaa.aaaaa.lll.lggggggggg.gooooooooooo000000000..
//.aaaaaaaaaa.lll..ggggggggg..ooooooooo..00000000...
//.aaaaaaaaaa.lll...gggggggg...ooooooo...0000000....
//...aaa..........lgggg.gggg....ooooo......0000.....
//................lggggggggg........................
//.................ggggggggg........................
//.................gggggggg.........................
//..................................................

void algo_0 (int ** data, int num_process, char* op_file) {
    struct Queue* ready_queue = createQueue();
    int time = 0;
    FILE *output_file = fopen(op_file, "w");

    char output_str[500];
    char pstate[100];
    int ifputs;
    int ifidle;
    int numidle = 0;

    int start_time[num_process];
    int end_time[num_process];
    int process_state[num_process]; // READY: 1,   RUNNING: 2,   BLOCKED: 3,   RESTARTING: 4,   ENDED: 5
    int i = 0;
    for (i=0; i<num_process; i++){
        start_time[i] = INT_MAX;
        end_time[i] = INT_MAX;
        process_state[i] = 0;
    }

    while (1){
        // CHECK FOR READY AND ENDED STATE UPDATE
        int process_id = 0;
        for (process_id=0; process_id<num_process; process_id++){
            // if just starting or restarting after being blocked
            if (data[process_id][3] == time || (process_state[process_id] == 3 && time == end_time[process_id] + data[process_id][2])){
                enQueue(ready_queue, process_id);
                process_state[process_id] = 1; // READY
            } else if (process_state[process_id] == 4 && time == start_time[process_id] + ceiling_half(data[process_id][1])) {
                process_state[process_id] = 5; // ENDED
                end_time[process_id] = time;
                deQueue(ready_queue);
            }
        }

        // CHECK FOR BLOCKING STATE UPDATE  
        if (ready_queue->front != NULL){
            int process_id = ready_queue->front->key;
            int block_moment = start_time[process_id] + ceiling_half(data[process_id][1]);
            if (time == block_moment){
                process_state[process_id] = 3; // BLOCKING
                end_time[process_id] = time;
                deQueue(ready_queue);
            }
        }

        // CHECK FOR RUNNING OR RESTARTING STATE UPDATE
        if (ready_queue->front != NULL && process_state[ready_queue->front->key] == 1) { // still have at least one process ready
            int process_id = ready_queue->front->key;
            start_time[process_id] = time;
            if (end_time[process_id] == INT_MAX) { // just starting
                process_state[process_id] = 2; // RUNNING
            } else { // restarting
                process_state[process_id] = 4; // RESTARTING
            }
        }

        // output the state of each process
        sprintf(output_str, "%d", time);
        strcat(output_str, " ");
        ifputs = 0;
        ifidle = 1;
        for (process_id=0; process_id<num_process; process_id++){
            sprintf(pstate, "%d", data[process_id][0]);
            if (process_state[process_id] == 1) {
                ifputs = 1;
                strcat(pstate, ":ready ");
            } else if (process_state[process_id] == 2 || process_state[process_id] == 4) {
                ifputs = 1;
                strcat(pstate, ":running ");
                ifidle = 0;
            } else if (process_state[process_id] == 3) {
                ifputs = 1;
                strcat(pstate, ":blocked ");
            } else {continue; }
            strcat(output_str, pstate);
        }
        if (ifputs == 1){
            strcat(output_str, "\n");
            fputs(output_str, output_file);
            time+=1;
            if (ifidle == 1) {
                numidle += 1;
            }
        } else {
            // finishing time
            fputs("\nFinishing time: ", output_file);
            char final_time[100];
            sprintf(final_time, "%d", time-1);
            fputs(final_time, output_file);
            fputs("\n", output_file);
            // CPU utilization
            char str_utilization[10];
            float utilization = (time-numidle)*1.0000/time;
            sprintf(str_utilization, "%.2f", utilization);
            fputs("CPU utilization: ", output_file);
            fputs(str_utilization, output_file);
            for (process_id=0; process_id<num_process; process_id++){
                int turnaround = end_time[process_id] - data[process_id][3];
                char str_turnaround[100];
                sprintf(str_turnaround, "%d", turnaround);
                fputs("\nTurnaround process ", output_file);
                char str_processid[100];
                sprintf(str_processid, "%d", data[process_id][0]);
                fputs(str_processid, output_file);
                fputs(": ", output_file);
                fputs(str_turnaround, output_file);
            }
            numidle = 0;
            free(ready_queue);
            fclose(output_file);
            return;
        }
    }
}

//...............................................
//............lll...........................111..
//............lll..........................1111..
//............lll........................111111..
//...aaaaaaa..lll...gggggggg...ooooooo...111111..
//..aaaaaaaa..lll..ggggggggg..ooooooooo..111111..
//.aaaaaaaaaa.lll.lggggggggg.goooooooooo....111..
//.aaaa.aaaaa.lll.lggg..gggg.gooo...oooo....111..
//..aaaaaaaaa.lll.lggg..gggg.gooo...oooo....111..
//.aaaaaaaaaa.lll.lggg...ggg.gooo...oooo....111..
//.aaaaa.aaaa.lll.lggg..gggg.gooo...oooo....111..
//.aaaa.aaaaa.lll.lggggggggg.goooooooooo....111..
//.aaaaaaaaaa.lll..ggggggggg..ooooooooo.....111..
//.aaaaaaaaaa.lll...gggggggg...ooooooo......111..
//...aaa..........lgggg.gggg....ooooo............
//................lggggggggg.....................
//.................ggggggggg.....................
//.................gggggggg......................
//...............................................

void algo_1 (int ** data, int num_process, char* op_file) {
    struct Queue* ready_queue = createQueue();
    int time = 0;
    FILE *output_file = fopen(op_file, "w");

    char output_str[500];
    char pstate[100];
    int ifputs;
    int ifidle;
    int numidle = 0;

    int start_time[num_process];
    int end_time[num_process];
    int cumulative_running_time[num_process];
    int process_state[num_process]; // READY: 1,   RUNNING: 2,   BLOCKED: 3,   RESTARTING: 4,   ENDED: 5
    int i=0;
    for (i=0; i<num_process; i++){
        start_time[i] = INT_MAX;
        end_time[i] = INT_MAX;
        cumulative_running_time[i] = 0;
        process_state[i] = 0;
    }

    while (1){
        // CHECK FOR READY AND ENDED STATE UPDATE
        int process_id = 0;
        for (process_id=0; process_id<num_process; process_id++){
            // if just starting or restarting after being blocked
            if (data[process_id][3] == time || (process_state[process_id] == 3 && time == end_time[process_id] + data[process_id][2])){
                enQueue(ready_queue, process_id);
                process_state[process_id] = 1; // READY
            } else if (process_state[process_id] == 4 && time == start_time[process_id] + ceiling_half(data[process_id][1]) - cumulative_running_time[process_id]) {
                process_state[process_id] = 5; // ENDED
                end_time[process_id] = time;
                deQueue(ready_queue);
            }
        }

        // CHECK FOR BLOCKING STATE UPDATE  
        if (ready_queue->front != NULL){
            process_id = ready_queue->front->key;
            int block_moment = start_time[process_id] + ceiling_half(data[process_id][1]) - cumulative_running_time[process_id];
            if (time == block_moment){
                process_state[process_id] = 3; // BLOCKING
                end_time[process_id] = time; // update end time only when blocked, not when being switched by another process
                cumulative_running_time[process_id] = 0; // we deafult cumulative running time to 0 when being blocked
                deQueue(ready_queue);
            }
        }

        // CHECK IF CURRENTLY RUNNING PROCESS HAS BEEN RUNNING or RERUNNING FOR 2 CYCLES
        if (ready_queue->front != NULL && (process_state[ready_queue->front->key] == 2 || process_state[ready_queue->front->key] == 4)){ // still have at least one process running
            process_id = ready_queue->front->key;
            if (time - start_time[process_id] == 2){
                process_state[process_id] = 1; // READY
                deQueue(ready_queue);
                enQueue(ready_queue, process_id);
                cumulative_running_time[process_id] += time - start_time[process_id];
            }
        }

        // CHECK FOR RUNNING OR RESTARTING STATE UPDATE
        if (ready_queue->front != NULL && process_state[ready_queue->front->key] == 1) { // still have at least one process ready
            process_id = ready_queue->front->key;
            start_time[process_id] = time;
            if (end_time[process_id] == INT_MAX) { // just starting
                process_state[process_id] = 2; // RUNNING
            } else { // restarting
                process_state[process_id] = 4; // RESTARTING
            }
        }

        // output the state of each process
        sprintf(output_str, "%d", time);
        strcat(output_str, " ");
        ifputs = 0;
        ifidle = 1;
        for (process_id=0; process_id<num_process; process_id++){
            sprintf(pstate, "%d", data[process_id][0]);
            if (process_state[process_id] == 1) {
                ifputs = 1;
                strcat(pstate, ":ready ");
            } else if (process_state[process_id] == 2 || process_state[process_id] == 4) {
                ifputs = 1;
                strcat(pstate, ":running ");
                ifidle = 0;
            } else if (process_state[process_id] == 3) {
                ifputs = 1;
                strcat(pstate, ":blocked ");
            } else {continue; }
            strcat(output_str, pstate);
        }
        if (ifputs == 1){
            strcat(output_str, "\n");
            fputs(output_str, output_file);
            time+=1;
            if (ifidle == 1) {
                numidle += 1;
            }
        } else {
            // finishing time
            fputs("\nFinishing time: ", output_file);
            char final_time[100];
            sprintf(final_time, "%d", time-1);
            fputs(final_time, output_file);
            fputs("\n", output_file);
            // CPU utilization
            char str_utilization[10];
            float utilization = (time-numidle)*1.0000/time;
            sprintf(str_utilization, "%.2f", utilization);
            fputs("CPU utilization: ", output_file);
            fputs(str_utilization, output_file);
            for (process_id=0; process_id<num_process; process_id++){
                int turnaround = end_time[process_id] - data[process_id][3];
                char str_turnaround[100];
                sprintf(str_turnaround, "%d", turnaround);
                fputs("\nTurnaround process ", output_file);
                char str_processid[100];
                sprintf(str_processid, "%d", data[process_id][0]);
                fputs(str_processid, output_file);
                fputs(": ", output_file);
                fputs(str_turnaround, output_file);
            }
            numidle = 0;
            free(ready_queue);
            fclose(output_file);
            return;
        }
    }
}

//..................................................
//............lll.........................222222....
//............lll........................22222222...
//............lll........................222222222..
//...aaaaaaa..lll...gggggggg...ooooooo..o222..2222..
//..aaaaaaaa..lll..ggggggggg..ooooooooo.o222..2222..
//.aaaaaaaaaa.lll.lggggggggg.goooooooooo.....22222..
//.aaaa.aaaaa.lll.lggg..gggg.gooo...oooo....22222...
//..aaaaaaaaa.lll.lggg..gggg.gooo...oooo...222222...
//.aaaaaaaaaa.lll.lggg...ggg.gooo...oooo..222222....
//.aaaaa.aaaa.lll.lggg..gggg.gooo...oooo.22222......
//.aaaa.aaaaa.lll.lggggggggg.gooooooooooo222222222..
//.aaaaaaaaaa.lll..ggggggggg..ooooooooo.o222222222..
//.aaaaaaaaaa.lll...gggggggg...ooooooo..o222222222..
//...aaa..........lgggg.gggg....ooooo...............
//................lggggggggg........................
//.................ggggggggg........................
//.................gggggggg.........................
//..................................................

void algo_2 (int ** data, int num_process, char* op_file) {
    int time = 0;
    FILE *output_file = fopen(op_file, "w");

    char output_str[500];
    char pstate[100];
    int ifputs;
    int ifidle;
    int numidle = 0;

    int end_time[num_process];
    int remaining_time[num_process];
    int running_process_id = INT_MAX;
    int process_state[num_process]; // READY: 1,   RUNNING: 2,   BLOCKED: 3,   RESTARTING: 4,   ENDED: 5
    int i=0;
    for (i=0; i<num_process; i++){
        end_time[i] = INT_MAX;
        remaining_time[i] = INT_MAX;
        process_state[i] = 0;
    }

    while (1){
        // CHECK FOR READY AND ENDED STATE UPDATE
        int process_id=0;
        for (process_id=0; process_id<num_process; process_id++){
            // if just starting or restarting after being blocked
            if (data[process_id][3] == time || (process_state[process_id] == 3 && remaining_time[process_id] == ceiling_half(data[process_id][1]))){
                // add to priority queue, with value of remaining time
                process_state[process_id] = 1; // READY
                if (data[process_id][3] == time) {
                    remaining_time[process_id] = data[process_id][1] + data[process_id][2];
                    if (data[process_id][1] % 2 == 1) {remaining_time[process_id] += 1;} // since we need to round up both 0.5 * cpu_time, total time += 1
                }
            } else if (process_state[process_id] == 4 && remaining_time[process_id] == 0) {
                process_state[process_id] = 5; // ENDED
                end_time[process_id] = time;
                running_process_id = INT_MAX;
            }
        }

        // CHECK FOR BLOCKING STATE UPDATE  
        if (running_process_id != INT_MAX){
            if (ceiling_half(data[running_process_id][1]) + data[running_process_id][2] == remaining_time[running_process_id]){
                process_state[running_process_id] = 3; // BLOCKING
                running_process_id = INT_MAX;
            } else {
                process_state[running_process_id] = 1; // READY 
                running_process_id = INT_MAX;
            }
        }

        // CHECK FOR ALL REMAINING CPU TIME 
        int min_remaining_time = INT_MAX;
        for (process_id=0; process_id<num_process; process_id++){
            if (process_state[process_id] == 1 || process_state[process_id] == 2 || process_state[process_id] == 4) { // ready or running or restarting state
                int remaining_cpu = remaining_time[process_id];
                if (process_state[process_id] == 1 || process_state[process_id] == 2){
                    remaining_cpu -= data[process_id][2]; // minus block time
                }
                if (min_remaining_time > remaining_cpu) {
                    min_remaining_time = remaining_cpu;
                    running_process_id = process_id;
                }
            }
        }
        if (running_process_id != INT_MAX){
            if (remaining_time[running_process_id] == data[running_process_id][1] + data[running_process_id][2]) { // just starting
                process_state[running_process_id] = 2; // RUNNING
            } else { // restarting
                process_state[running_process_id] = 4; // RESTARTING
            }
        }

        // UPDATE EACH PROCESS PROCESS'S REMAINING TIME
        for (process_id=0; process_id<num_process; process_id++){
            if (process_state[process_id] == 2 || process_state[process_id] == 3 || process_state[process_id] == 4) {
            // we only update remaining time when the process is running, blocked or restarting 
                remaining_time[process_id] -= 1;
            }
        }

        // output the state of each process
        sprintf(output_str, "%d", time);
        strcat(output_str, " ");
        ifputs = 0;
        ifidle = 1;
        for (process_id=0; process_id<num_process; process_id++){
            sprintf(pstate, "%d", data[process_id][0]);
            if (process_state[process_id] == 1) {
                ifputs = 1;
                strcat(pstate, ":ready ");
            } else if (process_state[process_id] == 2 || process_state[process_id] == 4) {
                ifputs = 1;
                strcat(pstate, ":running ");
                ifidle = 0;
            } else if (process_state[process_id] == 3) {
                ifputs = 1;
                strcat(pstate, ":blocked ");
            } else {continue; }
            strcat(output_str, pstate);
        }
        if (ifputs == 1){
            strcat(output_str, "\n");
            fputs(output_str, output_file);
            time+=1;
            if (ifidle == 1) {
                numidle += 1;
            }
        } else {
            // finishing time
            fputs("\nFinishing time: ", output_file);
            char final_time[100];
            sprintf(final_time, "%d", time-1);
            fputs(final_time, output_file);
            fputs("\n", output_file);
            // CPU utilization
            char str_utilization[10];
            float utilization = (time-numidle)*1.0000/time;
            sprintf(str_utilization, "%.2f", utilization);
            fputs("CPU utilization: ", output_file);
            fputs(str_utilization, output_file);
            for (process_id=0; process_id<num_process; process_id++){
                int turnaround = end_time[process_id] - data[process_id][3];
                char str_turnaround[100];
                sprintf(str_turnaround, "%d", turnaround);
                fputs("\nTurnaround process ", output_file);
                char str_processid[100];
                sprintf(str_processid, "%d", data[process_id][0]);
                fputs(str_processid, output_file);
                fputs(": ", output_file);
                fputs(str_turnaround, output_file);
            }
            numidle = 0;
            fclose(output_file);
            return;
        }
    }
}

//...........................................
//...........................aii.............
//...........................aii.............
//...........................aii.............
//...........................aii.............
//.mmmmmmmmmmmmm....aaaaaaa..aii..innnnnnnn..
//.mmmmmmmmmmmmmm..maaaaaaa..aii..innnnnnnn..
//.mmmmmmmmmmmmmm.mmaaaaaaaa.aii..innnnnnnn..
//.mmmm.mmmm..mmm.mmaa.aaaaa.aii..innn.nnnn..
//.mmm..mmmm..mmm..maaaaaaaa.aii..inn...nnn..
//.mmm..mmmm..mmm.mmaaaaaaaa.aii..inn...nnn..
//.mmm..mmmm..mmm.mmaaa.aaaa.aii..inn...nnn..
//.mmm..mmmm..mmm.mmaa.aaaaa.aii..inn...nnn..
//.mmm..mmmm..mmm.mmaaaaaaaa.aii..inn...nnn..
//.mmm..mmmm..mmm.mmaaaaaaaa.aii..inn...nnn..
//..................aaa......................
//...........................................

int main(int argc, char ** argv) {
    int num_process = 0;
    int i = 0;
    FILE *fr = fopen(argv[1], "r");
    fscanf(fr, "%d", &num_process);
    int **data = (int **)malloc(num_process * 4 * sizeof(int));
    int j = 0;
    for (j=0;j<num_process;j++){
        data[j] = (int *)malloc(4 * sizeof(int));
    }
    while (i < num_process) {
        fscanf(fr, "%d %d %d %d", &data[i][0], &data[i][1], &data[i][2], &data[i][3]);
        i++;
    }
    argv[1][strlen(argv[1])-4] = '\0';
    if (strcmp(argv[2], "0") == 0){
        strcat(argv[1], "-0.txt");
        algo_0(data, num_process, argv[1]);
    } else if (strcmp(argv[2], "1") == 0){
        strcat(argv[1], "-1.txt");
        algo_1(data, num_process, argv[1]);
    } else if (strcmp(argv[2], "2") == 0){
        strcat(argv[1], "-2.txt");
        algo_2(data, num_process, argv[1]);
    }
    free(data);
    fclose(fr);
    
    return 0;
}
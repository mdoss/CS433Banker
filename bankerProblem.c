#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
const int P = 5;
const int R = 4;
 int processes[] = {0, 1, 2, 3, 4}; 
/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

bool request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);

/* resource checking*/
int request[NUMBER_OF_RESOURCES];
int release[NUMBER_OF_RESOURCES];

pthread_mutex_t mutex;
bool isSafe(int processes[], int avail[], int maxm[][R],
            int allot[][R]);



bool request_resources(int customer_num, int request[]) {

    printf("P%d requested: %d, %d, %d, %d\n", customer_num, request[0], request[1], request[2], request[3]);
    printf("Available: ");
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
        printf("%d ",available[i]);
    }
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
        if(request[i] <= need[customer_num][i]){
            if(request[i] > available[i]){
                printf("Unsafe request\n");
                return false;
            }
            else{
                for(int k = 0; k < NUMBER_OF_RESOURCES; k++){
                    allocation[customer_num][k] += request[k];
                    available[k] -= request[k];
                    need[customer_num][k] -= request[k];
                }
                if( isSafe(processes, available, maximum, allocation) ){
                    printf("Safe request, resources given\n");
                    printf("Available: \n");
                    for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
                        printf("%d ",available[i]);
                    }
                    return true;
                }
                else{
                    printf("Unsafe Request 2\n");
                    for(int k = 0; k < NUMBER_OF_RESOURCES; k++){
                        available[k] += request[k]; 
                        need[customer_num][k] += request[k];
                        allocation[customer_num][k] -= request[k];
                    }
                    return false;
                }
            }
        } else{
            return false;
        }

    }
 
    return true;
}
void calculateNeed(int need[P][R], int maxm[P][R], 
                   int allot[P][R])  //copy pasted to make the isSafe functon work
{ 
    // Calculating Need of each P 
    for (int i = 0 ; i < P ; i++) 
        for (int j = 0 ; j < R ; j++) 
  
            // Need of instance = maxm instance - 
            //                    allocated instance 
            need[i][j] = maxm[i][j] - allot[i][j]; 
} 

bool isSafe(int processes[], int avail[], int maxm[][R], 
            int allot[][R]) //copy pasted and edited from tutorial for java/c++ because all my implementations wouldnt work... this doesnt work properly either 
{	
    int need[P][R]; 
  
    // Function to calculate need matrix 
    calculateNeed(need, maxm, allot); 
  
	int finish[P];
    // Mark all processes as infinish
for(int i = 0; i < P; i++)
{ 
	finish[P] = 0;
 } 
    // To store safe sequence 
    int safeSeq[P]; 
  
    // Make a copy of available resources 
    int work[R]; 
    for (int i = 0; i < R ; i++) 
        work[i] = avail[i]; 
  
    // While all processes are not finished 
    // or system is not in safe state. 
    int count = 0; 
    while (count < P) 
    { 
        // Find a process which is not finish and 
        // whose needs can be satisfied with current 
        // work[] resources. 
        bool found = false; 
        for (int p = 0; p < P; p++) 
        { 
            // First check if a process is finished, 
            // if no, go for next condition 
            if (finish[p] == 0) 
            { 
                // Check if for all resources of 
                // current P need is less 
                // than work 
                int j; 
                for (j = 0; j < R; j++) 
                    if (need[p][j] > work[j]) 
                        break; 
  
                // If all needs of p were satisfied. 
                if (j == R) 
                { 
                    // Add the allocated resources of 
                    // current P to the available/work 
                    // resources i.e.free the resources 
                    for (int k = 0 ; k < R ; k++) 
                        work[k] += allot[p][k]; 
  
                    // Add this process to safe sequence. 
                    safeSeq[count++] = p; 
  
                    // Mark this p as finished 
                    finish[p] = 1; 
  
                    found = true; 
                } 
            } 
        } 
  
        // If we could not find a next process in safe 
        // sequence. 
        if (found == false) 
        { 
            printf( "System is not in safe state\n"); 
            return false; 
        } 
    } 
  
    // If system is in safe state then 
    // safe sequence will be as below 
    printf("System is in safe state.\nSafe sequence is: "); 
 //   for (int i = 0; i < P ; i++) 
  
    return true; 
} 

void release_resources(int customer_num, int release[]) {

    for(int i=0; i<NUMBER_OF_RESOURCES; i++) {
        available[i] = available[i] - request[i]; 
        allocation[customer_num][i] = allocation[customer_num][i] + request[i]; 
        need[customer_num][i] = need[customer_num][i] - request[i]; 
    }
}
void *requestThread(void *params) { 
    pthread_mutex_lock(&mutex);

    request_resources(params, request);
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
}
void *releaseThread(void *params) { 
    pthread_mutex_lock(&mutex);

    release_resources(params, request);
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
}

void printArrays(){
	printf("Max Array:\n");
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		printf("%d, %d, %d, %d\n", maximum[i][0], maximum[i][1], maximum[i][2], maximum[i][3]); 
	}
	printf("Available Array:\n");
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		printf("%d ", available[i]);
	}
	printf("\nAlocation Array:\n");
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
       	 	printf("%d, %d, %d, %d\n", allocation[i][0], allocation[i][1], allocation[i][2], allocation[i][3]);
	}
	printf("Need Array:\n");
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
        	printf("%d, %d, %d, %d\n", need[i][0], need[i][1], need[i][2], need[i][3]);
	}
}

int main(int argc, char* argv[]) {
    char command[2];
    int customerNum = 0, resource1 = 0, resource2 = 0, resource3 = 0, resource4 = 0;
    
    FILE *fp;
    fp = fopen("bankerInput.txt", "r");
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    {	
	for(int k = 0; k < NUMBER_OF_RESOURCES; k++)
	{
	    fscanf(fp, "%d", &maximum[i][k]);
	    available[k] = atoi(argv[k + 1]);
	    need[i][k] = maximum[i][k];
	}
	fgetc(fp);
    }
    fclose(fp);

    pthread_t Customer[NUMBER_OF_CUSTOMERS];

    pthread_attr_t attr;

    pthread_attr_init(&attr);

    pthread_mutex_init(&mutex, NULL);
    while(strcmp(command, "quit") != 0)
{	
    printf("Type 'quit' to quit\n");
    scanf("%s", &command);
    if(strcmp(command, "*") == 0)
    {
	printArrays();
    }
    else if(strcmp(command, "RQ") == 0)
    {
	scanf("%d %d %d %d %d",&customerNum, &request[0], &request[1], &request[2], &request[3]);
	printf("request\n");
	pthread_create(&Customer[customerNum], &attr, requestThread, customerNum);
    }
    else if(strcmp(command, "RL") == 0)
    {
	scanf("%d %d %d %d %d",&customerNum, &request[0], &request[1], &request[2], &request[3]);
        printf("release\n");
      	pthread_create(&Customer[customerNum], &attr, releaseThread, customerNum);
    }
}
    pthread_mutex_destroy(&mutex);
}


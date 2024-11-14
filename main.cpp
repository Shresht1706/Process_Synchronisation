#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // For sleep function
#include <pthread.h> // For threading

#define Num_Users 5 // Number of users (threads)
#define Num_Jobs_Per_User 10
#define Job_Length_Types 3
#define Num_Shared_Var 2

typedef enum { PRINT, SCAN } JobType;

typedef struct {
    char user;
    JobType job_type;
    int length;
    int arrival_time;
} Job;

int printer = 0;
int scanner = 0;


// Function to generate and print 10 jobs for a user
void *generate_job(void *arg) {
    char user = *(char *)arg;

    for (int i = 0; i < Num_Jobs_Per_User; i++) {
        Job job;
        job.user = user;
        job.job_type = (rand() % 2 == 0) ? PRINT : SCAN;

        // Determine job length
        int length_type = rand() % Job_Length_Types;
        int min_pages, max_pages;
        switch (length_type) {
            case 0:  // SHORT
                min_pages = 1;
            max_pages = 5;
            break;
            case 1:  // MEDIUM
                min_pages = 6;
            max_pages = 15;
            break;
            case 2:  // LARGE
                min_pages = 16;
            max_pages = 50;
            break;
        }
        job.length = min_pages + (rand() % (max_pages - min_pages + 1));

        // Calculate arrival time as a random number between 1 and 5 + (5 * i)
        job.arrival_time = (rand() % 5 + 1) + (5 * i);

        // Print job details
        printf("User: %c, Job %d, Type: %s, Length: %d, Arrival Time: %d\n",
               job.user, i + 1,
               (job.job_type == PRINT ? "PRINT" : "SCAN"),
               job.length, job.arrival_time);
    }

    return NULL;
}

// Function to simulate a job


int main() {
    srand(time(NULL)); // Seed random number generator

    printf("\nExecuting jobs without any synchronization:\n");

    pthread_t threads[Num_Users]; // Array to hold thread IDs
    char users[Num_Users] = { '1', '2', '3', '4', '5' }; // User identifiers

    // Create threads for each user
    for (int i = 0; i < Num_Users; i++) {
        pthread_create(&threads[i], NULL, generate_job, (void *)&users[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < Num_Users; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
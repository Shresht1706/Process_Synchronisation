#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // For sleep function
#include <pthread.h> // For threading

#define NUM_USERS 5 // Number of users (threads)
#define NUM_JOBS_PER_USER 10
#define JOB_LENGTH_TYPES 3

typedef enum { PRINT, SCAN } JobType;

typedef struct {
    char user;
    JobType job_type;
    int length;
} Job;

// Shared variables to demonstrate race condition
int printer = 0;
int scanner = 0;

// Function to generate a random job
Job generate_job(char user) {
    Job job;
    job.user = user;
    job.job_type = (rand() % 2 == 0) ? PRINT : SCAN;

    // Randomly determine job length
    int length_type = rand() % JOB_LENGTH_TYPES;
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
    return job;
}

// Function to simulate a job
void* execute_jobs(void* arg) {
    char user = *(char*)arg; // Retrieve the user character
    Job jobs[NUM_JOBS_PER_USER];

    // Generate jobs for the user
    for (int i = 0; i < NUM_JOBS_PER_USER; i++) {
        jobs[i] = generate_job(user);
        printf("User        %c generated job: %s with length %d pages.\n",
               jobs[i].user,
               jobs[i].job_type == PRINT ? "Print" : "Scan",
               jobs[i].length);
    }

    // Execute jobs
    for (int i = 0; i < NUM_JOBS_PER_USER; i++) {
        Job current_job = jobs[i];
        printf("User        %c started job: %s with length %d pages.\n",
               current_job.user,
               current_job.job_type == PRINT ? "Print" : "Scan",
               current_job.length);

        // Simulate job execution page by page
        for (int page = 1; page <= current_job.length; page++) {
            sleep(1); // Simulate processing time for 1 second
            if (current_job.job_type == PRINT) {
                printf("User        %c printed page %d.\n", current_job.user, page);
            } else {
                printf("User        %c scanned page %d.\n", current_job.user, page);
            }

            // Introduce a race condition by modifying a shared resource
            printer += 1; // Increment shared resource without synchronization
            printf("User         %c updated shared resource to %d.\n", current_job.user, printer);
        }

        printf("User         %c finished job: %s with length %d pages.\n\n",
               current_job.user,
               current_job.job_type == PRINT ? "Print" : "Scan",
               current_job.length);
    }

    return NULL; // Return from thread
}

int main() {
    srand(time(NULL)); // Seed random number generator

    printf("\nExecuting jobs without any synchronization:\n");

    pthread_t threads[NUM_USERS]; // Array to hold thread IDs
    char users[NUM_USERS] = { '1', '2' }; // User identifiers

    // Create threads for each user
    for (int i = 0; i < NUM_USERS; i++) {
        pthread_create(&threads[i], NULL, execute_jobs, (void *)&users[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_USERS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
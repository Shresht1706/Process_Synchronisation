#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // For sleep function
#include <pthread.h> // For threading

#define Num_Users 5           // Number of users (threads)
#define Num_Jobs_Per_User 10  // Number of jobs per user
#define Job_Length_Types 3

typedef enum { PRINT, SCAN } JobType;

typedef struct {
    char user;
    JobType job_type;
    int length;
    int arrival_time;
} Job;

pthread_mutex_t array_mutex = PTHREAD_MUTEX_INITIALIZER; //only for adding to the lists

Job all_jobs[Num_Users * Num_Jobs_Per_User];
int job_index = 0;

int printer = 0;
int scanner = 0;

// Function to generate a single job for a user
Job generate_job(char user, int job_index) {
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

    // Calculate arrival time as a random number between 1 and 5 + (5 * job_index)
    job.arrival_time = (rand() % 5 + 1) + (5 * job_index);

    return job;
}


void *Order_queue(void *arg) {
    char user = *(char *)arg;

    // Private array for this user's jobs
    Job user_jobs[Num_Jobs_Per_User];

    for (int i = 0; i < Num_Jobs_Per_User; i++) {
        // Generate a job
        user_jobs[i] = generate_job(user, i);

        // Print the job in the requested format
        printf("User : %c, Job : %s, Length : %d, Arrival Time : %d\n",
               user,
               (user_jobs[i].job_type == PRINT ? "Print" : "Scan"),
               user_jobs[i].length,
               user_jobs[i].arrival_time);
    }


    return NULL;
}

    // Function to execute 10 jobs for each user without synchronization
    void *execute_jobs(void *arg) {
        char user = *(char *)arg;

        for (int i = 0; i < Num_Jobs_Per_User; i++) {
            // Generate a job
            Job job = generate_job(user, i);

            // Wait for the job's arrival time to simulate delay
            //sleep(job.arrival_time);

            // Announce the start of the job
            //printf("User %c is starting a %s job. Length: %d pages, Arrival Time: %d\n",
            //job.user, (job.job_type == PRINT ? "PRINT" : "SCAN"), job.length, job.arrival_time);

            // Check job type and process each page with a delay
            if (job.job_type == PRINT) {
                for (int page = 1; page <= job.length; page++) {
                    printf("User %c printing page %d of %d\n", job.user, page, job.length);
                    sleep(1);  // Simulate time taken to print each page
                }
                printf("User %c finished printing %d pages.\n", job.user, job.length);
            } else {
                for (int page = 1; page <= job.length; page++) {
                    printf("User %c scanning page %d of %d\n", job.user, page, job.length);
                    sleep(1);  // Simulate time taken to scan each page
                }
                printf("User %c finished scanning %d pages.\n", job.user, job.length);
            }
        }

        return NULL;
    }

}

int main() {
    srand(time(NULL)); // Seed random number generator

    printf("\nExecuting jobs without any synchronization:\n");

    pthread_t threads[Num_Users]; // Array to hold thread IDs
    char users[Num_Users] = { '1', '2', '3', '4', '5' }; // User identifiers

    // Create threads for each user
    for (int i = 0; i < Num_Users; i++) {
        pthread_create(&threads[i], NULL, Order_queue, (void *)&users[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < Num_Users; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

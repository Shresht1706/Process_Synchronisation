#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // For sleep function
#include <pthread.h> // For threading
#include <queue>>
#include <iostream>

#define Num_Users 5           // Number of users (threads)
#define Num_Jobs_Per_User 10  // Number of jobs per user
#define Job_Length_Types 3
#define Time_Quantum 2




typedef enum { PRINT, SCAN } JobType;

typedef struct {
    char user;
    JobType job_type;
    int length;
    int arrival_time;
    int waiting_time;
} Job;

pthread_mutex_t array_mutex = PTHREAD_MUTEX_INITIALIZER; //only for adding to the lists

std::queue<Job> printer_queue; // Global queue for printer jobs
std::queue<Job> scanner_queue; // Global queue for scanner jobs


int printer = 0;
int scanner = 0;

std::queue<Job> job_queue;

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


// Function to generate jobs and categorize them into printer and scanner vectors
std::pair<std::vector<Job>, std::vector<Job>> order_queue(char user) {
    std::vector<Job> printer_jobs;
    std::vector<Job> scanner_jobs;

    for (int i = 0; i < Num_Users; i++) {
        char users[Num_Users] = { '1', '2', '3', '4', '5' };
    for (int i = 0; i < Num_Jobs_Per_User; i++) {
        Job job = generate_job(user, i);

        // Output the generated job
        std::cout << "User : " << user
                  << ", Job : " << (job.job_type == PRINT ? "Print" : "Scan")
                  << ", Length : " << job.length
                  << ", Arrival Time : " << job.arrival_time << std::endl;

        // Categorize job
        if (job.job_type == PRINT) {
            printer_jobs.push_back(job);
        } else {
            scanner_jobs.push_back(job);
        }
    }

    return {printer_jobs, scanner_jobs};
}




int main() {
    srand(time(NULL)); // Seed random number generator

    printf("\nExecuting jobs without any synchronization:\n");

    pthread_t threads[Num_Users]; // Array to hold thread IDs
    // User identifiers

    // Create threads for each user
    for (int i = 0; i < Num_Users; i++) {
        pthread_create(&threads[i], NULL, , (void *)&users[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < Num_Users; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

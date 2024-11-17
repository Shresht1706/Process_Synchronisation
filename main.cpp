#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // For sleep function
#include <thread>    // For threads
#include <chrono>    // For sleep_for
#include <vector>
#include <queue>
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
    int job_id;
} Job;

// Global queues for jobs
std::queue<Job> printer_queue;
std::queue<Job> scanner_queue;

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

    char users_id[Num_Users] = { '1', '2', '3', '4', '5' }; // User IDs array

    std::cout << "All Jobs - " << std::endl;

    for (int u = 0; u < Num_Users; u++) { // user loop
        char current_user = users_id[u];
        for (int i = 0; i < Num_Jobs_Per_User; i++) { // job loop
            Job job = generate_job(current_user, i);
            job.job_id = i+1;

            // Output the generated job
            std::cout << "User : " << current_user
                << ", Job : " << (job.job_type == PRINT ? "Print" : "Scan")
                << ", Job ID : " << job.job_id
                << ", Length : " << job.length
                << ", Arrival Time : " << job.arrival_time << std::endl;

            // Categorize job
            if (job.job_type == PRINT) {
                printer_jobs.push_back(job);
            } else {
                scanner_jobs.push_back(job);
            }
        }
    }

    // Sort printer and scanner jobs by arrival time
    std::sort(printer_jobs.begin(), printer_jobs.end(), [](Job a, Job b) {
        return a.arrival_time < b.arrival_time;
    });
    std::sort(scanner_jobs.begin(), scanner_jobs.end(), [](Job a, Job b) {
        return a.arrival_time < b.arrival_time;
    });

    // Output sorted printer jobs
    std::cout << "\nSorted Printer Jobs - " << std::endl;
    for (const auto& job : printer_jobs) {
        std::cout << "User  : " << job.user
                  << ", Job ID : " << job.job_id
                  << ", Length : " << job.length
                  << ", Arrival Time : " << job.arrival_time << std::endl;
    }

    // Output sorted scanner jobs
    std::cout << "\nSorted Scanner Jobs - " << std::endl;
    for (const auto& job : scanner_jobs) {
        std::cout << "User  : " << job.user
                  << ", Job ID : " << job.job_id
                  << ", Length : " << job.length
                  << ", Arrival Time : " << job.arrival_time << std::endl;
    }

    return {printer_jobs, scanner_jobs};
}

// Function to simulate processing on a shared resource
void process_job(std::queue<Job>& job_queue, const std::string& resource_name) {
    while (!job_queue.empty()) {
        Job current_job = job_queue.front();
        job_queue.pop();

        // Simulate processing 2 pages
        int pages_to_process = std::min(2, current_job.length);
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate processing time

        current_job.length -= pages_to_process;

        std::cout << "User " << current_job.user << " used " << resource_name << " for job id : " << current_job.job_id <<
                  " and processed 2 pages, pages left: " << current_job.length << std::endl;

        // If there are still pages left, re-enqueue the job
        if (current_job.length > 0) {
            job_queue.push(current_job);
        } else {
            std::cout << "User " << current_job.user << " " << "finished job" << current_job.job_id << std::endl;
        }
    }
}

void Unsynchronised_Execution(std::vector<Job> printerJobs, std::vector<Job> scannerJobs) {
    // Populate printer and scanner queues
    for (const auto& job : printerJobs) {
        printer_queue.push(job);
    }
    for (const auto& job : scannerJobs) {
        scanner_queue.push(job);
    }

    // Local variable to track the current time
    int current_time = 0;

    // Function to simulate processing on a shared resource
    auto process_job = [&current_time](std::queue<Job>& job_queue, const std::string& resource_name) {
        while (!job_queue.empty()) {
            Job current_job = job_queue.front();

            // Wait until the current time reaches the job's arrival time
            if (current_time < current_job.arrival_time) {
                std::this_thread::sleep_for(std::chrono::seconds(current_job.arrival_time - current_time));
                current_time = current_job.arrival_time; // Update current time to the job's arrival time
            }

            job_queue.pop();

            // Simulate processing 2 pages
            int pages_to_process = std::min(2, current_job.length);
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate processing time

            current_job.length -= pages_to_process;

            std::cout << "User  " << current_job.user << " used " << resource_name << " for job id : " << current_job.job_id <<
                      " and processed 2 pages, pages left: " << current_job.length << std::endl;

            // If there are still pages left, re-enqueue the job
            if (current_job.length > 0) {
                job_queue.push(current_job);
            } else {
                std::cout << "User  " << current_job.user << " finished job " << current_job.job_id << std::endl;
            }

            // Increment current time for the next job processing
            current_time += 2; // Assuming each job processing takes 2 seconds
        }
    };

    // Create threads for the printer and scanner
    std::thread printer_thread(process_job, std::ref(printer_queue), "Printer");
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Ensures no overlap in processing
    std::thread scanner_thread(process_job, std::ref(scanner_queue), "Scanner");

    // Wait for threads to complete
    printer_thread.join();
    scanner_thread.join();

    std::cout << "All jobs processed!" << std::endl;
}

void MutexesExecution(std::vector<Job> printerJobs, std::vector<Job> scannerJobs) {
    std::mutex resource_mutex; // Mutex to ensure exclusive access to the shared resource

    // Populate printer and scanner queues
    for (const auto& job : printerJobs) {
        printer_queue.push(job);
    }
    for (const auto& job : scannerJobs) {
        scanner_queue.push(job);
    }

    // Thread function for processing jobs with mutex
    auto process_job_with_mutex = [&](std::queue<Job>& job_queue, const std::string& resource_name) {
        while (true) {
            std::lock_guard<std::mutex> lock(resource_mutex); // Lock the mutex
            if (job_queue.empty()) {
                return; // Exit the loop if the queue is empty
            }
            process_job(job_queue, resource_name); // Call existing function
        }
    };

    // Create threads for the printer and scanner
    std::thread printer_thread(process_job_with_mutex, std::ref(printer_queue), "Printer");
    std::thread scanner_thread(process_job_with_mutex, std::ref(scanner_queue), "Scanner");

    // Wait for threads to complete
    printer_thread.join();
    scanner_thread.join();

    std::cout << "All jobs processed with mutexes!" << std::endl;
}

int main() {
    srand(time(NULL)); // Seed random number generator

    // Generate and categorize jobs
    auto [printerJobs, scannerJobs] = order_queue('A'); // printer and scanner job vectors

    // Call unsynchronized execution
    Unsynchronised_Execution(printerJobs, scannerJobs);

    return 0;
}

#include "src.hpp"
#include <limits.h>

//HELPER FUNCTION

// custom comparison function for qsort
// it tells qsort how to compare two Job structs
int compare_jobs(const void* a, const void* b ) {
    //cast void pointers to job pointers

    Job* jobA = (Job*)a;
    Job* jobB = (Job*)b;

    // If priority are different, the one with a lower priority comes first
    if (jobA->priority < jobB->priority) {
        return -1; // jobA comes before jobB
    }
    if (jobA->priority > jobB->priority) {
        return 1; // jobA comes after jobB
    }

    // IF the priority is the same, then compare which job came firt via idx
    if (jobA->idx < jobB->idx) {
        return -1; // jobA comes before jobB
    }
    if (jobA->idx > jobB->idx) {
        return 1; // jobA comes after jobB
    }
    
    // If they are identical in priority and index, consider them equal.
    return 0;
}


// Priority Scheduling 
//   -- Jobs with highest priority (lowest number) run first
//   -- If multiple jobs with same priority, lowest index runs first
void priority(int n_jobs, Job* jobs)
{
    Job* sorted_jobs = (Job*)malloc(n_jobs * sizeof(Job));
    memcpy(sorted_jobs, jobs, n_jobs *sizeof(Job));

    qsort(sorted_jobs, n_jobs, sizeof(Job), compare_jobs);

    // now run jobs in newly sorted order
    for (int i =0; i < n_jobs; i++) {
        sorted_jobs[i].run_job(sorted_jobs[i].time);
    }

    //clean up heap
    free(sorted_jobs);
}

// Helper function for qsort to sort jobs by their arrival IDX
int compare_jobs_by_idx(const void* a, const void* b) {
    Job* jobA = (Job*)a;
    Job* jobB = (Job*)b;
    if (jobA->idx < jobB->idx) return -1;
    if (jobA->idx > jobB->idx) return 1;
    return 0;
}

// Priority Scheduling with Round Robin 
//    -- Jobs with highest priority (lowest number) run first
//    -- If multiple jobs have same priority, run all in round robin 
//    -- Time slice for round robin passed as a parameter
void priority_rr(int n_jobs, Job* jobs, int time_slice)
{
    Job* active_jobs = (Job*)malloc(n_jobs * sizeof(Job));
    memcpy(active_jobs, jobs, n_jobs * sizeof(Job));
    int jobs_remaining = n_jobs;


    // main scheddy loop that continues till all jobs are done
    while (jobs_remaining > 0) {

        // Find the highest priority level among all remaining jobs
        int highest_priority = INT_MAX;
        for (int i = 0; i < n_jobs; ++i) {
            // Only consider jobs that still have time left.
            if (active_jobs[i].time > 0 && active_jobs[i].priority < highest_priority) {
                highest_priority = active_jobs[i].priority;
            }
        }

        // Build a temporary queue of all jobs at that priority level
        Job current_queue[n_jobs];
        int queue_size = 0;
        for (int i = 0; i < n_jobs; ++i) {
            if (active_jobs[i].time > 0 && active_jobs[i].priority == highest_priority) {
                current_queue[queue_size++] = active_jobs[i];
            }
        }
        
        // Sort this queue by 'idx' to ensure FIFO for the RR
        qsort(current_queue, queue_size, sizeof(Job), compare_jobs_by_idx);

        // 5. Execute the RR cycle for this priority level
        int jobs_in_queue = queue_size;
        while (jobs_in_queue > 0) {
            for (int i = 0; i < queue_size; ++i) {
                
                // Skip jobs that are already finished in this queue
                if (current_queue[i].time <= 0) continue;

                // Find the corresponding job in our main active_jobs array
                for (int j = 0; j < n_jobs; ++j) {
                    if (active_jobs[j].idx == current_queue[i].idx) {
                        
                        // Determine run time by either a full time_slice or the remaining time
                        int run_time = (active_jobs[j].time < time_slice) ? active_jobs[j].time : time_slice;

                        // Run the job
                        active_jobs[j].run_job(run_time);
                        
                        // Update remaining time
                        active_jobs[j].time -= run_time;
                        current_queue[i].time -= run_time;

                        // If the job just finished, decrement our counters
                        if (active_jobs[j].time <= 0) {
                            jobs_remaining--;
                            jobs_in_queue--;
                        }
                        break; // Move to the next job in the queue
                    }
                }
            }
        }
    }

    // 6. Clean up the heapg
    free(active_jobs);

}


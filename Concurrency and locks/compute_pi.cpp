#include "src.hpp"
#include <stdlib.h>

void* compute_pi(void* arg)
{
    thread_data_t* data = (thread_data_t*)(arg);
    int thread_id = data->thread_id;
    calc_t* pi_data = data->pi_calc;
    int n_threads = pi_data -> global_n_threads;

    // Task: Update pi_data->global_sum to hold the number of random (x,y) pairs 
    // that fall within the unit circle
    // pi_data->global_n_samples is the total number of (x,y) pairs to be calculated
    // among all threads, combined.
    // Note: you must call thread_rand(), which will give you a thread safe version of rand()
    
    // casting to long long for rounding saftey during mult
    long long n_samples_total = (long long)pi_data -> global_n_samples;

    // determin this threads workload using block dsitrobution
    // cleanly divides the work without a special if case for the last thread
    long long start_index = (thread_id * n_samples_total) / n_threads;
    long long end_index = ((thread_id + 1) * n_samples_total) / n_threads;

    // the number of samples this specific thread will process
    int num_samples_for_this_thread = (int)(end_index - start_index);

    // this will store the count for this thread only
    // using int to match the type of pi_data->global_sum
    int local_sum = 0;
    double rand_x, rand_y;

    //denominator to scale random number from [0, RAND_MAX]
    // to [-1.0, 1.0] mtching serial_compute_pi
    double denom = (double)RAND_MAX * 0.5;

    // 2. perform this threads calculations. This look runs num_samples_for_this_thread times
    for (int i = 0; i < num_samples_for_this_thread; i++) {
        // thread_rand() atomically pulls the next pre-computer random number
        rand_x = ((double) thread_rand() / denom) - 1.0;
        rand_y = ((double) thread_rand() / denom) - 1.0;

        // check if the (x.y) pair is inside the unit circle
        if ( rand_x * rand_x + rand_y * rand_y <= 1.0) {
            local_sum++;
        }
    }

    // 3.) update the global sum ONCE
    // This is the critical section and the only part that reqires a lock
    lock(&(pi_data -> lock));
    pi_data -> global_sum += local_sum;
    unlock(&(pi_data -> lock));
    

    return NULL;
}


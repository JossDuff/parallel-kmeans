// parallelize manually with static load balancing

#include "static.hpp"
#include "constants.hpp"
#include <array>
#include <atomic>
#include <iostream>
#include <set>
#include <thread>
#include <vector>

using namespace std;

namespace {
bool is_prime(long n) {
    if (n <= 1) {
        return false;
    }

    // Loop from 2 up to n-1 to check for divisibility
    for (int i = 2; i < n; ++i) {
        if (n % i == 0) {
            return false; // found a divisor, so it's not prime
        }
    }

    return true; // no divisors found, so it's prime
}

void find_primes_in_vec(vector<long> nums, atomic<int> &prime_count) {
    int local_count = 0;
    for (long num : nums) {
        if (is_prime(num)) {
            local_count++;
        }
    }

    prime_count.fetch_add(local_count);
}

} // namespace

int find_primes_static(array<vector<long>, NUM_THREADS> static_load) {
    atomic<int> prime_count(0);

    vector<thread> threads;
    for (vector<long> nums : static_load) {
        threads.emplace_back(find_primes_in_vec, nums, ref(prime_count));
    }

    // wait for all threads to finish
    for (auto &t : threads) {
        t.join();
    }

    return prime_count.load();
}

array<vector<long>, NUM_THREADS> setup_static_naive(long n) {
    long nums_per_thread = n / NUM_THREADS;

    // vector of threads that hold a vector of the numbers that they must check
    array<vector<long>, NUM_THREADS> thread_assignments;

    // collect all numbers into a vector
    vector<long> numbers;

    for (long i = 1; i <= n; ++i) {
        numbers.push_back(i);
    }

    // next number to be assigned to a thread
    long next_number = 0;

    // assign a list of numbers to each thread
    for (int t = 0; t < NUM_THREADS; ++t) {

        // get this thread
        vector<long> &this_threads_assignment = thread_assignments[t];

        bool is_last_thread = t == NUM_THREADS - 1;
        if (is_last_thread) {
            // give remaining numbers to this thread for edge cases when
            // nums_per_thread isn't a round number
            for (long i = next_number; i <= n; i++) {
                this_threads_assignment.push_back(i);
            }

            break;
        }

        // give the next <nums_per_thread> numbers to this thread
        for (long i = next_number; i < next_number + nums_per_thread; i++) {
            this_threads_assignment.push_back(i);
        }

        next_number += nums_per_thread;

        // cout << "Thread " << t << " nums: " << this_threads_assignment.size()
        //      << endl;
    }

    return thread_assignments;
}

// If system is 32 bit there will be integer overflow
// returns an array of the numbers that each thread will make a prime check for
array<vector<long>, NUM_THREADS> setup_static(long n) {
    long total_work = 0;
    for (long i = 2; i <= n; ++i) {
        // each i in n checks for factors in 2..i-1
        // Compensate for the work of storing / loading the number by adding 10
        total_work += i + 10;
    }

    // For example, when n = 1 Million with 16 threads,
    // total_work = 1 782 293 665
    // work_per_thread = 111 393 354
    long work_per_thread = total_work / NUM_THREADS;

    // collect all numbers into a vector
    vector<long> numbers;
    for (long i = 1; i <= n; ++i) {
        numbers.push_back(i);
    }

    // vector of threads that hold a vector of the numbers that they must check
    array<vector<long>, NUM_THREADS> thread_assignments;

    // assign a list of numbers to each thread
    for (int t = 0; t < NUM_THREADS; ++t) {

        // get this thread
        vector<long> &this_threads_assignment = thread_assignments[t];

        // the last thread gets assigned all the remaining work
        bool is_last_thread = t == NUM_THREADS - 1;

        // index of numbers that have been assigned to a thread.  We later
        // remove these numbers from the list of all numbers so they're not
        // assigned to multiple threads
        set<long> indexes_to_remove;

        // how much work has been assigned to this thread
        long assigned_work = 0;

        // assign numbers greedily by filling in the largest numbers first
        for (long i = numbers.size() - 1; i >= 0; --i) {
            long number = numbers[i];
            long work = number + 10;
            long work_remaining = work_per_thread - assigned_work;

            // assign the last thread all remaining numbers.  This shouldn't be
            // too much because we iterated from largest to smallest
            if (is_last_thread) {
                this_threads_assignment.push_back(number);
                assigned_work += work;
                indexes_to_remove.insert(i);
                continue;
            }

            // this thread can be assigned this work
            if (work <= work_remaining) {
                this_threads_assignment.push_back(number);
                assigned_work += work;
                // this number has been assigned to a thread.  Make sure it's
                // not assigned to another thread by removing it from the vector
                indexes_to_remove.insert(i);
            } else {
                // Can't fit this much work
                // check lower numbers
                continue;
            }
        }

        // remove numbers that have already been assigned to threads
        vector<long> trimmed_numbers;
        for (size_t i = 0; i < numbers.size(); ++i) {
            if (indexes_to_remove.find(i) == indexes_to_remove.end()) {
                trimmed_numbers.push_back(numbers[i]);
            }
        }
        numbers = std::move(trimmed_numbers);

        // cout << "Thread " << t << " work: " << assigned_work << endl;
    }

    // cout << "In the end, numbers has " << numbers.size() << " elements" <<
    // endl;

    return thread_assignments;
}

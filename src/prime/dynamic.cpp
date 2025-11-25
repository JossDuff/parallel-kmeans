// parallelize manually with dynamic load balancing

#include "constants.hpp"
#include "static.hpp"
#include <array>
#include <atomic>
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

void find_primes_impl(vector<long> nums, atomic<size_t> &next_index,
                      atomic<int> &prime_count) {

    int local_count = 0;
    while (true) {
        size_t i = next_index.fetch_add(1);
        if (i >= nums.size())
            break;

        long n = nums[i];
        if (is_prime(n)) {
            local_count++;
        }
    }

    prime_count.fetch_add(local_count);
}

} // namespace

int find_primes_dynamic(vector<long> nums) {
    atomic<size_t> next_index{0};
    atomic<int> prime_count(0);

    vector<thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(find_primes_impl, nums, ref(next_index),
                             ref(prime_count));
    }

    // wait for all threads to finish
    for (auto &t : threads) {
        t.join();
    }

    return prime_count.load();
}

// sets up a vector of numbers starting at the largest n
vector<long> setup_dynamic(long n) {
    vector<long> nums;

    for (long i = n; i > 0; i--) {
        nums.push_back(i);
    }

    return nums;
}

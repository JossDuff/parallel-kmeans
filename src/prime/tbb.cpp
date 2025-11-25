#include "prime/tbb.hpp"
#include "tbb/tbb.h"
#include <vector>

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
} // namespace

int find_primes_tbb(long n) {
    std::atomic<int> prime_count(0);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, n + 1),
                      [&prime_count](const tbb::blocked_range<size_t> &range) {
                          size_t local_count = 0;
                          for (size_t i = range.begin(); i != range.end();
                               ++i) {
                              if (is_prime(i)) {
                                  local_count++;
                              }
                          }
                          prime_count += local_count;
                      });

    return prime_count.load();
}
// parallel_reduce impl was slightly slower than parallel_for with an atomic
/*
class PrimeCounter {
  public:
    int count;

    PrimeCounter() : count(0) {}
    PrimeCounter(PrimeCounter &other, tbb::split) : count(0) {}

    void operator()(const tbb::blocked_range<int> &range) {
        for (int i = range.begin(); i != range.end(); ++i) {
            if (is_prime(i)) {
                count++;
            }
        }
    }

    void join(const PrimeCounter &other) { count += other.count; }
};
} // namespace

int find_primes_tbb(long n) {
    PrimeCounter counter;
    tbb::parallel_reduce(tbb::blocked_range<int>(0, n + 1), counter);
    return counter.count;
}
*/

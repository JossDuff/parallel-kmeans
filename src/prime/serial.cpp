#include "prime/serial.hpp"

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

// naive prime checker in O(n^2)
int find_primes_serial(long n) {
    int prime_count = 0;
    for (long i = 1; i <= n; ++i) {
        if (is_prime(i)) {
            prime_count++;
        }
    }

    return prime_count;
}

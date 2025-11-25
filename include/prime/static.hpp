#include "constants.hpp"
#include <array>
#include <vector>

#ifndef PRIME_STATIC_HPP
#define PRIME_STATIC_HPP

using namespace std;

array<vector<long>, NUM_THREADS> setup_static_naive(long n);
array<vector<long>, NUM_THREADS> setup_static(long n);

int find_primes_static(array<vector<long>, NUM_THREADS>);

#endif

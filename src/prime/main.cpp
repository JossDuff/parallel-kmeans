#include "benchmark.hpp"
#include "constants.hpp"
#include "prime/dynamic.hpp"
#include "prime/serial.hpp"
#include "prime/static.hpp"
#include "prime/tbb.hpp"
#include <iostream>
#include <tbb/tbb.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }

    tbb::global_control gc(tbb::global_control::max_allowed_parallelism,
                           NUM_THREADS);

    long n = std::atol(argv[1]);
    std::cout << "\nRunning Prime with N = " << n << std::endl;

    int expected_primes = find_primes_serial(n);

    Benchmark serial("Prime serial");
    serial.run([=]() {
        int primes = find_primes_serial(n);

        if (primes != expected_primes) {
            cout << "FAILED: expected_primes = " << expected_primes
                 << ", serial execution primes = " << primes;
        }
    });
    serial.print_results();

    Benchmark tbb("Prime tbb parallel_for");
    tbb.run([=]() {
        int primes = find_primes_tbb(n);

        if (primes != expected_primes) {
            cout << "FAILED: expected_primes = " << expected_primes
                 << ", tbb parallel_for primes = " << primes << endl;
        }
    });
    tbb.print_comparison(serial);

    Benchmark static_load("Prime static load balancing");
    array<vector<long>, NUM_THREADS> static_load_alloc = setup_static(n);
    static_load.run([=]() {
        int primes = find_primes_static(static_load_alloc);

        if (primes != expected_primes) {
            cout << "FAILED: expected_primes = " << expected_primes
                 << ", static load balanced primes = " << primes;
        }
    });
    static_load.print_comparison(serial);

    Benchmark static_load_naive("Prime static naive load balancing");
    array<vector<long>, NUM_THREADS> static_load_alloc_naive =
        setup_static_naive(n);
    static_load_naive.run([=]() {
        int primes = find_primes_static(static_load_alloc_naive);

        if (primes != expected_primes) {
            cout << "FAILED: expected_primes = " << expected_primes
                 << ", static naive load balanced primes = " << primes;
        }
    });
    static_load_naive.print_comparison(serial);

    Benchmark dynamic_load("Prime dynamic load balancing");
    vector<long> nums = setup_dynamic(n);
    dynamic_load.run([=]() {
        int primes = find_primes_dynamic(nums);

        if (primes != expected_primes) {
            cout << "FAILED: expected_primes = " << expected_primes
                 << ", dynamic load balanced primes = " << primes;
        }
    });
    dynamic_load.print_comparison(serial);

    return 0;
}

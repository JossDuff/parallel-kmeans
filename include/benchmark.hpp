#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include "constants.hpp"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

class Benchmark {
  private:
    using clock = std::chrono::high_resolution_clock;
    using duration = std::chrono::duration<double>;

    std::vector<double> times;
    std::string name;
    clock::time_point start_time;

  public:
    explicit Benchmark(const std::string &benchmark_name)
        : name(benchmark_name) {}

    void start() { start_time = clock::now(); }

    void stop() {
        auto end_time = clock::now();
        duration diff = end_time - start_time;
        times.push_back(diff.count());
    }

    template <typename Func> void run(Func func) {
        // Warmup
        for (int i = 0; i < WARMUP_RUNS; i++) {
            func();
        }
        std::cout << "warm. Runs: " << std::flush;

        // Actual benchmarking
        times.clear();
        for (int i = 0; i < BENCH_RUNS; ++i) {
            start();
            func();
            stop();
            std::cout << ". " << std::flush;
        }
        std::cout << "\n";
    }

    double average() const {
        if (times.empty())
            return 0.0;
        return std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    }

    double median() const {
        if (times.empty())
            return 0.0;
        std::vector<double> sorted = times;
        std::sort(sorted.begin(), sorted.end());
        size_t n = sorted.size();
        if (n % 2 == 0) {
            return (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
        }
        return sorted[n / 2];
    }

    double min() const {
        if (times.empty())
            return 0.0;
        return *std::min_element(times.begin(), times.end());
    }

    double max() const {
        if (times.empty())
            return 0.0;
        return *std::max_element(times.begin(), times.end());
    }

    void print_results() const {
        std::cout << "Benchmark: " << name << "\n";
        std::cout << "  Iterations: " << times.size() << "\n";
        std::cout << "  Average:    " << std::fixed << std::setprecision(6)
                  << average() << " seconds\n";
        std::cout << "  Median:     " << median() << " seconds\n";
        std::cout << "  Min:        " << min() << " seconds\n";
        std::cout << "  Max:        " << max() << " seconds\n";
        std::cout << "  Speedup:    N/A\n";
    }

    void print_comparison(const Benchmark &baseline) const {
        std::cout << "Benchmark: " << name << "\n";
        std::cout << "  Iterations: " << times.size() << "\n";
        std::cout << "  Average:    " << std::fixed << std::setprecision(6)
                  << average() << " seconds\n";
        std::cout << "  Median:     " << median() << " seconds\n";
        std::cout << "  Min:        " << min() << " seconds\n";
        std::cout << "  Max:        " << max() << " seconds\n";
        std::cout << "  Speedup:    " << std::fixed << std::setprecision(2)
                  << baseline.average() / average() << "x\n";
    }
};

#endif // BENCHMARK_HPP

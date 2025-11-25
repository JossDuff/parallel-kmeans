# All data is sunlab & O3

# Naive sequential, no optimizations
Primes are stored in a vector so the run doesn't get optimized out.
Benchmarking is done with 3 warmup runs, then the next runs are timed

## n = 10k
  Iterations: 5
  Average:    0.011168 seconds
  Median:     0.011169 seconds
  Min:        0.011137 seconds
  Max:        0.011196 seconds

## n = 100k (10^5)
  Iterations: 5
  Average:    0.899906 seconds
  Median:     0.905985 seconds
  Min:        0.880318 seconds
  Max:        0.916620 seconds

## n = 1M (10^6)
  Iterations: 5
  Average:    72.670633 seconds
  Median:     72.654981 seconds
  Min:        72.651001 seconds
  Max:        72.706599 seconds

## n = 10M (10^7)
  Turned off warmup runs and used only 1 iteration because I didn't want to wait 8 hours for this test. This is okay because previous tests showed low standard deviation from the mean.
  Iterations: 1
  Average:    6197.060159 seconds
  Median:     6197.060159 seconds
  Min:        6197.060159 seconds
  Max:        6197.060159 seconds

## n = 100M (10^8)
  Not calculated because it would take roughly 167 hours.

This correctly shows exponential O(n^2) growth.  As N increases by a factor of 10, runtime increases by 10^2. 

# Basic TBB impl with parallel_for

## n = 10k
Benchmark: Prime tbb
  Iterations: 10
  Average:    0.001529 seconds
  Median:     0.001526 seconds
  Min:        0.001520 seconds
  Max:        0.001559 seconds
  Speedup:    7.15x

## n = 100k
Benchmark: Prime tbb
  Iterations: 10
  Average:    0.121613 seconds
  Median:     0.121557 seconds
  Min:        0.121494 seconds
  Max:        0.122015 seconds
  Speedup:    7.20x

## n = 1M (only 5 benchmarks)
Benchmark: Prime tbb
  Iterations: 5
  Average:    10.468615 seconds
  Median:     10.500953 seconds
  Min:        10.256447 seconds
  Max:        10.602895 seconds
  Speedup:    6.95x

## n = 10M (only 1 benchmark)
Benchmark: Prime tbb
  Iterations: 1
  Average:    914.973098 seconds
  Median:     914.973098 seconds
  Min:        914.973098 seconds
  Max:        914.973098 seconds
  Speedup:    6.77x

# Manual parallelization using std::thread with naive static load balancing
Evenly divide n across threads.  The last thread will have much more work to do than the first thread because isPrime() checks 1,i-1 for factors

## n = 10k
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    0.014286 seconds
  Median:     0.013138 seconds
  Min:        0.012618 seconds
  Max:        0.017031 seconds
  Speedup:    0.78x

## n = 100k
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    1.131167 seconds
  Median:     1.140020 seconds
  Min:        1.089998 seconds
  Max:        1.147790 seconds
  Speedup:    0.78x

## n = 1M
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    93.187937 seconds
  Median:     92.602221 seconds
  Min:        91.228844 seconds
  Max:        95.500705 seconds
  Speedup:    0.78x

## n = 10M
Benchmark: Prime static load balancing
  Iterations: 1
  Average:    7380.315412 seconds
  Median:     7380.315412 seconds
  Min:        7380.315412 seconds
  Max:        7380.315412 seconds
  Speedup: 0.84x

# Better static benchmarking.  Evenly distribute the entire workload.  This will perform better than dynamic because we have complete information on the entire workload
This technique (assign highest work first) has almost perfect work allocation:
Each number has an addition flat 10 work to compensate for load the number / storing it in a vector

Running Prime with N = 10000000
Thread 0 work: 3124999062500
Thread 1 work: 3124999062500
Thread 2 work: 3124999062500
Thread 3 work: 3124999062500
Thread 4 work: 3124999062500
Thread 5 work: 3124999062500
Thread 6 work: 3124999062500
Thread 7 work: 3124999062500
Thread 8 work: 3124999062500
Thread 9 work: 3124999062500
Thread 10 work: 3124999062500
Thread 11 work: 3124999062500
Thread 12 work: 3124999062500
Thread 13 work: 3124999062500
Thread 14 work: 3124999062500
Thread 15 work: 3124999062501 (has 1 more unit of work)

Running Prime with N = 10000
Thread 0 work: 3124062
Thread 1 work: 3124062
Thread 2 work: 3124062
Thread 3 work: 3124062
Thread 4 work: 3124062
Thread 5 work: 3124062
Thread 6 work: 3124062
Thread 7 work: 3124062
Thread 8 work: 3124062
Thread 9 work: 3124062
Thread 10 work: 3124062
Thread 11 work: 3124062
Thread 12 work: 3124062
Thread 13 work: 3124062
Thread 14 work: 3124062
Thread 15 work: 3124071 (has 9 more units of work)

## N = 10k
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    0.001663 seconds
  Median:     0.001666 seconds
  Min:        0.001644 seconds
  Max:        0.001689 seconds
  Speedup:    6.74x

## N = 100k
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    0.126258 seconds
  Median:     0.126225 seconds
  Min:        0.124048 seconds
  Max:        0.127613 seconds
  Speedup:    6.84x

## n = 1M
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    10.464106 seconds
  Median:     10.464418 seconds
  Min:        10.461605 seconds
  Max:        10.466167 seconds
  Speedup:    6.87x

## n = 10M
Benchmark: Prime static load balancing
  Iterations: 1
  Average:    887.121701 seconds
  Median:     887.121701 seconds
  Min:        887.121701 seconds
  Max:        887.121701 seconds
  Speedup:    6.99x

# Dynamic load balancing

## n = 10k
Benchmark: Prime dynamic load balancing
  Iterations: 5
  Average:    0.001668 seconds
  Median:     0.001665 seconds
  Min:        0.001646 seconds
  Max:        0.001685 seconds
  Speedup:    6.59x

## n = 100k
Benchmark: Prime dynamic load balancing
  Iterations: 5
  Average:    0.119459 seconds
  Median:     0.119317 seconds
  Min:        0.119256 seconds
  Max:        0.120095 seconds
  Speedup:    7.25x

## n = 1M
Benchmark: Prime dynamic load balancing
  Iterations: 5
  Average:    10.027898 seconds
  Median:     10.025827 seconds
  Min:        10.025424 seconds
  Max:        10.033011 seconds
  Speedup:    7.18x

## n = 10M
Benchmark: Prime dynamic load balancing
  Iterations: 1
  Average:    854.934491 seconds
  Median:     854.934491 seconds
  Min:        854.934491 seconds
  Max:        854.934491 seconds
  Speedup:    7.25x

# Overall benchmarking
## Redone: using an atomic to return the number of primes
The above benchmarks were made across different days on different sunlab machines.  This below is the one true benchmark. Each has 1 warmup run and takes the average over 5 runs of each technique

# TBB parallel_for with atomic

## Running Prime with N = 10000
Benchmark: Prime tbb parallel_for
  Iterations: 5
  Average:    0.001567 seconds
  Median:     0.001563 seconds
  Min:        0.001558 seconds
  Max:        0.001577 seconds
  Speedup:    7.28x

## Running Prime with N = 100000
Benchmark: Prime tbb parallel_for
  Iterations: 5
  Average:    0.121653 seconds
  Median:     0.121654 seconds
  Min:        0.121563 seconds
  Max:        0.121740 seconds
  Speedup:    7.33x

## Running Prime with N = 1000000 1M
Benchmark: Prime tbb parallel_for
  Iterations: 5
  Average:    10.381945 seconds
  Median:     10.429953 seconds
  Min:        10.137109 seconds
  Max:        10.550245 seconds
  Speedup:    7.09x

# TBB parallel_reduce (workload isn't big enough maybe)

## Running Prime with N = 10000
warm. Runs: . . . . .
Benchmark: Prime tbb parallel_reduce
  Iterations: 5
  Average:    0.001572 seconds
  Median:     0.001572 seconds
  Min:        0.001554 seconds
  Max:        0.001594 seconds

## Running Prime with N = 100000
warm. Runs: . . . . .
Benchmark: Prime tbb parallel_reduce
  Iterations: 5
  Average:    0.122097 seconds
  Median:     0.122222 seconds
  Min:        0.121587 seconds
  Max:        0.122322 seconds

## Running Prime with N = 1000000 1M
warm. Runs: . . . . .
Benchmark: Prime tbb parallel_reduce
  Iterations: 5
  Average:    10.678858 seconds
  Median:     10.678392 seconds
  Min:        10.443262 seconds
  Max:        10.902671 seconds
  Speedup:    N/A

# Final data

# Running Prime with N = 10000
warm. Runs: . . . . .
Benchmark: Prime serial
  Iterations: 5
  Average:    0.010950 seconds
  Median:     0.010940 seconds
  Min:        0.010939 seconds
  Max:        0.010990 seconds
  Speedup:    N/A
warm. Runs: . . . . .
Benchmark: Prime tbb parallel_for
  Iterations: 5
  Average:    0.001536 seconds
  Median:     0.001529 seconds
  Min:        0.001527 seconds
  Max:        0.001564 seconds
  Speedup:    7.13x
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    0.001732 seconds
  Median:     0.001747 seconds
  Min:        0.001696 seconds
  Max:        0.001764 seconds
  Speedup:    6.32x
warm. Runs: . . . . .
Benchmark: Prime dynamic load balancing
  Iterations: 5
  Average:    0.001794 seconds
  Median:     0.001795 seconds
  Min:        0.001777 seconds
  Max:        0.001822 seconds
  Speedup:    6.10x

# Running Prime with N = 100000
warm. Runs: . . . . .
Benchmark: Prime serial
  Iterations: 5
  Average:    0.872956 seconds
  Median:     0.872764 seconds
  Min:        0.868742 seconds
  Max:        0.876112 seconds
  Speedup:    N/A
warm. Runs: . . . . .
Benchmark: Prime tbb parallel_for
  Iterations: 5
  Average:    0.120868 seconds
  Median:     0.121313 seconds
  Min:        0.119192 seconds
  Max:        0.121902 seconds
  Speedup:    7.22x
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    0.129746 seconds
  Median:     0.130002 seconds
  Min:        0.128424 seconds
  Max:        0.130290 seconds
  Speedup:    6.73x
warm. Runs: . . . . .
Benchmark: Prime dynamic load balancing
  Iterations: 5
  Average:    0.122719 seconds
  Median:     0.122716 seconds
  Min:        0.122668 seconds
  Max:        0.122792 seconds
  Speedup:    7.11x

# Running Prime with N = 1000000
warm. Runs: . . . . .
Benchmark: Prime serial
  Iterations: 5
  Average:    73.035794 seconds
  Median:     72.778033 seconds
  Min:        72.739846 seconds
  Max:        74.110970 seconds
  Speedup:    N/A
warm. Runs: . . . . .
Benchmark: Prime tbb parallel_for
  Iterations: 5
  Average:    10.232289 seconds
  Median:     10.249783 seconds
  Min:        10.024361 seconds
  Max:        10.423788 seconds
  Speedup:    7.14x
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    11.222076 seconds
  Median:     10.913604 seconds
  Min:        10.884940 seconds
  Max:        12.458051 seconds
  Speedup:    6.51x
warm. Runs: . . . . .
Benchmark: Prime dynamic load balancing
  Iterations: 5
  Average:    10.588427 seconds
  Median:     10.576974 seconds
  Min:        10.538583 seconds
  Max:        10.679560 seconds
  Speedup:    6.90x

# Running Prime with N = 10000000
## 1 iteration each at 10M, skipping serial
warm. Runs: .
Benchmark: Prime tbb parallel_for
  Iterations: 1
  Average:    921.592442 seconds
  Median:     921.592442 seconds
  Min:        921.592442 seconds
  Max:        921.592442 seconds
  Speedup:    N/A
warm. Runs: .
Benchmark: Prime static load balancing
  Iterations: 1
  Average:    951.615472 seconds
  Median:     951.615472 seconds
  Min:        951.615472 seconds
  Max:        951.615472 seconds
  Speedup:    N/A
warm. Runs: .
Benchmark: Prime dynamic load balancing
  Iterations: 1
  Average:    916.992667 seconds
  Median:     916.992667 seconds
  Min:        916.992667 seconds
  Max:        916.992667 seconds
  Speedup:    N/A

# 2 threads
## Running Prime with N = 1M
Benchmark: Prime tbb parallel_for
  Iterations: 5
  Average:    37.721575 seconds
  Median:     37.716716 seconds
  Min:        37.707830 seconds
  Max:        37.736384 seconds
  Speedup:    1.94x
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    38.639176 seconds
  Median:     38.620420 seconds
  Min:        38.596086 seconds
  Max:        38.686946 seconds
  Speedup:    1.89x
warm. Runs: . . . . .
Benchmark: Prime dynamic load balancing
  Iterations: 5
  Average:    37.701527 seconds
  Median:     37.699771 seconds
  Min:        37.675492 seconds
  Max:        37.725730 seconds
  Speedup:    1.94x

# 4 threads
oops! I forgot to copy

# 8 threads
## Running Prime with N = 1000000
Benchmark: Prime tbb parallel_for
  Iterations: 5
  Average:    10.027879 seconds
  Median:     10.026987 seconds
  Min:        10.026792 seconds
  Max:        10.030128 seconds
  Speedup:    7.24x
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    10.823162 seconds
  Median:     10.822359 seconds
  Min:        10.819125 seconds
  Max:        10.829527 seconds
  Speedup:    6.71x
warm. Runs: . . . . .
Benchmark: Prime dynamic load balancing
  Iterations: 5
  Average:    10.036309 seconds
  Median:     10.036884 seconds
  Min:        10.033922 seconds
  Max:        10.038522 seconds
  Speedup:    7.24x

# 16 threads
## Running Prime with N = 1000000
Benchmark: Prime tbb parallel_for
  Iterations: 5
  Average:    10.177711 seconds
  Median:     10.205054 seconds
  Min:        10.026847 seconds
  Max:        10.307886 seconds
  Speedup:    7.15x
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    10.803231 seconds
  Median:     10.796278 seconds
  Min:        10.774765 seconds
  Max:        10.828341 seconds
  Speedup:    6.73x
warm. Runs: . . . . .
Benchmark: Prime dynamic load balancing
  Iterations: 5
  Average:    10.434496 seconds
  Median:     10.435140 seconds
  Min:        10.423451 seconds
  Max:        10.441174 seconds
  Speedup:    6.97x

# Naive static load balancing:
Running Prime with N = 10000
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    0.001650 seconds
  Median:     0.001644 seconds
  Min:        0.001639 seconds
  Max:        0.001667 seconds
  Speedup:    N/A
warm. Runs: . . . . .
Benchmark: Prime static naive load balancing
  Iterations: 5
  Average:    0.002369 seconds
  Median:     0.002339 seconds
  Min:        0.002144 seconds
  Max:        0.002613 seconds
  Speedup:    N/A

Running Prime with N = 100000
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    0.127605 seconds
  Median:     0.127590 seconds
  Min:        0.127479 seconds
  Max:        0.127808 seconds
  Speedup:    N/A
warm. Runs: . . . . .
Benchmark: Prime static naive load balancing
  Iterations: 5
  Average:    0.160153 seconds
  Median:     0.159616 seconds
  Min:        0.155232 seconds
  Max:        0.166001 seconds
  Speedup:    N/A

Running Prime with N = 1000000
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    10.462100 seconds
  Median:     10.465487 seconds
  Min:        10.437093 seconds
  Max:        10.471637 seconds
  Speedup:    N/A
warm. Runs: . . . . .
Benchmark: Prime static naive load balancing
  Iterations: 5
  Average:    12.468341 seconds
  Median:     12.551449 seconds
  Min:        11.885362 seconds
  Max:        12.825147 seconds
  Speedup:    N/A

Running Prime with N = 10000000
warm. Runs: . . . . .
Benchmark: Prime static load balancing
  Iterations: 5
  Average:    892.554834 seconds
  Median:     892.189584 seconds
  Min:        891.267016 seconds
  Max:        894.187489 seconds
  Speedup:    N/A
warm. Runs: . . . . .
Benchmark: Prime static naive load balancing
  Iterations: 5
  Average:    1076.562059 seconds
  Median:     1084.698854 seconds
  Min:        995.972647 seconds
  Max:        1153.556215 seconds
  Speedup:    N/A

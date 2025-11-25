# Approach
I found a suitably large dataset on household electricity usage.  Tuning the number of iterations, clusters, and number of data points allows me to greatly increase the sequential runtime.  This gives a good starting point for parallelization.

Before starting to parallelize, I profiled the dataset and by timing the execution of all the logical parts of the algorithm.  This will give me a good idea for which functions take the longest to run.  An effective parallelization strategy is to attack the largest functions first.

## profile 1
Profiling kmeans runtime
K:10 total_points:10000 total_values:13 max_iterations:10
iteration 1
Total time: 103369 micros

   select clusters time:    23 micros

   nearest cluster time:    53511 micros
       remove point time:         15680 micros
       find nearest center time:  234micros
       set cluster time:          6micros
       add point time:            1844micros

   recalculate center time: 49820 micros

## k = 100
Profiling kmeans runtime
K:100 total_points:10000 total_values:13 max_iterations:10
iteration 1
Total time: 135737 micros

   select clusters time:    122 micros

   nearest cluster time:    103179 micros
       remove point time:         270 micros
       find nearest center time:  12004micros
       set cluster time:          9micros
       add point time:            1492micros

   recalculate center time: 32420 micros

## 100k points, k = 100
K:100 total_points:100000 total_values:13 max_iterations:10
iterations: . . . . . . . . . Break in iteration 10


Total time: 1527319 ms

   select clusters time:    0.176 ms

   nearest cluster time:    1171.21 ms
       remove point time:         7.455 ms
       find nearest center time:  638.399 ms
       set cluster time:          0.025 ms
       add point time:            14.784 ms

   recalculate center time: 355.75 ms

## max 100 iterations
K:100 total_points:100000 total_values:13 max_iterations:100
iterations: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . Break in iteration 41


Total time: 5763891 ms

   select clusters time:    0.126 ms

   nearest cluster time:    4324.95 ms
       remove point time:         25.839 ms
       find nearest center time:  1758.19 ms
       set cluster time:          0.02 ms
       add point time:            13.741 ms

   recalculate center time: 1438.12 ms

## 500k total points
### we see a much quicker execution.  These 500k points fit the initial clusters better than the 100k points.  Iterations are a strong overhead
K:100 total_points:500000 total_values:13 max_iterations:100
iterations: . Break in iteration 2


Total time: 1428558 ms

   select clusters time:    0.142 ms

   nearest cluster time:    1090.63 ms
       remove point time:         0 ms
       find nearest center time:  129.067 ms
       set cluster time:          0.057 ms
       add point time:            51.071 ms

   recalculate center time: 337.733 ms

### Seems like runtime can be cut very short if initial clusters are randomly great
### More clusters means less iterations
### Running with k = 2 and 500k total points takes forever.  >15 mins

## K:10 total_points:100000 total_values:13 max_iterations:100
iterations: . Break in iteration 2


Total time: 167792 ms

   select clusters time:    0.023 ms

   nearest cluster time:    88.01 ms
       remove point time:         0.001 ms
       find nearest center time:  1.521 ms
       set cluster time:          0.028 ms
       add point time:            13.903 ms

   recalculate center time: 79.693 ms

## K:2 total_points:100000 total_values:13 max_iterations:100
iterations: . . Break in iteration 3


Total time: 51972070 ms

   select clusters time:    0.008 ms

   nearest cluster time:    51866.1 ms
       remove point time:         51657.2 ms
       find nearest center time:  0.513 ms
       set cluster time:          0.071 ms
       add point time:            34.365 ms

   recalculate center time: 105.901 ms

## K:5 total_points:100000 total_values:13 max_iterations:100
iterations: . . Break in iteration 3


Total time: 50300390 ms

   select clusters time:    0.013 ms

   nearest cluster time:    50186.3 ms
       remove point time:         49972.4 ms
       find nearest center time:  0.629 ms
       set cluster time:          0.072 ms
       add point time:            32.167 ms

   recalculate center time: 114 ms

## Less values
## K:5 total_points:100000 total_values:2 max_iterations:100
iterations: . . . . . . . . . . . . . . . Break in iteration 16


Total time: 111900 ms

   select clusters time:    0.013 ms

   nearest cluster time:    111763 ms
       remove point time:         111123 ms
       find nearest center time:  5.215 ms
       set cluster time:          1.29 ms
       add point time:            64.339 ms

   recalculate center time: 136.041 ms


# total_points:100000 total_values:13 K:1000 max_iterations:10
## Trying to optimize remove by allowing it to take a set to be removed.  Didn't really work
warm. Runs: iterations: . . . . . . . . . Break in iteration 10


Total time: 10251.7 ms

   nearest cluster time:    9898.36 ms

   recalculate center time: 351.975 ms
.
Benchmark: Kmeans serial
  Iterations: 1
  Average:    10.256331 seconds
  Median:     10.256331 seconds
  Min:        10.256331 seconds
  Max:        10.256331 seconds
  Speedup:    N/A
warm. Runs: iterations: . . . . . . . . . Break in iteration 10


Total time: 3378.718000 ms

   nearest cluster time:    2987.623000 ms

   recalculate center time: 384.596000 ms
.
Benchmark: Kmeans parallel
  Iterations: 1
  Average:    3.382959 seconds
  Median:     3.382959 seconds
  Min:        3.382959 seconds
  Max:        3.382959 seconds
  Speedup:    3.03x

# total_points:100000 total_values:13 K:1000 max_iterations:10
## approach of reconstructing the vector in parallel and skipping points that were already in it
warm. Runs: iterations: . . . . . . . . . Break in iteration 10


Total time: 9219.83 ms

   nearest cluster time:    8891.14 ms

   recalculate center time: 327.288 ms
.
Benchmark: Kmeans serial
  Iterations: 1
  Average:    9.225183 seconds
  Median:     9.225183 seconds
  Min:        9.225183 seconds
  Max:        9.225183 seconds
  Speedup:    N/A
warm. Runs: iterations: . . . . . . . . . Break in iteration 10


Total time: 4050.785000 ms

   nearest cluster time:    3638.579000 ms

   recalculate center time: 404.675000 ms
.
Benchmark: Kmeans parallel
  Iterations: 1
  Average:    4.058580 seconds
  Median:     4.058580 seconds
  Min:        4.058580 seconds
  Max:        4.058580 seconds
  Speedup:    2.27x

# Use points set instead of vector
## total_points:10000 total_values:13 K:2 max_iterations:10 max_TBB_threads:16
warm. Runs: iterations: . . Break in iteration 3


Total time: 13.901 ms

   nearest cluster time:    9.49 ms

   recalculate center time: 4.398 ms
.
Benchmark: Kmeans serial
  Iterations: 1
  Average:    0.014055 seconds
  Median:     0.014055 seconds
  Min:        0.014055 seconds
  Max:        0.014055 seconds
  Speedup:    N/A
warm. Runs: iterations: . . Break in iteration 3


Total time: 13.077000 ms

   nearest cluster time:    3.101000 ms

   recalculate center time: 9.756000 ms
.
Benchmark: Kmeans parallel
  Iterations: 1
  Average:    0.013199 seconds
  Median:     0.013199 seconds
  Min:        0.013199 seconds
  Max:        0.013199 seconds
  Speedup:    1.06x

(same as above but with 5 iterations)
total_points:10000 total_values:13 K:2 max_iterations:10 max_TBB_threads:16
warm. Runs: . . . . .
Benchmark: Kmeans serial
  Iterations: 5
  Average:    0.019129 seconds
  Median:     0.013143 seconds
  Min:        0.000491 seconds
  Max:        0.041361 seconds
  Speedup:    N/A
warm. Runs: . . . . .
Benchmark: Kmeans parallel
  Iterations: 5
  Average:    0.014229 seconds
  Median:     0.021305 seconds
  Min:        0.000244 seconds
  Max:        0.023784 seconds
  Speedup:    1.34x

# total_points:100000 total_values:13 K:10 max_iterations:10 max_TBB_threads:16
warm. Runs: iterations: . Break in iteration 2


Total time: 49.276 ms

   nearest cluster time:    19.086 ms

   recalculate center time: 30.164 ms
.
Benchmark: Kmeans serial
  Iterations: 1
  Average:    0.050117 seconds
  Median:     0.050117 seconds
  Min:        0.050117 seconds
  Max:        0.050117 seconds
  Speedup:    N/A
warm. Runs: iterations: . Break in iteration 2


Total time: 118.766000 ms

   nearest cluster time:    10.483000 ms

   adjust clusters time:    10.045000 ms

   recalculate center time: 96.875000 ms
.
Benchmark: Kmeans parallel
  Iterations: 1
  Average:    0.120742 seconds
  Median:     0.120742 seconds
  Min:        0.120742 seconds
  Max:        0.120742 seconds
  Speedup:    0.42x


## My datasets were too big to fit on sunlab.  I couldn't open them to save.  Max was 250,000 lines

# I moved to google sheets for the rest of my data for quicker graphing
 


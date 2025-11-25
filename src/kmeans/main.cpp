#include "benchmark.hpp"
#include "constants.hpp"
#include "kmeans/parallel.hpp"
#include "kmeans/serial.hpp"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

using namespace std;

int main() {

    tbb::global_control gc(tbb::global_control::max_allowed_parallelism,
                           NUM_THREADS);

    cout << "RAND_SEED = " << RAND_SEED << endl;

    cout << "TOTAL_POINTS:" << TOTAL_POINTS << " TOTAL_VALUES:" << TOTAL_VALUES
         << " CLUSTERS:" << CLUSTERS << " MAX_ITERATIONS:" << MAX_ITERATIONS
         << " max_TBB_threads:" << NUM_THREADS << endl;

    vector<Point> points;
    string point_name;

    for (int i = 0; i < TOTAL_POINTS; i++) {
        vector<double> values;

        for (int j = 0; j < TOTAL_VALUES; j++) {
            double value;
            cin >> value;
            values.push_back(value);
        }

        if (HAS_NAME) {
            cin >> point_name;
            Point p(i, values, point_name);
            points.push_back(p);
        } else {
            Point p(i, values);
            points.push_back(p);
        }
    }

    srand(RAND_SEED);
    Benchmark serial("Kmeans serial");
    serial.run([&]() {
        // Make a fresh copy of points so it isn't modified between runs
        vector<Point> points_copy = points;
        serial::KMeans kmeans(CLUSTERS, TOTAL_POINTS, TOTAL_VALUES,
                              MAX_ITERATIONS);
        kmeans.run(points_copy);
    });
    serial.print_results();

    // have to re-seed the randomness
    srand(RAND_SEED);
    Benchmark parallel("Kmeans parallel");
    parallel.run([&]() {
        // Make a fresh copy of points so it isn't modified between runs
        vector<Point> points_copy = points;
        parallel::KMeans kmeans;
        kmeans.run(points_copy);
    });
    parallel.print_comparison(serial);

    return 0;
}

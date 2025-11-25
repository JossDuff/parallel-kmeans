
#include "constants.hpp"
#include "kmeans/parallel.hpp"
#include "kmeans/serial.hpp"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

using namespace std;

int main() {

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

    vector<Point> serial_points = points;
    vector<Point> parallel_points = points;

    // serial
    srand(RAND_SEED);
    serial::KMeans serial_kmeans(CLUSTERS, TOTAL_POINTS, TOTAL_VALUES,
                                 MAX_ITERATIONS);
    serial_kmeans.run(serial_points);

    // parallel
    srand(RAND_SEED);
    parallel::KMeans parallel_kmeans;
    parallel_kmeans.run(parallel_points);

    bool results_match = true;
    int differences = 0;

    // Clusters can have the same collection of points but different IDs across
    // runs This maps from serial cluster id to parallel cluster id
    unordered_map<int, int> logical_clusters;

    // Program is correct if the clusterings are the same.  Cluster IDs may be
    // different, but the actual collections must be the same
    for (size_t i = 0; i < serial_points.size(); i++) {
        int serial_cluster = serial_points[i].getCluster();
        int parallel_cluster = parallel_points[i].getCluster();

        // if it already exists in the map, it should match the parallel cluster
        if (logical_clusters.find(serial_cluster) != logical_clusters.end()) {
            if (logical_clusters.at(serial_cluster) != parallel_cluster) {
                // this point's serial cluster doesn't match the parallel
                // cluster that it should have been associated with if the
                // clustering was the same

                results_match = false;
                differences++;
            }
        } else {
            // it doesn't exist in the map, so we can insert it
            logical_clusters.insert(
                make_pair(serial_cluster, parallel_cluster));
        }

        // cout << "Point " << i
        //      << ": serial cluster=" << serial_points[i].getCluster()
        //      << ", parallel cluster=" << parallel_points[i].getCluster()
        //      << endl;
    }

    cout << "\nResults verification: "
         << (results_match ? "PASSED :)" : "FAILED :(") << endl;

    if (!results_match) {
        cout << "Total differences: " << differences << "/"
             << serial_points.size() << " points" << endl;
    }
    return 0;
}

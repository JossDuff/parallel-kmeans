#include "constants.hpp"
#include "point.hpp"
#include "profiler.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <emmintrin.h> // SSE2
#include <iostream>
#include <math.h>
#include <oneapi/tbb/concurrent_vector.h>
#include <stdlib.h>
#include <tbb/tbb.h>
#include <time.h>
#include <unordered_set>
#include <vector>

using namespace std;

namespace parallel {

class Cluster {
  private:
    int id_cluster;
    vector<double> central_values;
    vector<Point> points;

  public:
    Cluster(int id_cluster, Point point) {
        this->id_cluster = id_cluster;

        for (int i = 0; i < TOTAL_VALUES; i++)
            central_values.push_back(point.getValue(i));

        points.push_back(point);
    }

    void addPoint(Point point) { points.push_back(point); }

    void removePoints(unordered_set<int> id_points) {
        tbb::concurrent_vector<Point> new_points;
        tbb::parallel_for(tbb::blocked_range<size_t>(0, points.size()),
                          [&](const tbb::blocked_range<size_t> &r) {
                              for (size_t i = r.begin(); i != r.end(); ++i) {
                                  if (id_points.count(points[i].getID()) == 0) {
                                      new_points.push_back(points[i]);
                                  }
                              }
                          });

        points = vector<Point>(new_points.begin(), new_points.end());
    }

    double getCentralValue(int index) { return central_values[index]; }

    const double *centralValuesPointer() { return central_values.data(); }

    void setCentralValue(int index, double value) {
        central_values[index] = value;
    }

    Point getPoint(int index) { return points[index]; }

    int getTotalPoints() { return points.size(); }

    int getID() { return id_cluster; }
};

class KMeans {
  private:
    vector<Cluster> clusters;
    // timings
    long calculate_cluster_time;
    long id_nearest_center_time;
    long adjust_cluster_time;
    long recalculate_center_time;

    int getIDNearestCenter(Point point) {
        int id_cluster_center = 0;
        double min_dist_sq = std::numeric_limits<double>::max();

        // CACHE point values once
        double point_cache[TOTAL_VALUES];
        for (int i = 0; i < TOTAL_VALUES; i++) {
            point_cache[i] = point.getValue(i);
        }

        // Use cached values for all clusters
        for (int cluster_id = 0; cluster_id < CLUSTERS; cluster_id++) {
            __m128d sum_vec = _mm_setzero_pd();

            int j = 0;
            for (; j + 1 < TOTAL_VALUES; j += 2) {
                // Load from cache array instead of calling getValue()
                __m128d p_vals = _mm_loadu_pd(&point_cache[j]);

                __m128d c_vals =
                    _mm_set_pd(clusters[cluster_id].getCentralValue(j + 1),
                               clusters[cluster_id].getCentralValue(j));

                __m128d diff = _mm_sub_pd(c_vals, p_vals);
                __m128d sq = _mm_mul_pd(diff, diff);
                sum_vec = _mm_add_pd(sum_vec, sq);
            }

            double temp[2];
            _mm_store_pd(temp, sum_vec);
            double dist_sq = temp[0] + temp[1];

            if (j < TOTAL_VALUES) {
                double diff =
                    clusters[cluster_id].getCentralValue(j) - point_cache[j];
                dist_sq += diff * diff;
            }

            if (cluster_id == 0 || dist_sq < min_dist_sq) {
                min_dist_sq = dist_sq;
                id_cluster_center = cluster_id;
            }
        }

        return id_cluster_center;
    }

  public:
    void run(vector<Point> &points) {
        auto begin = start();

        if (CLUSTERS > TOTAL_POINTS)
            return;

        vector<int> prohibited_indexes;

        // choose K distinct values for the centers of the clusters
        for (int i = 0; i < CLUSTERS; i++) {
            while (true) {
                int index_point = rand() % TOTAL_POINTS;

                if (find(prohibited_indexes.begin(), prohibited_indexes.end(),
                         index_point) == prohibited_indexes.end()) {
                    prohibited_indexes.push_back(index_point);
                    points[index_point].setCluster(i);
                    Cluster cluster(i, points[index_point]);
                    clusters.push_back(cluster);
                    break;
                }
            }
        }

        int iter = 1;

        if (PRINT_PROFILE)
            cout << "iterations: " << flush;
        while (true) {
            bool done = true;

            // vector of points to remove from clusters
            // fist vector is cluster index, second vector is the point_id to
            // remove from that cluster
            vector<tbb::concurrent_vector<int>> remove_vec(CLUSTERS);

            // vector of id_cluster, index_point to be set
            vector<tbb::concurrent_vector<Point>> add_point_to_cluster(
                CLUSTERS);

            // affinity_partitioner is good for cache locality
            static tbb::affinity_partitioner ap;
            auto start_calculate_cluster = start();
            auto start_id_nearest_center = start();
            tbb::parallel_for(
                tbb::blocked_range<int>(0, TOTAL_POINTS),
                [&](const tbb::blocked_range<int> &r) {
                    for (int i = r.begin(); i != r.end(); ++i) {
                        int id_old_cluster = points[i].getCluster();
                        int id_nearest_center = getIDNearestCenter(points[i]);

                        if (id_old_cluster != id_nearest_center) {
                            if (id_old_cluster != -1) {
                                remove_vec[id_old_cluster].push_back(
                                    points[i].getID());
                            }

                            // can make this operation in parallel because only
                            // this thread is accessing points[i]
                            points[i].setCluster(id_nearest_center);

                            add_point_to_cluster[id_nearest_center].push_back(
                                points[i]);

                            done = false;
                        }
                    }
                },
                ap);
            id_nearest_center_time += end(start_id_nearest_center);

            auto start_adjust_cluster = start();
            tbb::parallel_for(
                tbb::blocked_range<int>(0, CLUSTERS),
                [&](const tbb::blocked_range<int> &r) {
                    for (int k = r.begin(); k != r.end(); ++k) {
                        // turn the list of points to remove into a set
                        unordered_set<int> remove_set(remove_vec[k].begin(),
                                                      remove_vec[k].end());
                        // Remove points from this cluster
                        clusters[k].removePoints(remove_set);

                        // Add points to this cluster
                        // This doesn't need to be batched because it just adds
                        // to the end of the vector
                        for (Point point : add_point_to_cluster[k]) {
                            clusters[k].addPoint(point);
                        }
                    }
                });
            adjust_cluster_time += end(start_adjust_cluster);
            calculate_cluster_time += end(start_calculate_cluster);

            auto start_recalculate_center = start();

            // recalculating the center of each cluster
            tbb::parallel_for(
                tbb::blocked_range<int>(0, CLUSTERS),
                [&](const tbb::blocked_range<int> &r) {
                    for (int i = r.begin(); i != r.end(); i++) {
                        for (int j = 0; j < TOTAL_VALUES; j++) {
                            int total_points_cluster =
                                clusters[i].getTotalPoints();
                            double sum = 0.0;

                            if (total_points_cluster > 0) {
                                for (int p = 0; p < total_points_cluster; p++)
                                    sum += clusters[i].getPoint(p).getValue(j);
                                clusters[i].setCentralValue(
                                    j, sum / total_points_cluster);
                            }
                        }
                    }
                });
            recalculate_center_time += end(start_recalculate_center);

            if (done == true || iter >= MAX_ITERATIONS) {
                if (PRINT_PROFILE)
                    cout << "Break in iteration " << iter << "\n\n";
                break;
            }

            if (PRINT_PROFILE)
                cout << ". " << flush;
            iter++;
        }
        auto total_duration = end(begin);

        if (PRINT_PROFILE) {
            std::cout << "\nTotal time: " << total_duration / 1000.0 << " ms"
                      << std::endl;
            cout << "\n   calculate cluster time:    "
                 << calculate_cluster_time / 1000.0 << " ms" << endl;
            cout << "      calculate closest center:   "
                 << id_nearest_center_time / 1000.0 << " ms" << endl;
            cout << "      adjust cluster time:        "
                 << adjust_cluster_time / 1000.0 << " ms" << endl;
            cout << "\n   recalculate center time: "
                 << recalculate_center_time / 1000.0 << " ms" << endl;
        }
    }
};
} // namespace parallel

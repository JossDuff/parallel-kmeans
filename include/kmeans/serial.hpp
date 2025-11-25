
// Implementation of the KMeans Algorithm
// reference: https://github.com/marcoscastro/kmeans

#include "constants.hpp"
#include "point.hpp"
#include "profiler.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace std;

namespace serial {

class Cluster {
  private:
    int id_cluster;
    vector<double> central_values;
    vector<Point> points;

  public:
    Cluster(int id_cluster, Point point) {
        this->id_cluster = id_cluster;

        int total_values = point.getTotalValues();

        for (int i = 0; i < total_values; i++)
            central_values.push_back(point.getValue(i));

        points.push_back(point);
    }

    void addPoint(Point point) { points.push_back(point); }

    bool removePoint(int id_point) {
        int total_points = points.size();

        for (int i = 0; i < total_points; i++) {
            if (points[i].getID() == id_point) {
                points.erase(points.begin() + i);
                return true;
            }
        }
        return false;
    }

    double getCentralValue(int index) { return central_values[index]; }

    void setCentralValue(int index, double value) {
        central_values[index] = value;
    }

    Point getPoint(int index) { return points[index]; }

    int getTotalPoints() { return points.size(); }

    int getID() { return id_cluster; }
};

class KMeans {
  private:
    int K; // number of clusters
    int total_values, total_points, max_iterations;
    vector<Cluster> clusters;
    // timings
    long calculate_clusters;
    long id_nearest_center_time;
    long adjust_cluster_time;
    long recalculate_center_time;

    // return ID of nearest center (uses euclidean distance)
    int getIDNearestCenter(Point point) {
        double sum = 0.0, min_dist;
        int id_cluster_center = 0;

        for (int i = 0; i < total_values; i++) {
            sum += pow(clusters[0].getCentralValue(i) - point.getValue(i), 2.0);
        }

        min_dist = sqrt(sum);

        for (int i = 1; i < K; i++) {
            double dist;
            sum = 0.0;

            for (int j = 0; j < total_values; j++) {
                sum += pow(clusters[i].getCentralValue(j) - point.getValue(j),
                           2.0);
            }

            dist = sqrt(sum);

            if (dist < min_dist) {
                min_dist = dist;
                id_cluster_center = i;
            }
        }

        return id_cluster_center;
    }

  public:
    KMeans(int K, int total_points, int total_values, int max_iterations) {
        this->K = K;
        this->total_points = total_points;
        this->total_values = total_values;
        this->max_iterations = max_iterations;
        // timings
        calculate_clusters = 0;
        id_nearest_center_time = 0;
        adjust_cluster_time = 0;
        recalculate_center_time = 0;
    }

    void run(vector<Point> &points) {
        auto begin = start();

        if (K > total_points)
            return;

        vector<int> prohibited_indexes;

        // choose K distinct values for the centers of the clusters
        for (int i = 0; i < K; i++) {
            while (true) {
                int index_point = rand() % total_points;

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

            auto start_calculate_cluster = start();
            // associates each point to the nearest center
            for (int i = 0; i < total_points; i++) {
                int id_old_cluster = points[i].getCluster();

                auto start_id_nearest_center = start();
                int id_nearest_center = getIDNearestCenter(points[i]);
                id_nearest_center_time += end(start_id_nearest_center);

                auto start_adjust_cluster = start();
                if (id_old_cluster != id_nearest_center) {
                    if (id_old_cluster != -1)
                        clusters[id_old_cluster].removePoint(points[i].getID());

                    points[i].setCluster(id_nearest_center);
                    clusters[id_nearest_center].addPoint(points[i]);
                    done = false;
                }
                adjust_cluster_time += end(start_adjust_cluster);
            }
            calculate_clusters += end(start_calculate_cluster);

            auto start_recalculate_center = start();
            // recalculating the center of each cluster
            for (int i = 0; i < K; i++) {
                for (int j = 0; j < total_values; j++) {
                    int total_points_cluster = clusters[i].getTotalPoints();
                    double sum = 0.0;

                    if (total_points_cluster > 0) {
                        for (int p = 0; p < total_points_cluster; p++)
                            sum += clusters[i].getPoint(p).getValue(j);
                        clusters[i].setCentralValue(j,
                                                    sum / total_points_cluster);
                    }
                }
            }
            recalculate_center_time += end(start_recalculate_center);

            if (done == true || iter >= max_iterations) {
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
                 << calculate_clusters / 1000.0 << " ms" << endl;
            cout << "      calculate closest center:   "
                 << id_nearest_center_time / 1000.0 << " ms" << endl;
            cout << "      adjust cluster time:        "
                 << adjust_cluster_time / 1000.0 << " ms" << endl;
            cout << "\n   recalculate center time: "
                 << recalculate_center_time / 1000.0 << " ms" << endl;
        }
    }
};
} // namespace serial

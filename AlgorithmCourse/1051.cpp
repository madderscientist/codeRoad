// 不是我做的...

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

using namespace std;

// Constants and Global Variables
const double INF = 1e18;
const double EPS = 1e-9;

struct Sensor {
    double L, R, t;
};

struct Segment {
    double end_pos;
    double velocity;
};

double D;
int n;
vector<Sensor> sensors;
vector<Segment> solution_segments;
double V_opt;

// Calculate Optimal Velocity by solving 0.74v^3 + 0.136v^2 - 1160 = 0
void calculate_v_opt() {
    double low = 0.1, high = 100.0;
    for (int i = 0; i < 100; ++i) {
        double mid = low + (high - low) / 2.0;
        double val = 0.74 * pow(mid, 3) + 0.136 * pow(mid, 2) - 1160.0;
        if (val < 0) low = mid;
        else high = mid;
    }
    V_opt = high;
}

// Prefix sum for time to make range sum O(1)
vector<double> time_prefix;

// Recursive Solver
// range_idx_start, range_idx_end: range of sensors to consider (indices in 'sensors')
// pos_start, pos_end: spatial range available to process these sensors
void solve(int idx_start, int idx_end, double pos_start, double pos_end) {
    if (pos_start >= pos_end - EPS) return; // No distance to cover

    // Base Case: No sensors left in this region
    if (idx_start > idx_end) {
        solution_segments.push_back({pos_end, V_opt});
        return;
    }

    // Find the global bottleneck in this range
    // We want to find pair (u, v) such that the velocity constraint is minimal (tightest).
    // Velocity = Distance / Time. We want min(Velocity).
    
    double min_v = INF;
    int best_u = -1, best_v = -1;

    // Iterate through all sub-intervals of sensors [u, v]
    for (int u = idx_start; u <= idx_end; ++u) {
        for (int v = u; v <= idx_end; ++v) {
            // Calculate required time
            double total_time = time_prefix[v+1] - time_prefix[u];
            
            // Calculate available distance
            // The segment for sensors u...v physically spans from max(pos_start, L_u) to min(pos_end, R_v)
            double start_limit = max(pos_start, sensors[u].L);
            double end_limit = min(pos_end, sensors[v].R);
            
            double dist = end_limit - start_limit;
            
            double required_v;
            if (dist <= EPS) {
                // If distance is 0 or negative but time is required, velocity is 0 (impossible constraint logic, 
                // but effectively implies we need infinite time per meter, i.e., v -> 0).
                required_v = 0.0; 
            } else {
                required_v = dist / total_time;
            }

            if (required_v < min_v) {
                min_v = required_v;
                best_u = u;
                best_v = v;
            }
        }
    }

    // If the tightest constraint allows speed >= V_opt, we fly at V_opt
    if (min_v >= V_opt) {
        solution_segments.push_back({pos_end, V_opt});
        return;
    }

    // Otherwise, we must satisfy the bottleneck (u...v)
    // The bottleneck physically occupies [bottleneck_start, bottleneck_end]
    double bottleneck_start = max(pos_start, sensors[best_u].L);
    double bottleneck_end = min(pos_end, sensors[best_v].R);

    // 1. Solve Left Region (sensors before u, space before bottleneck)
    solve(idx_start, best_u - 1, pos_start, bottleneck_start);

    // 2. Add Bottleneck Segment
    solution_segments.push_back({bottleneck_end, min_v});

    // 3. Solve Right Region (sensors after v, space after bottleneck)
    solve(best_v + 1, idx_end, bottleneck_end, pos_end);
}

void run_test_case() {
    cin >> D >> n;
    sensors.resize(n);
    time_prefix.assign(n + 1, 0.0);
    
    for (int i = 0; i < n; ++i) {
        cin >> sensors[i].L >> sensors[i].R >> sensors[i].t;
        time_prefix[i+1] = time_prefix[i] + sensors[i].t;
    }

    solution_segments.clear();
    
    // Start recursive solver
    solve(0, n - 1, 0.0, D);

    // Merge adjacent segments with same velocity
    vector<Segment> merged;
    for (const auto& seg : solution_segments) {
        if (merged.empty()) {
            merged.push_back(seg);
        } else {
            if (abs(merged.back().velocity - seg.velocity) < 1e-6) {
                merged.back().end_pos = seg.end_pos;
            } else {
                merged.push_back(seg);
            }
        }
    }

    // Output
    cout << merged.size() << endl;
    
    // Print positions
    for (int i = 0; i < merged.size(); ++i) {
        cout << fixed << setprecision(6) << merged[i].end_pos << (i == merged.size() - 1 ? "" : " ");
    }
    cout << endl;

    // Print velocities
    for (int i = 0; i < merged.size(); ++i) {
        cout << fixed << setprecision(6) << merged[i].velocity << (i == merged.size() - 1 ? "" : " ");
    }
    cout << endl;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    calculate_v_opt();
    
    int t;
    if (cin >> t) {
        while (t--) {
            run_test_case();
        }
    }
    return 0;
}
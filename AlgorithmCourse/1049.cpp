#include <cstdio>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
const double opt_v = 13.98951925;

struct sensor {
    double l;
    double r;
    double t;
};

void solve() {
    int n;     // 传感器数量
    double D;  // 终点距离
    scanf("%d %lf", &n, &D);
    sensor sensors[n];
    vector<double> seq{0.0, D};  // 原子区间端点
    for (int i = 0; i < n; i++) {
        sensor& s = sensors[i];
        scanf("%lf %lf %lf", &(s.l), &(s.r), &(s.t));
        seq.push_back(s.l);
        seq.push_back(s.r);
    }
    // 区间去重
    sort(seq.begin(), seq.end());
    seq.erase(unique(seq.begin(), seq.end()), seq.end());
    int seqNum = seq.size() - 1;
    // 每个区间的速度
    double v[seqNum];
    for (int i = 0; i < seqNum; i++) v[i] = opt_v;

    for (int seqLen = 1; seqLen <= seqNum; seqLen++) {
        for (int i = 0; i <= seqNum - seqLen; i++) {
            double L = seq[i];
            double R = seq[i + seqLen];
            double required_t = 0;
            double LL = D;
            double RR = 0;
            for (sensor& s : sensors) {
                if (s.l - L >= -1e-7 && s.r - R <= 1e-7) {
                    required_t += s.t;
                    LL = min(LL, s.l);
                    RR = max(RR, s.r);
                }
            }
            if (required_t < 1e-7) continue;
            // 只更新刚好包围的
            if (LL != L || RR != R) continue;
            double dis = R - L;
            double v_max = dis / required_t;
            // 去掉中间更小速度区间所用时间和路程
            while (1) {
                double dis2 = dis;
                double t2 = required_t;
                for (int k = i; k < i + seqLen; k++) {
                    if (v[k] < v_max) {
                        double temp_dis = seq[k + 1] - seq[k];
                        t2 -= temp_dis / v[k];
                        dis2 -= temp_dis;
                    }
                }
                if (t2 < 1e-7) break;
                double v2 = dis2 / t2;
                if (v2 == v_max) break;
                v_max = v2;
            }
            for (int k = i; k < i + seqLen; k++) v[k] = min(v[k], v_max);
        }
    }

    // 输出
    double current_v = v[0];
    vector<int> at{0};
    vector<double> speed{current_v};
    for (int i = 1; i < seqNum; i++) {
        if (fabs(v[i] - current_v) < 1e-7) continue;
        at.push_back(i);
        speed.push_back(v[i]);
        current_v = v[i];
    }
    printf("%d\n", (int)speed.size());
    // 路程
    for (int i = 1; i < at.size(); i++) {
        printf("%.7f ", seq[at[i]]);
    } printf("%.7f\n", D);
    // 速度
    for (auto v : speed) {
        printf("%.7f ", v);
    } putchar('\n');
}

int main() {
    int m;
    scanf("%d", &m);
    while (m--) solve();
}
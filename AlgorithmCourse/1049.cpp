#include <bits/stdc++.h>
using namespace std;

int n;  // 传感器数量
int D;  // 终点距离
struct sensor {
    int l;
    int r;
    int t;
};

inline float PdV(float v) { // 除以了速度的 只要乘距离就是消耗的能量
    float v2 = v * v;
    return 0.07 * v2 + 0.0391 * v - 13.196 + 390.95 / v;
    // 求导：0.14v+0.0391-390.95/v^2
    // 二阶导：0.14 + 2 * 390.95 / v^3 非负，说明PdV是凸函数，存在一个最小值
    // 最小值当v=13.98951925时取，为28.996376637023385
    // 考虑到样例中速度大约为1，所以速度越快越好
    // 因此约束为在时间内采集完数据
}

void solve() {
    scanf("%d %d", n, D);
    sensor sensors[n];
    for (int i = 0; i < n; i++) {
        scanf("%d %d %d", sensors[i].l, sensors[i].r, sensors[i].t);
    }
}

int main() {
    int m;
    cin >> m;
    while(m--) solve();
}
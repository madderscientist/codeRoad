#include <bits/stdc++.h>
using namespace std;

int m;
int v_n;  // 顶点数
int e_n;  // 边数
int r_n;  // 总订单数
int x;  // 外卖小哥数

void solve() {
    scanf("%d %d %d %d", v_n, e_n, r_n, x);
    int e_n2 = e_n << 1;
    int v[v_n];
    int to[e_n2];
    int time[e_n];  // 取值记得左移
    int next[e_n2];
    memset(next, -1, e_n2 * sizeof(int));
    memset(v, -1, sizeof(int) * v_n);
    int tmp_from, tmp_to;
    for (int i = 0, id = 0; i < e_n; i++) {
        scanf("%d %d %d", tmp_from, tmp_to, time[i]);
        tmp_from--;
        tmp_to--;
        // 无向图要两条边
        next[id] = v[tmp_from];
        v[tmp_from] = id;
        to[id] = tmp_to;
        id += 1;
        next[id] = v[tmp_to];
        v[tmp_to] = id;
        to[id] = tmp_from;
        id += 1;
    }
    // 求两两最小时间
}

int main() {
    int m;
    cin >> m;
    while(m--) solve();
}
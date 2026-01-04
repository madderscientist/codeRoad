// #include <stdio.h>
// #include <queue>
// #include <vector>
// using namespace std;

// void solve() {
//     int n, m;   // 节点数 边数
//     scanf("%d%d", &n, &m);
//     int nodes[n];
//     int nextEdge[m];
//     int to[m];
//     int degree[n];  // 入度
//     for (int i = 0; i < n; i++) nodes[i] = -1;
//     for (int i = 0; i < n; i++) degree[i] = 0;
//     for (int i = 0, begin, end; i < m; i++) {
//         scanf("%d%d", &begin, &end);
//         begin--, end--;
//         to[i] = end;
//         nextEdge[i] = nodes[begin];
//         nodes[begin] = i;
//         degree[end]++;
//     }
//     // 拓扑排序
//     queue<int> zeroInDeg;
//     vector<int> topoOrder;
//     for (int i = 0; i < n; i++) {
//         if (degree[i] == 0) zeroInDeg.push(i);
//     }
//     while (!zeroInDeg.empty()) {
//         int u = zeroInDeg.front(); zeroInDeg.pop();
//         topoOrder.push_back(u);
//         for (int i = nodes[u]; i != -1; i = nextEdge[i]) {
//             int v = to[i];
//             if (--degree[v] == 0) zeroInDeg.push(v);
//         }
//     }
//     if (topoOrder.size() < n) {
//         printf("0\n");
//     } else {
//         for (int i = 0; i < n; i++) {
//             printf("%d%c", topoOrder[i] + 1, i == n - 1 ? '\n' : ' ');
//         }
//     }
// }


// int main() {
//     int t;
//     scanf("%d", &t);
//     while (t--) solve();
// }

#include <stdio.h>
using namespace std;

void solve() {
    int n, m;
    scanf("%d%d", &n, &m);
    int nodes[n], nextEdge[m], to[m], degree[n], topoOrder[n];
    for (int i = 0; i < n; i++) nodes[i] = -1, degree[i] = 0;
    for (int i = 0, begin, end; i < m; i++) {
        scanf("%d%d", &begin, &end);
        begin--, end--;
        to[i] = end;
        nextEdge[i] = nodes[begin];
        nodes[begin] = i;
        degree[end]++;
    }
    int q[n], front = 0, rear = 0, cnt = 0;
    for (int i = 0; i < n; i++) if (degree[i] == 0) q[rear++] = i;
    while (front < rear) {
        int u = q[front++];
        topoOrder[cnt++] = u;
        for (int i = nodes[u]; i != -1; i = nextEdge[i]) {
            int v = to[i];
            if (--degree[v] == 0) q[rear++] = v;
        }
    }
    if (cnt < n) printf("0\n");
    else for (int i = 0; i < n; i++) printf("%d%c", topoOrder[i] + 1, i == n - 1 ? '\n' : ' ');
}

int main() {
    int t;
    scanf("%d", &t);
    while (t--) solve();
}
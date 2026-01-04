// #include <bits/stdc++.h>
#include <stdio.h>
#include <queue>
#include <vector>
using namespace std;

struct Edge {
    int to; // 指向
    int cost; // 单位流量的花费
    bool cap; // 容量 true 表示有容量
    int rev; // 反向边在图中的位置
    int next; // 下一条同起点的边 为-1则无
};

int edgeCount;
Edge edges[50*10*2]; // 最多50个男嘉宾 每个男嘉宾10条边 每条边有正反两条边
int mans, womans;
int nodes[50+50+2];    // [0]存源点 [1]存汇点 [2, 51]男嘉宾 [52, 101]女嘉宾
const int manOffset = 2;
const int womanOffset = 52;
const int minInt = -2147483647;

int distances[102];
int predecessors[102];

void addEdge(int from, int to, int cost) {
    edges[edgeCount] = {
        to, cost, true, edgeCount + 1, nodes[from]
    };
    nodes[from] = edgeCount++;
    // 反向边 流量为0 费用为负
    edges[edgeCount] = {
        from, -cost, false, edgeCount - 1, nodes[to]
    };
    nodes[to] = edgeCount++;
}


void SPFA(int* distances, int* predecessors) {
    // 找到从源点到汇点的最短路径
    queue<int> q;
    q.push(0);
    distances[0] = 0;
    while(!q.empty()) {
        int u = q.front(); q.pop();
        for (int i = nodes[u]; i != -1; i = edges[i].next) {    // i是边的索引
            Edge& e = edges[i];
            if (!e.cap) continue; // 无容量
            int v = e.to;
            if (distances[u] == minInt) continue;
            int newDist = distances[u] + e.cost;
            if (distances[v]==minInt || newDist < distances[v]) {
                distances[v] = newDist;
                q.push(v);
                predecessors[v] = e.rev;    // 记录反向边，因为反向边的to才指向上一个
            }
        }
    }
}


void solve() {
    edgeCount = 0;
    scanf("%d%d", &mans, &womans);
    for (int i = 0; i < 102; i++) nodes[i] = -1;
    // 建图
    {
        // 女嘉宾费用
        int* costs[womans];
        for (int i = 0; i < womans; i++) {
            addEdge(i + womanOffset, 1, 0); // 女嘉宾到汇点
            int* costx = costs[i] = new int[mans];
            for (int j = 0; j < mans; j++) {
                scanf("%d", &costx[j]);
            }
        }
        // 建图
        for (int i = 0; i < mans; i++) {
            addEdge(0, i + manOffset, 0); // 源点到男嘉宾
            int womenLikeNum;
            scanf("%d", &womenLikeNum);    // 喜欢几号女嘉宾 从1开始
            int womanId;
            for (int j = 0; j < womenLikeNum; j++) {
                scanf("%d", &womanId);
                womanId--;
                addEdge(i + manOffset, womanId + womanOffset, costs[womanId][i]);
            }
        }
        for (int i = 0; i < womans; i++) delete[] costs[i];
    }
    
    int costSum = 0;
    int pairs = 0;
    while(1) {
        for (int i = 0; i < 102; i++) predecessors[i] = -1;
        for (int i = 0; i < 102; i++) distances[i] = minInt;
        SPFA(distances, predecessors);  // predecessors记录反向边
        if (distances[1] == minInt) {
            break;
        }
        pairs++;
        for (int i = predecessors[1]; i != -1;) {
            Edge& rev = edges[i];
            Edge& e = edges[rev.rev];
            rev.cap = true;
            e.cap = false;
            costSum += e.cost;
            i = predecessors[rev.to];
        }
    }
    printf("%d %d\n", pairs, costSum);
}


int main() {
    int m;
    scanf("%d", &m);
    while(m--) solve();
}
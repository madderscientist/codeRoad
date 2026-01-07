// 我的答案，但过不了；怀疑超时或者爆内存了，所以让ai写了1050_2.cpp，基本贪心策略不变
#include <bits/stdc++.h>
using namespace std;

int m;
int v_n;  // 顶点数
int e_n;  // 边数
int r_n;  // 总订单数
int x;  // 外卖小哥数
int** dis;  // dis[u][v]: u到v的最短时间
string** path;  // path[u][v]: u到v的路径字符串表示

void dijkstra(int* distance, string* path, int from, int* v, int* to, int* time, int* next) {
    typedef pair<int, int> P; // (distance, vertex)
    priority_queue<P, vector<P>, greater<P>> pq;
    fill(distance, distance + v_n, -1);
    int prev[v_n];
    memset(prev, -1, sizeof(prev));
    distance[from] = 0;
    pq.push({0, from});
    while (!pq.empty()) {
        auto [dist, u] = pq.top(); pq.pop();
        if (distance[u] < dist) continue;
        for (int eid = v[u]; eid != -1; eid = next[eid]) {
            int vtx = to[eid];
            int cost = time[eid >> 1];
            if (distance[vtx] == -1 || distance[vtx] > dist + cost) {
                distance[vtx] = dist + cost;
                prev[vtx] = u;
                pq.push({distance[vtx], vtx});
            }
        }
    }
    // 记录路径
    for (int i = 0; i < v_n; i++) {
        if (distance[i] == -1 || i == from) {
            path[i] = "";
            continue;
        }
        vector<int> nodes;
        int cur = i;
        while (cur != -1) {
            nodes.push_back(cur + 1); // 1-based
            if (cur == from) break;
            cur = prev[cur];
        }
        stringstream ss;
        ss << nodes.size();
        for (int idx = nodes.size() - 1; idx >= 0; --idx) ss << " " << nodes[idx];
        path[i] = ss.str();
    }
}

struct order {
    int id; // 1-based
    int from, to;
    int start, ddl;
    order(int id_,int from_, int to_, int start_, int ddl_): id(id_), from(from_), to(to_), start(start_), ddl(ddl_) {}
    // 优先级 ddl越小优先级越大
    bool operator>(const order& other) const {
        if (ddl == other.ddl) return id > other.id;
        return ddl < other.ddl;
    }
    bool operator<(const order& other) const {
        if (ddl == other.ddl) return id > other.id;
        return ddl > other.ddl;
    }
};

// 读取图，得到所有点对最短路径
void readGraph() {
    scanf("%d %d %d %d", &v_n, &e_n, &r_n, &x);
    int e_n2 = e_n << 1;
    int v[v_n];
    int to[e_n2];
    int time[e_n];  // 取值记得左移
    int next[e_n2];
    memset(next, -1, e_n2 * sizeof(int));
    memset(v, -1, sizeof(int) * v_n);
    int tmp_from, tmp_to;
    for (int i = 0, id = 0; i < e_n; i++) {
        scanf("%d %d %d", &tmp_from, &tmp_to, &time[i]);
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
    dis = new int*[v_n];
    path = new string*[v_n];
    for (int i = 0; i < v_n; i++) {
        int* tmp = dis[i] = new int[v_n];
        string* tmp_path = path[i] = new string[v_n];
        memset(tmp, -1, v_n * sizeof(int));
        dijkstra(tmp, tmp_path, i, v, to, time, next);
    }
    // 释放内存交给调用方
}

void readOrders(vector<order>& orders) {
    int tmp_start, tmp_ddl, tmp_from, tmp_to;
    for (int i = 0; i < r_n; i++) {
        scanf("%d %d %d %d", &tmp_from, &tmp_to, &tmp_start, &tmp_ddl);
        tmp_from--;
        tmp_to--;
        if (dis[tmp_from][tmp_to] == -1) continue; // 无法送达
        orders.emplace_back(i + 1, tmp_from, tmp_to, tmp_start, tmp_ddl);
    }
}

struct rider {
    int pos;    // 当前位置
    int time;   // 当前时间
    vector<order*> assigned_orders; // 分配的订单
    rider(): pos(-1), time(0) {}
    void print() {
        if (assigned_orders.empty()) return;
        int current_at = assigned_orders[0]->from;
        for (int i = 0; i < assigned_orders.size(); i++) {
            order* ord = assigned_orders[i];
            if (current_at != ord->from) printf("goto %s\n", path[current_at][ord->from].c_str());
            printf("pick %d\n", ord->id);
            if (ord->from != ord->to) printf("goto %s\n", path[ord->from][ord->to].c_str());
            printf("serve %d\n", ord->id);
            current_at = ord->to;
        }
    }
};

void solve() {
    readGraph();
    // 读取订单
    vector<order> orders;
    readOrders(orders);
    // 优先队列按ddl排序
    priority_queue<order> q(orders.begin(), orders.end());
    
    rider riders[x];
    while(!q.empty()) {
        order ord = q.top(); q.pop();
        // 为订单分配骑手
        int best_idx = -1;
        int min_time = INT_MAX;
        int earst_finish = 144;
        for (int i = 0; i < x; ++i) {
            rider& r = riders[i];
            int to_pick = (r.pos == -1) ? 0 : dis[r.pos][ord.from];
            int to_serve = dis[ord.from][ord.to];
            if (to_pick == -1 || to_serve == -1) continue;  // 图不连通
            int total_time = to_pick + to_serve;
            int finish_time = max(r.time + total_time, ord.start);
            if (finish_time > ord.ddl) continue;
            if (r.pos == -1) {  // 直接选空闲的
                best_idx = i;
                r.pos = ord.from;
                earst_finish = finish_time;
                break;
            }
            // 谁结束时间更早就选谁；如果相同，则选用时最少的。
            if (finish_time > earst_finish) continue;
            if (finish_time == earst_finish) {
                if (total_time >= min_time) continue;
            }
            earst_finish = finish_time;
            min_time = total_time;
            best_idx = i;
        }
        if (best_idx == -1) continue;   // 放弃这一单
        rider& r = riders[best_idx];
        r.time = earst_finish;
        r.pos = ord.to;
        r.assigned_orders.push_back(&orders[ord.id - 1]);
    }
    // 输出结果
    for (int i = 0; i < x; i++) {
        rider& r = riders[i];
        if (r.pos == -1) break;
        printf("solution %d\n", i + 1);
        r.print();
    }
    // 释放内存
    for (int i = 0; i < v_n; i++) {
        delete [] dis[i];
    } delete [] dis;
    for (int i = 0; i < v_n; i++) {
        delete [] path[i];
    } delete [] path;
}

int main() {
    int m;
    scanf("%d", &m);
    while(m--) solve();
}
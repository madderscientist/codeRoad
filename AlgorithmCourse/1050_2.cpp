// 将预计算dijktra结果改为实时计算以节省内存，能过了但分数只有0.752

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <string>
#include <sstream>

using namespace std;

const int INF = 0x3f3f3f3f;

struct Edge {
    int to;
    int weight;
};

struct Order {
    int id; // 1-based
    int from, to;
    int start, ddl;
    
    // Priority Queue: Smallest DDL on top, then largest ID
    bool operator<(const Order& other) const {
        if (ddl != other.ddl) return ddl > other.ddl; 
        return id > other.id;
    }
};

struct Rider {
    int id;
    int pos; // 1-based node index, -1 if initially empty
    int time;
    vector<string> actions; 
    
    Rider(int id_): id(id_), pos(-1), time(0) {}
};

int n, e, r, x;
vector<vector<Edge>> adj;

// Returns distance vector and parent vector from start_node
// parent[v] contains the node that precedes v in the path FROM start_node TO v
void dijkstra(int start_node, vector<int>& dist, vector<int>& parent) {
    dist.assign(n + 1, INF);
    parent.assign(n + 1, -1);
    
    dist[start_node] = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, start_node});
    
    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();
        
        if (d > dist[u]) continue;
        
        for (const auto& edge : adj[u]) {
            if (dist[u] + edge.weight < dist[edge.to]) {
                dist[edge.to] = dist[u] + edge.weight;
                parent[edge.to] = u;
                pq.push({dist[edge.to], edge.to});
            }
        }
    }
}

void solve() {
    if (!(cin >> n >> e >> r >> x)) return;

    adj.assign(n + 1, vector<Edge>());
    
    for (int i = 0; i < e; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    priority_queue<Order> q;
    for (int i = 0; i < r; ++i) {
        int from, to, start, ddl;
        cin >> from >> to >> start >> ddl;
        q.push({i + 1, from, to, start, ddl});
    }

    vector<Rider> riders;
    for(int i=0; i<x; ++i) riders.emplace_back(i+1);

    // Reuse vectors for Dijkstra results
    vector<int> dist;
    vector<int> parent;

    while (!q.empty()) {
        Order ord = q.top();
        q.pop();

        // Run Dijkstra from order source
        dijkstra(ord.from, dist, parent);
        
        // If impossible to deliver (disconnected), skip
        if (dist[ord.to] == INF) continue; 

        int best_idx = -1;
        int min_time = INT_MAX;
        int earst_finish = 144; 

        for (int i = 0; i < x; ++i) {
            Rider& r = riders[i];
            
            int to_pick = 0;
            if (r.pos != -1) {
                if (dist[r.pos] == INF) continue; // Cannot reach pickup from rider
                to_pick = dist[r.pos]; // dist[r.pos] is distance from ord.from to r.pos (symmetric)
            }
            
            int to_serve = dist[ord.to];
            
            int total_time = to_pick + to_serve;
            int finish_time = max(r.time + total_time, ord.start);
            
            // Check deadline constraint
            if (finish_time > ord.ddl) continue;

            // Greedy Logic from 1050.cpp
            // If rider is fresh (pos == -1), they are the best candidate immediately
            // because they start at 0 cost/time relative to pickup location (teleport/spawn)
            if (r.pos == -1) {
                best_idx = i;
                earst_finish = finish_time;
                break; 
            }
            
            if (finish_time > earst_finish) continue;
            if (finish_time == earst_finish) {
                if (total_time >= min_time) continue;
            }
            
            earst_finish = finish_time;
            min_time = total_time;
            best_idx = i;
        }

        if (best_idx != -1) {
            Rider& r = riders[best_idx];
            
            stringstream ss;
            
            // 1. Move to pickup (if not first order)
            if (r.pos != -1 && r.pos != ord.from) {
                // We need path r.pos -> ord.from.
                // We have Dijkstra from ord.from (Source). 
                // parent[v] points towards Source.
                // Path from v to Source is: v, parent[v], parent[parent[v]], ..., Source.
                // This sequence corresponds to movement v -> ... -> Source.
                // We want movement r.pos -> ... -> ord.from.
                // So we just follow parent pointers from r.pos.
                
                vector<int> path_nodes;
                int curr = r.pos;
                while (curr != -1) {
                    path_nodes.push_back(curr);
                    if (curr == ord.from) break;
                    curr = parent[curr];
                }
                
                if (path_nodes.size() > 1) { 
                     ss << "goto " << path_nodes.size();
                     for (int node : path_nodes) ss << " " << node;
                     r.actions.push_back(ss.str());
                     ss.str(""); ss.clear();
                }
            }

            // 2. Pick
            ss << "pick " << ord.id;
            r.actions.push_back(ss.str());
            ss.str(""); ss.clear();

            // 3. Move to delivery
            if (ord.from != ord.to) {
                // We need path ord.from -> ord.to.
                // We have Dijkstra from ord.from.
                // Path from ord.to to ord.from (via parent) is: ord.to -> ... -> ord.from.
                // We want ord.from -> ... -> ord.to.
                // So we follow parent from ord.to, store, and reverse.
                
                vector<int> path_nodes;
                int curr = ord.to;
                while (curr != -1) {
                    path_nodes.push_back(curr);
                    if (curr == ord.from) break;
                    curr = parent[curr];
                }
                reverse(path_nodes.begin(), path_nodes.end());
                
                if (path_nodes.size() > 1) {
                    ss << "goto " << path_nodes.size();
                    for (int node : path_nodes) ss << " " << node;
                    r.actions.push_back(ss.str());
                    ss.str(""); ss.clear();
                }
            }

            // 4. Serve
            ss << "serve " << ord.id;
            r.actions.push_back(ss.str());
            ss.str(""); ss.clear();

            // Update rider state
            r.time = earst_finish;
            r.pos = ord.to;
        }
    }

    // Output
    for (int i = 0; i < x; ++i) {
        if (riders[i].pos == -1) break; // Optimization from 1050.cpp
        
        cout << "solution " << riders[i].id << endl;
        for (const string& act : riders[i].actions) {
            cout << act << endl;
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int m;
    if (cin >> m) {
        bool first = true;
        while(m--) {
            if (!first) cout << endl;
            solve();
            first = false;
        }
    }
    return 0;
}

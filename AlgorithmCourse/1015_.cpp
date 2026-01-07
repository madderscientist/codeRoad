#include <stdio.h>
#include <algorithm>
#include <queue>
#include <vector>
using namespace std;

struct building {
    int a, b, h;
    // h 相同时，b 越大优先级越高，以保持天际线延续，减少抖动
    bool operator<(const building& other) const {
        if (h == other.h) return b < other.b; 
        return h < other.h;
    }
};

bool cmp_build(const building& a, const building& b) {
    if (a.a != b.a) return a.a < b.a;
    return a.h > b.h; // 起点相同时，高的先处理
}

void solve() {
    int N;
    if (scanf("%d", &N) == EOF) return;
    
    // 尽量不使用局部数组 building buildings[N]，防止栈溢出
    building buildings[N];
    for (int i = 0; i < N; i++) {
        building& b = buildings[i];
        scanf("%d %d %d", &(b.a), &(b.b), &(b.h));
    }
    sort(buildings, buildings + N, cmp_build);

    priority_queue<building> q;
    vector<pair<int, int>> track;

    for (int i = 0; i < N; i++) {
        const building& b = buildings[i];
        // 处理在当前建筑起点 b.a 之前就已经结束的建筑（即处理“落差”和“间隙”）
        while (!q.empty() && q.top().b < b.a) {
            int drop_x = q.top().b;
            q.pop();
            // 清理所有在 drop_x 之前或同时结束的建筑，因为它们不再影响天际线
            while (!q.empty() && q.top().b <= drop_x) q.pop();
            int next_h = q.empty() ? 0 : q.top().h;
            track.push_back({drop_x, next_h});
        }
        q.push(b);
        // 当前位置 b.a 的高度由队列中最顶层的建筑决定
        track.push_back({b.a, q.top().h});
    }

    // 收尾：处理剩余在队列中的建筑
    while (!q.empty()) {
        int drop_x = q.top().b;
        q.pop();
        while (!q.empty() && q.top().b <= drop_x) q.pop();
        track.push_back({drop_x, q.empty() ? 0 : q.top().h});
    }

    if (track.empty()) return;
    
    sort(track.begin(), track.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        if (a.first != b.first) return a.first < b.first;
        return a.second > b.second;
    });

    int current_h = -1;
    int current_x = -1;
    for (auto& p : track) {
        if (p.second != current_h && p.first != current_x) {
            printf("%d %d\n", p.first, p.second);
            current_h = p.second;
            current_x = p.first;
        }
    }
}

int main() {
    solve();
    return 0;
}
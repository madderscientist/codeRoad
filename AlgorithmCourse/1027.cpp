#include <bits/stdc++.h>
using namespace std;

struct e {
    int begin;
    int end;
    int w;
    // 按照end排序
    bool operator<(const e& ano) const {
        return this->end < ano.end;
    }
};

void solve() {
    int N;
    cin >> N;
    e events[N];
    int maxTime = -1;
    // 将时间重新标号 目的是减少dp的大小
    unordered_map<int, int> time2idx;
    for (int i = 0; i < N; i++) {
        int g, e, w;
        cin >> g >> e >> w;
        events[i] = {g, e, w};
        time2idx[g] = 1;
        time2idx[e] = 1;
    }
    {
        vector<int> times;
        for (auto& kv : time2idx) {
            times.push_back(kv.first);
        }
        sort(times.begin(), times.end());
        for (int i = 0; i < times.size(); i++) {
            time2idx[times[i]] = i;
        }
        maxTime = times.size() - 1;
        for (int i = 0; i < N; i++) {
            e& event = events[i];
            event.begin = time2idx[event.begin];
            event.end = time2idx[event.end];
        }
    }
    sort(events, events + N);

    // int** dp = new int*[N + 1];
    // for (int i = 0; i <= N; i++) {
    //     dp[i] = new int[maxTime + 1]{0};
    // }
    // dp[i][j]表示用前i个事件，在j时间内的最大权重

    int dp[maxTime+1]{0};
    for (int i = 0; i < N; i++) {
        const e& event = events[i];
        int use_i = dp[event.begin] + event.w;
        for (int j = maxTime; j >= event.end; j--) {
            dp[j] = max(dp[j], use_i);
        }
    }
    cout << dp[maxTime]  << '\n';
}

int main() {
    int m;
    cin >> m;
    while (m--) solve();
}
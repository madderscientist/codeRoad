#include <bits/stdc++.h>
using namespace std;

void solve() {
    int n;
    cin >> n;
    pair<int, int> tasks[n];
    for (int i = 0; i < n; i++) {
        cin >> tasks[i].first >> tasks[i].second;
    }
    sort(tasks, tasks+n);
    int mintime = tasks[0].first;
    int maxtime = tasks[n-1].first;
    int rewards[maxtime]{0};

    int lastvaliable = 0;
    for (int i = 0; i < n; i++) {
        int ddl = tasks[i].first;
        int reward = tasks[i].second;
        if (lastvaliable < ddl) {
            // 还有空位
            rewards[lastvaliable++] = reward;
            continue;
        }
        // 找最小值替换
        int minat = ddl - 1;
        int minval = rewards[minat];
        for (int j = minat - 1; j >= 0; j--) {
            if (minval > rewards[j]) {
                minat = j;
                minval = rewards[j];
            }
        }
        if (minval < reward) {
            rewards[minat] = reward;
        }
    }
    long long sum = 0;
    for (int i = 0 ; i < maxtime; i++) {
        sum += rewards[i];
    }
    cout << sum << '\n';
}


int main() {
    int m;
    cin >> m;
    while(m--) solve();
}
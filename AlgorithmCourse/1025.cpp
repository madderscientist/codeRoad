#include <bits/stdc++.h>
using namespace std;

void solve() {
    int N;
    cin >> N;
    vector<int> v;
    v.reserve(1024);
    for (int i = 0; i < N; i++) {
        int t;
        cin >> t;
        int k = v.size() - 1;
        for (; k >= 0; k--) {
            if (v[k] <= t) break;
        }
        if (k == v.size() - 1) {
            v.push_back(t);
        } else {
            v[k + 1] = t;
        }
    }
    cout << v.size() << endl;
}

int main( ) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int m;
    cin >> m;
    while (m--) solve();
}
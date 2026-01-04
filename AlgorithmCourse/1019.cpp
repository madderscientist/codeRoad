#include <bits/stdc++.h>
using namespace std;

int bp01(int* s, int* v, int N, int C) {
    vector<int> dp(C + 1, 0);
    for (int i = 0; i < N; i++) {
        for (int j = C; j >= s[i]; j--) {
            dp[j] = max(dp[j], dp[j - s[i]] + v[i]);
        }
    }
    return dp[C];
}

int main() {
    int m;
    cin >> m;
    while (m--) {
        int N, C;
        cin >> N >> C;
        int* s = new int[N];
        int* v = new int[N];
        for (int i = 0; i < N; i++) {
            cin >> s[i] >> v[i];
        }
        cout << bp01(s, v, N, C) << '\n';
        delete[] s;
        delete[] v;
    }
}
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
void solve() {
    int N, K;   // 数据和乘号数目
    cin >> N >> K;
    int* inputs = new int[N];
    for (int i = 0; i < N; i++) {
        cin >> inputs[i];
    }
    ll** dp = new ll*[K+1];
    for (int i = 0; i <= K; i++) {
        dp[i] = new ll[N+1]{0};
    }
    // d[k][n]为[0,n)个数插入k个乘号
    for (int i = 1; i <= N; i++) {
        dp[0][i] = inputs[i-1] + dp[0][i-1];
    }


    for (int k = 1; k <= K; k++) {
        // k个乘号需要(k+1)个数
        for (int n = k+1; n <= N; n++) {
            // k-1 个乘号需要k个数
            for (int a = k; a < n; a++) {
                dp[k][n] = max(dp[k][n], dp[k-1][a] * (dp[0][n] - dp[0][a]));
            }
        }
    }
    cout << dp[K][N] << '\n';

    // 收尾
    delete[] inputs;
    for (int i = 0; i <= K; i++) {
        delete [] dp[i];
    }
    delete[] dp;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int m;
    cin >> m;
    while(m--) solve();
}
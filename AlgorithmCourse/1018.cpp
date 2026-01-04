#include <bits/stdc++.h>
using namespace std;

// 大小 价值 数量 容量
int bp01full(int* s, int* v, int N, int C) {
    int** dp = new int*[N + 1];
    for (int i = 0; i <= N; i++) {
        dp[i] = new int[C + 1]{0};
        for (int j = 1; j <= C; j++) {
            dp[i][j] = -1;
        }
        // dp[i][0] = 0 说明有效，为负数说明无效
    }
    // dp[n][c]: using first n items to fill capacity c
    for (int i = 1; i <= N; i++) {  // judge item i-1
        for (int c = 0; c <= C; c++) {
            int withoutMe = dp[i - 1][c];  // 不要
            if (withoutMe >= 0) {
                dp[i][c] = max(dp[i][c], withoutMe);
            }
            if (c - s[i - 1] < 0) continue;
            int withMe = dp[i - 1][c - s[i - 1]];
            if (withMe >= 0) {
                dp[i][c] = max(dp[i][c], withMe + v[i - 1]);
            }
        }
    }
    int result = dp[N][C];
    for (int i = 0; i <= N; i++) {
        delete[] dp[i];
    }
    delete[] dp;
    return max(0, result);
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
        cout << '@' << bp01full(s, v, N, C) << '\n';
        delete[] s;
        delete[] v;
    }
}
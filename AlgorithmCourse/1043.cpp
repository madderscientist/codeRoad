// #include <bits/stdc++.h>
// using namespace std;

// int K = 0, N = 0;
// int* buffer[101]{nullptr};


// int _solve(int level, int k) {
//     if (level < 0) level = 0;
//     if (k == 1) return level;
//     if (level <= 1) return level;
//     int& already = buffer[k-1][level-1];
//     if (already >= 0) return already;
//     int min_times = N;
//     for (int i = 1; i <= level; i++) {
//         // 尝试第i层
//         int lower = _solve(i-1, k-1);
//         int higher = _solve(level - i, k);
//         int sum = (lower > higher ? lower : higher) + 1;
//         if (min_times > sum) min_times = sum;
//     }
//     already = min_times;
//     return min_times;
// }


// void solve() {
//     cin >> K >> N;
//     for (int i = 0; i < K; i++) {
//         buffer[i] = new int[N];
//         memset(buffer[i], -1, N*sizeof(int));
//     }
//     cout << _solve(N, K) << endl;
// }

// int main() {
//     int m;
//     cin >> m;
//     while (m--) {
//         solve();
//         for (int i = 0; i < K; i++) delete [] buffer[i];
//     }
// }
#include <cstdio>

int N, K;
int max(int a, int b) {
    return (a < b) ? b : a;
}
void solve() {
    scanf("%d%d", &K, &N);
    if (K == 1) {
        printf("%d\n", N);
        return;
    }
    int dp1[N+1]{0, 1};
    int dp2[N+1]{0, 1};
    int* dp = dp1;
    int* dp_ = dp2;
    int* temp;
    for (int i = 2; i <= N; i++) dp_[i] = i;
    for (int k = 2; k <= K; k++) {
        int x = 1;
        for (int n = 2; n <= N; n++) {
            while (x < n && max(dp_[x-1], dp[n-x]) > max(dp_[x], dp[n-x-1])) x++;
            dp[n] = 1 + max(dp_[x-1], dp[n-x]);
        }
        temp = dp_; dp_ = dp; dp = temp;
    }
    printf("%d\n", dp_[N]);
}

int main() {
    int m;
    scanf("%d", &m);
    while(m--) solve();
}

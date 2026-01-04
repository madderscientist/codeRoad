#include <stdio.h>
using namespace std;

void solve() {
    int m;
    scanf("%d", &m);
    int days[m];
    for (int i = 0; i < m; i++) scanf("%d", &days[i]);
    int cost[3];
    for (int i = 0; i < 3; i++) scanf("%d", &cost[i]);
    int dp[m + 1]{0};  // dp[i]表示覆盖前i天最小的票钱
    for (int i = 0; i < m; i++) {
        int day = days[i];
        // 1天票
        int cost1 = dp[i] + cost[0];
        // 7天票
        int day7id = i;
        while (day7id >= 0 && days[i] - days[day7id] < 7) day7id--;
        int cost7 = dp[day7id + 1] + cost[1];
        // 30天票
        int day30id = i;
        while (day30id >= 0 && days[i] - days[day30id] < 30) day30id--;
        int cost30 = dp[day30id + 1] + cost[2];

        dp[i + 1] = cost1;
        if (cost7 < dp[i + 1]) dp[i + 1] = cost7;
        if (cost30 < dp[i + 1]) dp[i + 1] = cost30;
    }
    printf("%d\n", dp[m]);
}

int main() {
    int m;
    scanf("%d", &m);
    while (m--) solve();
}
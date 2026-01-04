#include <bits/stdc++.h>
using namespace std;

double dp[1000];

void solve() {
    int n;
    cin >> n;
    if (n == 1) {
        cout << "1.000000\n";
        return;
    }
    int prob_num = (n-1) << 1;
    double prob[prob_num]{.0};
    double tmp;
    for (int i = prob_num - 1; i >= 0;) {
        cin >> tmp;
        tmp /= 100;
        prob[i--] = tmp;    // 奇数为断的概率
        prob[i--] = 1-tmp;  // 偶数为不断
    }
    // dp[i]表示断i个边
    dp[0] = prob[0];
    dp[1] = prob[1];
    for (int i = 2; i < n; i++) {
        dp[i] = 0;
    }
    // 对[i]条边，即当前考虑i+1条边
    for (int i = 1; i < n-1; i++) {
        int base = i << 1;
        double bad = prob[base + 1];
        double good = prob[base];
        for (int j = i+1; j > 0; j--) {
            dp[j] = dp[j-1] * bad + dp[j] * good;
        }
        dp[0] = dp[0] * good;
    }
    double e = 0;
    for (int i = 0; i < n; i++) {
        e += (i+1) * dp[i];
    }
    cout << fixed << setprecision(6) << e << '\n';
}

int main() {
    int m;
    cin >> m;
    while(m--) solve();
}
#include <bits/stdc++.h>
using namespace std;

const int MAXN = 100000;

unsigned int cost[MAXN];
unsigned int need[MAXN];

void solve() {
    int n;
    scanf("%d", &n);
    // unsigned int cost[n];
    // unsigned int need[n];
    for (int i = 0; i < n; i++) scanf("%u", &cost[i]);
    for (int i = 0; i < n; i++) scanf("%u", &need[i]);
    long long min_cost_this_month = cost[0];
    long long min_cost_all = min_cost_this_month * need[0];
    for (int i = 1; i < n; i++) {
        min_cost_this_month++;
        if (min_cost_this_month > cost[i]) {
            min_cost_this_month = cost[i];
        }
        min_cost_all += need[i] * min_cost_this_month;
    }
    printf("%lld\n", min_cost_all);
}

int main() {
    int m;
    scanf("%d", &m);
    while(m--) solve();
}

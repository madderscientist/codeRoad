#include <bits/stdc++.h>
using namespace std;

struct G {
    int len;
    int value;
};

int bp(G* g, int n, int k) {
    int bparr[n + 1]{0};
    for (int i = 0; i <= n; i++) bparr[i] = 0;
    for (int kk = 0; kk < k; kk++) {
        for (int nn = g[kk].len; nn <= n; nn++) {
            bparr[nn] = max(bparr[nn], bparr[nn - g[kk].len] + g[kk].value);
        }
    }
    return bparr[n];
}

int main() {
    int m;
    cin >> m;
    while (m--) {
        int n, k;
        // 钢条长度 不同价格数量
        cin >> n >> k;
        G* g = new G[k];
        for (int i = 0; i < k; i++) {
            cin >> g[i].len >> g[i].value;
        }
        cout << bp(g, n, k) << '\n';
        delete [] g;
    }
}
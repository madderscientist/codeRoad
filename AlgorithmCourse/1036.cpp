#include <bits/stdc++.h>
using namespace std;

void solve() {
    int n, m;
    cin >> n >> m;
    char txt[n + 1];    // 文本串
    char tmp[m + 1];    // 模式串
    scanf("%s", txt);
    scanf("%s", tmp);

    int kmp[m+1]{-1};
    for (int i = 0, j = -1; i < m;) {
        if (j == -1 || tmp[i] == tmp[j]) {
            kmp[++i] = ++j;
        } else {
            j = kmp[j];
        }
    }
    int cnt = 0;
    for (int f = 0, i = 0; f < n;) {
        if (txt[f] == tmp[i]) {
            f++, i++;
            if (i == m) {
                cnt++;
                i = kmp[i];
            }
        } else {
            i = kmp[i];
            if (i == -1) {
                f++, i++;
            }
        }
    }
    printf("%d\n", cnt);
}

int main() {
    int t;
    cin >> t;
    while (t--) solve();
}
#include <bits/stdc++.h>
using namespace std;

void solve() {
    int N, X;
    scanf("%d%d", &N, &X);
    unordered_set<int> s;
    int t;
    for (int i = 0; i < N; i++) {
        scanf("%d", &t);
        if (s.find(t) == s.end()) {
            s.insert(X - t);
        } else {
            printf("yes\n");
            for (int j = i + 1; j < N; j++) scanf("%d", &t);
            return;
        }
    }
    printf("no\n");
}

int main() {
    int m;
    scanf("%d", &m);
    while (m--) solve();
}
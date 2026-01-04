#include <bits/stdc++.h>
using namespace std;

int n;
int temp;

void solve() {
    cin >> n;
    int total = n << 1;
    int black_pos[n];
    int white_pos[n];
    for (int i = 0, bp = 0, wp = 0; i < total; i++) {
        cin >> temp;
        if (temp == 0) {
            black_pos[bp++] = i;
        } else {
            white_pos[wp++] = i;
        }
    }

    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += abs(black_pos[i] - white_pos[i]);
    }
    cout << sum << '\n';
}

int main() {
    int m;
    cin >> m;
    while (m--) solve();
}
#include <stdio.h>
using namespace std;

int mArray[1000];
int nArray[1000];

int min(int a, int b) {
    return a < b ? a : b;
}

void solve() {
    int m, n;
    scanf("%d%d", &m, &n);
    int mid_l = (m + n - 1) >> 1;
    int mid_r = (m + n) >> 1;
    for (int i = 0; i < m; i++) {
        scanf("%d", &mArray[i]);
    }
    for (int i = 0; i < n; i++) {
        scanf("%d", &nArray[i]);
    }
    int mp = 0, np = 0;
    int cnt = -1;
    int mid_l_val, mid_r_val;
    int current;
    // printf("mid_l:%d mid_r:%d\n", mid_l, mid_r);
    while(1) {
        if (cnt == mid_l) {
            mid_l_val = current;
        }
        if (cnt == mid_r) {
            mid_r_val = current;
            float median = (mid_l_val + mid_r_val) / 2.0;
            printf("%.5f\n", median);
            return;
        }
        if (mp == m || (np < n && mArray[mp] > nArray[np])) {
            current = nArray[np];
            np++;
        } else {
            current = mArray[mp];
            mp++;
        }
        cnt++;
    }
}

int main() {
    int t;
    scanf("%d", &t);
    while (t--) solve();
}
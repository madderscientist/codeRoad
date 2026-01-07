#include <stdio.h>
using namespace std;

void solve() {
    int m, n, target;
    scanf("%d%d%d", &m, &n, &target);
    int x;
    bool found = false;
    for (int i = m * n; i > 0; i--) {
        scanf("%d", &x);
        if (x == target) {
            printf("true\n");
            for (i--; i > 0; i--) scanf("%d", &x);
            return;
        }
    }
    printf("false\n");
}

int main() {
    int t;
    scanf("%d", &t);
    while (t--) solve();
}

// 正规的方式是从右上角开始遍历
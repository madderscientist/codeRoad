#include <stdio.h>
#include <algorithm>
#include <cmath>
using namespace std;

/*
必须根据右端点排序。用x排序反例：
d = 2
(0.999, 1)
(1, 1.9999)
*/

struct c {
    double l, r;
    bool operator<(const c& ano) const {
        return r < ano.r;
    }
};

void solve() {
    int n, d;
    scanf("%d %d", &n, &d);
    c boats[n];
    double xx, yy;
    for (int i = 0; i < n; i++) {
        auto& p = boats[i];
        scanf("%lf %lf", &xx, &yy);
        double r = sqrt(d*d-yy*yy);
        p.r = xx + r;
        p.l = xx - r - 1e-8;
    }
    sort(boats, boats + n);
    int num = 0;
    double lastRight = -d-1;
    for (c& b : boats) {
        if (lastRight < b.r + 1e-8 && lastRight > b.l) continue;
        num++;
        lastRight = b.r;
    }
    printf("%d\n", num);
}

int main() {
    int m;
    scanf("%d", &m);
    while(m--) solve();
}
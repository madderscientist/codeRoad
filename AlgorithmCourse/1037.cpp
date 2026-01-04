#include <stdio.h>
using namespace std;

char match[50]; // 女嘉宾被谁牵手 为-1则无人
int* like[50];

bool findWay(int man, bool* hasman) {
    int* Ilike = like[man];
    for (int i = Ilike[0]; i > 0; i--) {
        int women = Ilike[i];
        if (hasman[women]) continue;
        hasman[women] = true;
        if (match[women] == -1 || findWay(match[women], hasman)) {
            match[women] = man;
            return true;
        }
    } return false;
}

void solve() {
    int n, m;
    scanf("%d%d", &n, &m);
    for (int i = 0; i < n; i++) {
        int k;
        scanf("%d", &k);
        int* Ilike = like[i];
        Ilike[0] = k;
        for (int j = 1; j <= k; j++) {
            int tmp;
            scanf("%d", &tmp);
            Ilike[j] = tmp-1;
        }
    }
    int matchNum = 0;
    bool hasman[m];
    for (int i = 0 ; i < m; i++) match[i] = -1;
    for (int i = 0; i < n; i++) {
        for (int a = 0; a < m; a++) hasman[a] = false;
        if (findWay(i, hasman)) matchNum++;
    }
    printf("%d\n", matchNum);
}

int main() {
    for (int i = 0; i < 50; i++) like[i] = new int[11];
    int m; scanf("%d", &m);
    while(m--) solve();
    for (int i = 0; i < 50; i++) delete[] like[i];
}
#include <bits/stdc++.h>
using namespace std;

int buff[1002]{9999};
int len;

void down(int u) {
    int t = u;
    if((u << 1 ) + 1 <= len && buff[(u << 1) + 1] < buff[t]) t = (u << 1) + 1;
    if(u << 1 <= len && buff[u << 1] < buff[t]) t = u << 1;
    if(t != u) {
        swap(buff[u], buff[t]);
        down(t);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int m;
    cin >> m;
    while(m--) {
        cin >> len;
        for (int i = 1; i <= len; i++) {
            cin >> buff[i];
        }
        for (int i = len >> 1; i >= 1; i--) {
            down(i);
        }
        for (int i = 1; i <= len; i++) {
            cout << buff[i] << (i == len ? '\n' : ' ');
        }
    }
    return 0;
}
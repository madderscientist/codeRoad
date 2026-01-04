#include <bits/stdc++.h>
using namespace std;

int buff[50001]{0};

int mx(int len) {
    int m = buff[0];
    for (int i = 1; i < len; i++) {
        buff[i] = max(buff[i-1]+buff[i], buff[i]);
        m = max(m, buff[i]);
    }
    return m;
}


int main() {
    int m;
    cin >> m;
    while(m--) {
        int n;
        cin >> n;
        for (int i = 0; i < n; i++) {
            cin >> buff[i];
        }
        cout << mx(n) << '\n';
    }
}
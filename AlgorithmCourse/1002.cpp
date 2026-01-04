#include <bits/stdc++.h>
using namespace std;
int m;
int main() {
    cin >> m;
    while(m--) {
        int n;
        cin >> n;
        int min1 = -1;  // 最小
        int min2 = -1;
        while(n--) {
            int a;
            cin >> a;
            if (a <= min1 || min1 == -1) {
                min2 = min1;
                min1 = a;
            } else if (a < min2 || min2 == -1) {
                min2 = a;
            }
        }
        cout << min2 << endl;
    } return 0;
}
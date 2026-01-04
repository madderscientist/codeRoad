#include <bits/stdc++.h>
using namespace std;

int buffer[101];
int s[101];
/*
某国为了防御敌国的导弹袭击，发展出一种导弹拦截系统。但是这种导弹拦截系统有一个缺陷：虽然它的第一发炮弹能够到达任意的高度，
但是以后每一发炮弹都不能高于前一发的高度。某天，雷达捕捉到敌国的导弹来袭。由于该系统还在试用阶段，所以只有一套系统，因此
有可能不能拦截所有的导弹。

对于每组输入数据，第一行输出这套系统最多能拦截多少导弹，以及输出如果要拦截所有导弹最少要配备多少套这种导弹拦截系统。
*/
int lanjie(int* h, int len, int* s = nullptr) {
    bool needStack = (s == nullptr);
    if (needStack) {
        s = new int[len];
    }
    int top = 0;    // 指向下一个能放的位置
    for (int c = 0; c < len; c++) {
        int p = top;
        while (p > 0 && h[c] > h[s[p - 1]]) {
            p--;
        }
        s[p] = c;
        top = max(top, p + 1);
    }


    if (needStack) {
        delete[] s;
    }
    return top;
}

int count(int* h, int len, int* judge) {
    for (int c = 0; c < len; c++) {
        judge[c] = 0;
    }
    for (int c = 0; c < len; c++) {
        int now = 65535;
        bool flag = true;
        for (int i = 0; i < len; i++) {
            if (judge[i] == 1) continue;
            if (h[i] < now) {
                now = h[i];
                judge[i] = 1;
                flag = false;
            }
        }
        if (flag) return c;
    }
    return len;
}


int main() {
    int m;
    cin >> m;
    while (m--) {
        int n;
        cin >> n;
        for (int c = 0; c < n; c++) {
            cin >> buffer[c];
        }
        cout << lanjie(buffer, n, s) << ' ' << count(buffer, n, s) << endl;
    }
    return 0;
}
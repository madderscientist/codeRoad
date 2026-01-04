#include <bits/stdc++.h>
using namespace std;

int buffer[101];
/*
某国为了防御敌国的导弹袭击，发展出一种导弹拦截系统。但是这种导弹拦截系统有一个缺陷：虽然它的第一发炮弹能够到达任意的高度，
但是以后每一发炮弹都不能高于前一发的高度。某天，雷达捕捉到敌国的导弹来袭。由于该系统还在试用阶段，所以只有一套系统，因此
有可能不能拦截所有的导弹。
*/
int lanjie(int* h, int len, int* s = nullptr) {
    bool needStack = (s == nullptr);
    if (needStack) {
        s = new int[len];
    }
    int top = 0;    // 指向下一个能放的位置
    for (int i = 0; i < len; i++) {
        int p = top;
        while (p > 0 && h[i] > h[s[p - 1]]) {
            p--;
        }
        // cout << "i=" << i << ", h[i]=" << h[i] << ", p=" << p << endl;
        s[p] = i;
        top = max(top, p + 1);
    }


    if (needStack) {
        delete[] s;
    }
    return top;
}


int main() {
    int m;
    cin >> m;
    while (m--) {
        int n;
        cin >> n;
        for (int i = 0; i < n; i++) {
            cin >> buffer[i];
        }
        cout << lanjie(buffer, n) << endl;
    }
    return 0;
}
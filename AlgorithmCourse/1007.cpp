#include <bits/stdc++.h>
using namespace std;

int getNumber(int* arr, int i, int j) {
    int result = 0;
    for (int k = i; k < j; k++) {
        result = result * 10 + arr[k];
    }
    return result;
}

int getMult(int* arr, int len, int k) {
    int** f = new int*[len + 1];
    for (int i = 1; i <= len; i++) {
        // 0个乘号
        f[i] = new int[k + 1]{getNumber(arr, 0, i)};
    }
    // f[i][j] 表示前i个数插入j个乘号的最大乘积
    // f[i][j+1] = max(f[t][j] * getNumber(t+1, i)), t < i
    // f[i][0] = 前i个数组成的数字
    for (int i = 1; i <= len; i++) {
        for (int j = 1; j <= k; j++) {
            for (int t = 1; t < i; t++) {
                f[i][j] = max(f[i][j], f[t][j - 1] * getNumber(arr, t, i));
            }
        }
    }
    int result = f[len][k];
    for (int i = 1; i <= len; i++) {
        delete[] f[i];
    }
    delete[] f;
    return result;
}


int main() {
    int m;
    cin >> m;
    while(m--) {
        int N, K;
        cin >> N >> K;
        int* arr = new int[N];
        for (int i = 0; i < N; i++) {
            char x;
            cin >> x;
            arr[i] = x - '0';
        }
        cout << getMult(arr, N, K) << '\n';
        delete[] arr;
    }
}
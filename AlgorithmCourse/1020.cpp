#include <bits/stdc++.h>
using namespace std;

/*
两个矩阵A(r行s列)和B(s行t列)相乘, 乘法代价为r*s*t. 现给定N(N<=500)个矩阵连乘问题, 请计算最小乘法代价。
第一行输入M(M<=10)表示有M组数据。每组数据第一行输入N，表示矩阵个数；接下来一行输入N个矩阵的行数和列数。
*/

struct mat {
    int h;
    int w;
};

int _bp(int** bp, mat* mats, int start, int end) {
    if (bp[start][end] >= 0) return bp[start][end];
    int min = -1;
    for (int c = start; c < end; c++) {
        int newvalue = _bp(bp, mats, start, c) + _bp(bp, mats, c+1, end) + mats[start].h * mats[c].w * mats[end].w;
        if (min == -1 || min > newvalue) min = newvalue;
    }
    bp[start][end] = min;
    return min;
}

int bpmat(mat* mats, int n) {
    int** bp = new int*[n];
    for (int i = 0; i < n; i++) {
        bp[i] = new int[n];
        for (int j = 0; j < n; j++) {
            bp[i][j] = -1;
        }
        bp[i][i] = 0;
    }
    int result = _bp(bp, mats, 0, n-1);
    for (int i = 0; i < n; i++) {
        delete [] bp[i];
    }
    delete [] bp;
    return result;
}

int main() {
    int m;
    cin >> m;
    while (m--) {
        int n;
        cin >> n;
        mat* mats = new mat[n];
        for (int i = 0; i < n; i++) {
            cin >> mats[i].h >> mats[i].w;
        }
        cout << bpmat(mats, n) << '\n';
        delete [] mats;
    }
}
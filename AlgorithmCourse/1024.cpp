#include <bits/stdc++.h>
using namespace std;

// 对称上三角矩阵
template <class T>
class TriangleMat {
public:
    int width;
    int size;
    T* data;
    TriangleMat(int w, T init = -1) : width(w) {
        size = (w * (w + 1)) >> 1;
        data = new T[size];
        for (int i = 0; i < size; i++) data[i] = init;
    }

    ~TriangleMat() {
        delete[] data;
    }

    int id(int i, int j) const {
        if (i > j) swap(i, j);
        int befRowNum = ((width + width - i + 1) * i) >> 1;
        int jOffset = j - i;
        return jOffset + befRowNum;
    }

    T& operator() (int i, int j) {
        return data[id(i, j)];
    }
};

void solve() {
    int N;
    cin >> N;
    // vector<int> key;
    // key.reserve(N);
    for (int i = 0; i < N; i++) {
        int temp;
        cin >> temp;
        // key.push_back(temp);
    }
    vector<double> prob;
    prob.reserve(N);
    for (int i = 0; i < N; i++) {
        double p;
        cin >> p;
        prob.push_back(p);
    }
    vector<double> outProb;
    outProb.reserve(N+1);
    for (int i = 0; i <= N; i++) {
        double p;
        cin >> p;
        outProb.push_back(p);
    }

    TriangleMat<double> bp(N, 0);
    TriangleMat<double> p(N, 0);
    // bp[i][j]为[i,j]的节点的最小代价
    // p[i][j]为[i,j]的概率
    for (int i = 0; i < N; i++) {
        bp(i, i) = p(i, i) = prob[i] + outProb[i] + outProb[i+1];
    }
    for (int j = 1; j < N; j++) {
        for (int i = 0; i < N; i++) {
            int col = i + j;
            if (col >= N) break;
            double ppp = p(i, col) = p(i, col - 1) + prob[col] + outProb[col + 1];
            bp(i, col) = ppp + min(
                // 选第一个点为root
                bp(i+1, col),
                // 选最后一个点为root
                bp(i, col - 1)
            );
            for (int k = i + 1; k < col; k++) {
                bp(i, col) = min(
                    bp(i, col), 
                    bp(i, k - 1) + bp(k + 1, col) + ppp
                );
            }
        }
    }
    cout << fixed << setprecision(6) << bp(0, N - 1) << endl;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int m;
    cin >> m;
    while (m--) solve();
}
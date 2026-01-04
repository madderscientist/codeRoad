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

TriangleMat<int> e(500, -1);

void solve() {
    int n, E, s, t;
    cin >> n >> E >> s >> t;
    s--, t--;
    memset(e.data, 0xff, sizeof(e.data[0])*e.size);
    while (E--) {
        int u, v, w;
        cin >> u >> v >> w;
        int& x = e(--u, --v);
        if (x == -1 || x > w) x = w;
    }
    int distance[n]{0};
    bool visited[n]{0};
    visited[s] = true;
    for(int i = 0; i < n; i++) {
        distance[i] = e(s, i);
    }
    while(1) {
        // 找到最小的点
        int minat = -1;
        int minvalue = -1;
        for (int i = 0; i < n; i++) {
            if (visited[i]) continue;
            if (distance[i] == -1) continue;
            if (minat == -1 || distance[i] < minvalue) {
                minat = i;
                minvalue = distance[i];
            }
        }
        // 拓展
        if (minat == -1) break;
        visited[minat] = true;
        // 用minat的边值更新distance
        for (int k = 0; k < n; k++) {
            if (visited[k] == true) continue;
            int dis = e(minat, k);  // 到k的距离
            if (dis == -1) continue;
            dis += distance[minat];
            if (distance[k] == -1 || distance[k] > dis) {
                distance[k] = dis;
            }
        }
    }
    cout << distance[t] << '\n';
}

int main() {
    int m;
    cin >> m;
    while (m--) solve();
}
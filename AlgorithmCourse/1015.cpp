#include <bits/stdc++.h>
using namespace std;

struct building {
    int l;
    int r;
    int h;
};

int main() {
    int N;
    cin >> N;
    building bs[N];
    unordered_map<int, int> pos2idx;
    for (int i = 0; i < N; i++) {
        int l, r, h;
        cin >> l >> r >> h;
        bs[i] = {l, r, h};
        pos2idx[l] = 1;
        pos2idx[r] = 1;
    }

    vector<int> pos;
    for (auto& kv : pos2idx) {
        pos.push_back(kv.first);
    }
    sort(pos.begin(), pos.end());
    for (int i = 0; i < pos.size(); i++) {
        pos2idx[pos[i]] = i;
    }

    int line[pos.size()]{0};
    for (auto& b : bs) {
        int left = pos2idx[b.l];
        int right = pos2idx[b.r];
        for (int i = left; i < right; i++) {
            line[i] = max(line[i], b.h);
        }
    }
    int last = -1;
    for (int i = 0; i < pos.size(); i++) {
        if (last == line[i]) continue;
        last = line[i];
        printf("%d %d\n", pos[i], line[i]);
    }
}
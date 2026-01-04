#include <bits/stdc++.h>
using namespace std;

// 对称上三角矩阵
class TriangleMat {
public:
    int width;
    int size;
    int* data;
    TriangleMat(int w) : width(w) {
        size = (w * (w + 1)) >> 1;
        data = new int[size];
        for (int i = 0; i < size; i++) data[i] = -1;
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

    int get(int i, int j) const {
        int ID = id(i, j);
        if (ID >= size) {
            return -1;
        }
        return data[ID];
    }

    void set(int i, int j, int value) {
        int ID = id(i, j);
        if (ID >= size) {
            return;
        }
        data[ID] = value; 
    }
};

struct state {
    int moneyLeft;
    int walked;
    int country;
};

void solve() {
    int countryNum, roadNum, myCountry, tgtCountry, money;
    cin >> countryNum >> roadNum >> myCountry >> tgtCountry >> money;
    myCountry--;
    tgtCountry--;
    if (countryNum <= 0 || myCountry < 0 || myCountry >= countryNum || tgtCountry < 0 || tgtCountry >= countryNum || roadNum <= 0) {
        cout << -1 << '\n';
        return;
    }
    vector<int> pays(countryNum);
    int minpay = -1;
    for (int i = 0; i < countryNum; i++) {
        int t;
        cin >> t;
        pays[i] = t;
        if (minpay == -1 || minpay > t) minpay = t;
    }
    pays[myCountry] = 0;
    // 由于边数和国家数^2同一个量级，所以用邻接矩阵
    TriangleMat roads(countryNum);
    int _rl, _from, _to;
    while (roadNum--) {
        cin >> _from >> _to >> _rl;
        if (_from == _to) continue; // 对角线置-1
        _from --;
        _to --;
        if (_from >= countryNum || _to >= countryNum || _from < 0 || _to < 0) continue;
        int v = roads.get(_from, _to);
        if (v == -1 || v > _rl) roads.set(_from, _to, _rl);
    }
    // 搜路
    // 剪枝：重复到一个位置，如果现有方案用的钱更少且路程更短就不继续
    vector<int> countryMaxMoney(countryNum, -1);
    vector<int> countryMinDis(countryNum, -1);

    queue<state> BFSstate;
    BFSstate.push({money, 0, myCountry});
    countryMaxMoney[myCountry] = money;
    countryMinDis[myCountry] = 0;
    int result = -1;

    while (!BFSstate.empty()) {
        state s = BFSstate.front();
        BFSstate.pop();
        int country_now = s.country;
        for (int i = 0; i < countryNum; i++) {
            int rl = roads.get(country_now, i);
            if (rl < 0) continue;
            int money_left = s.moneyLeft - pays[i];
            int walked = s.walked + rl;
            if (money_left < 0) continue;
            if (i == tgtCountry) {  // 到终点了！
                if (result == -1 || result > walked) result = walked;
            } else {
                if (money_left < minpay) continue;
                if (countryMinDis[i] == -1) {  // 没到过
                    BFSstate.push({money_left, walked, i});
                    countryMaxMoney[i] = money_left;
                    countryMinDis[i] = walked;
                    continue;
                }
                if (money_left < countryMaxMoney[i] && walked > countryMinDis[i]) continue;
                BFSstate.push({money_left, walked, i});
                if (money_left >= countryMaxMoney[i] && walked <= countryMinDis[i]) {
                    countryMaxMoney[i] = money_left;
                    countryMinDis[i] = walked;
                }
            }
        }
    }
    cout << result << '\n';
}

int main() {
    int m;
    cin >> m;
    while (m--) solve();
}
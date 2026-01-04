#include <bits/stdc++.h>
using namespace std;

// 上三角矩阵类
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
	~TriangleMat() { delete[] data; }
	int id(int i, int j) const {
		if (i > j) swap(i, j);
		int befRowNum = ((width + width - i + 1) * i) >> 1;
		int jOffset = j - i;
		return jOffset + befRowNum;
	}
	int get(int i, int j) const {
		int ID = id(i, j);
		if (ID >= size) return -1;
		return data[ID];
	}
	void set(int i, int j, int value) {
		int ID = id(i, j);
		if (ID >= size) return;
		data[ID] = value;
	}
};

struct vertex {
	int m, dis, v;
};
struct mycompare {
	bool operator()(vertex a, vertex b) {
		return a.dis > b.dis;
	}
};

void solve() {
	int n, E, s, t, M;
	cin >> n >> E >> s >> t >> M;
	s--, t--;
	vector<int> cost(n);
	for (int &v : cost) cin >> v;
	TriangleMat roads(n);
	for (int i = 0; i < E; ++i) {
		int x, y, z;
		cin >> x >> y >> z;
		--x, --y;
		if (x == y) continue;
		int v = roads.get(x, y);
		if (v == -1 || v > z) roads.set(x, y, z);
	}
	using pii = pair<int, int>; // (country, money)
	unordered_map<long long, int> dist; // key: (country<<20)|money, value: min distance
	auto key = [](int v, int m) { return (1LL * v << 20) | m; };
	priority_queue<pair<int, pii>, vector<pair<int, pii>>, greater<>> pq;
	dist[key(s, M)] = 0;
	pq.push({0, {s, M}});
	int ans = -1;
	while (!pq.empty()) {
		auto [d, pr] = pq.top(); pq.pop();
		int v = pr.first, m = pr.second;
		if (v == t) { ans = d; break; }
		// 枚举所有邻居
		for (int u = 0; u < n; ++u) {
			if (u == v) continue;
			int rl = roads.get(v, u);
			if (rl < 0) continue;
			int nm = m - cost[u];
			if (nm < 0) continue;
			long long nk = key(u, nm);
			int nd = d + rl;
			if (!dist.count(nk) || nd < dist[nk]) {
				dist[nk] = nd;
				pq.push({nd, {u, nm}});
			}
		}
	}
	cout << ans << endl;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	int T;
	cin >> T;
	while (T--) solve();
}
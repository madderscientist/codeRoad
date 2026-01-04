#include <iostream>
#include <cstring>
#include <algorithm>
#include <unordered_map>
using namespace std;

const int N = 100100;
int h[N];  // 与该节点(父节点)相连的最后一条边(索引最大的)
int e[N];  // 边i指向的节点
int ne[N];  // 边i的上一条边
int idx = 0;  // 边的下标索引
bool st[N];  // 该节点是否计算过
int dp[N][2];

// 添加一条a到b的边
void add(int a, int b)
{
    e[idx] = b;
    ne[idx] = h[a];
    h[a] = idx++;
}

void dfs(int root)
{
    // 叶节点
    if(h[root] == -1) {
        dp[root][0] = 0;
        dp[root][1] = 1;
        return ;
    }
    
    // 重置
    dp[root][0] = dp[root][1] = 0;
    
    for(int i = h[root]; i != -1; i = ne[i]) {
        int j = e[i];  // 子节点
        // 还没有计算过
        if( !st[j] ) {
            dfs(j);
        }
        dp[root][0] += max(dp[j][0], dp[j][1]);
        dp[root][1] += dp[j][0];
    }
    // 加上自身的黑色
    dp[root][1]++;
}

int main()
{
    int t;
    cin >> t;
    while( t-- ) {
        int n;
        cin >> n;
        idx = 0;
        memset(h, -1, sizeof(h));
        memset(dp, 0, sizeof(dp));
        // memset(e, 0, sizeof(e));
        // memset(ne, 0, sizeof(ne));
        memset(st, false, sizeof(st));
        // 记录每个节点父节点个数
        unordered_map<int, int> pnums;
        for(int i = 0; i < n-1; i++) {
            int a, b;
            cin >> a >> b;
            add(a, b);
            pnums[b]++;
            pnums[a] += 0;
        }
        
        int root = -1;
        for(auto t : pnums) {
            if(t.second == 0) {
                root = t.first;
                break;
            }
        }
        dfs(root);
        cout << max(dp[root][0], dp[root][1]) << endl;
    }
    
    return 0;
}

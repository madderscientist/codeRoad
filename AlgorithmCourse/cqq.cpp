#include <bits/stdc++.h>
using namespace std;
// 戳气球
class Solution {
public:
    int maxCoins(vector<int>& nums) {
        const int size = nums.size();
        nums.insert(nums.begin(), 1);
        nums.push_back(1);
        // dp[i][j]表示戳爆i和j之间的气球，最大的得分
        int* dp[size+2];
        for (int i = 0; i < size+2; i++)
            dp[i] = new int[size+2-i]{0};
        // 从下往上 从左到右遍历
        for (int row = size; row >= 0; row--) {
            for (int col = row+1; col < size+2; col++) {
                int& m = dp[row][col-row];
                // 遍历row和col之间的每一种戳破可能
                for (int i = row+1; i < col; i++) {
                    m = max(m,
                        dp[row][i-row] + dp[i][col-i] + 
                        nums[i]*nums[row]*nums[col]
                    );
                }
            }
        }
        int result = dp[0][size+1];
        for (int i = 0; i < size+2; i++) delete[] dp[i];
        return result;
    }
};

int main() {
    Solution s;
    vector<int> ques{3,1,5,8};
    cout << s.maxCoins(ques);
}
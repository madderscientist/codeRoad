#include <bits/stdc++.h>
using namespace std;

unsigned short shareLen(string& a, string& b) {
    unsigned short la = a.length();
    unsigned short lb = b.length();
    // unsigned short** dp = new unsigned short*[la + 1];
    // for (int i = 0; i <= la; i++) {
    //     dp[i] = new unsigned short[lb + 1]{0};
    // }

    // for (int i = 1; i <= la; i++) {
    //     for (int j = 1; j <= lb; j++) {
    //         if (a[i-1] == b[j-1]) {
    //             dp[i][j] = dp[i-1][j-1] + 1;
    //         } else {
    //             dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
    //         }
    //     }
    // }

    // unsigned short result = dp[la][lb];
    // for (int i = 0; i <= la; i++) delete[] dp[i];
    // delete[] dp;
    // return result;

    // unsigned short* dp = new unsigned short[lb + 1]{0};
    // unsigned short* dp2 = new unsigned short[lb + 1]{0};
    // for (int i = 1; i <= la; i ++) {
    //     for (int j = 1; j <= lb; j++) {
    //         if (a[i-1] == b[j-1]) {
    //             dp[j] = dp2[j-1] + 1;
    //         } else {
    //             dp[j] = max(dp2[j], dp[j-1]);
    //         }
    //     }
    //     unsigned short* temp = dp2;
    //     dp2 = dp;
    //     dp = temp;
    // }
    // auto result = dp2[lb];
    // delete[] dp;
    // delete[] dp2;
    // return result;

    unsigned short* dp = new unsigned short[lb + 1]{0};
    for (int i = 1; i <= la; i++) {
        unsigned short prev = 0;  // 代表 dp2[j-1]
        for (int j = 1; j <= lb; j++) {
            unsigned short tmp = dp[j];  // 记录当前 dp[j]，即 dp2[j]
            if (a[i - 1] == b[j - 1]) {
                dp[j] = prev + 1;
            } else {
                dp[j] = max(dp[j], dp[j - 1]);
            }
            prev = tmp;
        }
    }
    auto result = dp[lb];
    delete[] dp;
    return result;
}

int main() {
    int m;
    cin >> m;
    while (m--) {
        string a, b;
        cin >> a >> b;
        if (a.length() > b.length()) {
            cout << shareLen(a, b) << '\n';
        } else {
            cout << shareLen(b, a) << '\n';
        }
    }
    return 0;
}
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <queue>
#include <stack>
#include <string>
#include <vector>
// 电路布线 其实就是逆序对
using namespace std;
int num[50001];
int n;
int ans = 0;
void merge(int left, int mid, int right) {
    int* temp = new int[right - left + 1];
    int l = left, r = mid + 1;
    int i = 0;
    while (l <= mid && r <= right) {
        if (num[l] <= num[r])
            temp[i++] = num[l++];
        else {
            temp[i++] = num[r++];
            ans += mid + 1 - l;
        }
    }
    while (l <= mid) {
        temp[i++] = num[l++];
    }
    while (r <= right) temp[i++] = num[r++];
    int k = 0;
    for (i = left; i <= right; i++) {
        num[i] = temp[k];
        k++;
    }

    delete[] temp;
}
void parti(int l, int r) {
    if (l < r) {
        int mid = (l + r) / 2;
        parti(l, mid);
        parti(mid + 1, r);
        merge(l, mid, r);
    }
}
int main() {
    int t;
    scanf("%d", &t);
    while (t--) {
        ans = 0;
        scanf("%d", &n);
        for (int i = 0; i < n; i++) scanf("%d", &num[i]);
        parti(0, n - 1);
        printf("%d\n", ans);
    }
    return 0;
}
#include <bits/stdc++.h>
using namespace std;

int buff[20001]{0};

int fff(int* arr, int low, int high, int target) {
    int father;
    while (low != high) {
        int mid = (low + high) >> 1;
        if (arr[mid] < target) {
            low = mid + 1;
            father = mid;
        } else if (arr[mid] > target) {
            high = mid - 1;
            father = mid;
        } else {
            high = low = mid;
        }
    }
    if (arr[low] == target) {
        return father;
    }
    return - low - 1;
}

int main() {
    int m;
    cin >> m;
    while (m--) {
        int n, x;
        cin >> n >> x;
        for (int i = 0; i < n; i++) {
            cin >> buff[i];
        }
        int i = fff(buff, 0, n - 1, x);
        if (i < 0) printf("not found, father is %d\n", buff[-i - 1]);
        else printf("success, father is %d\n", buff[i]);
    }
}
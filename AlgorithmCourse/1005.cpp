#include <bits/stdc++.h>
using namespace std;

int temp[1002]{0};
void quickSort(int* arr, int len, int layer = 0) {
    if (len <= 1) return;
    int flag = arr[0];
    int p = 0;
    for (int i = 1; i < len; i++) {
        if (arr[i] < flag) {
            swap(arr[i], arr[++p]);
        }
    }
    swap(arr[0], arr[p]);
    if (layer >= 1) return;
    quickSort(arr, p, layer + 1);
    quickSort(arr + p + 1, len - p - 1, layer + 1);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int m;
    cin >> m;
    while(m--) {
        int n;
        cin >> n;
        int* arr = new int[n];
        for (int i = 0; i < n; i++) {
            cin >> arr[i];
        }
        quickSort(arr, n);
        for (int i = 0; i < n; i++) {
            cout << arr[i] << (i == n - 1 ? '\n' : ' ');
        }
        delete[] arr;
    }
    return 0;
}
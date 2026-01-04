#include <bits/stdc++.h>
using namespace std;
int temp[1002]{0};
void merge1(int* arr, int len, int layer = 0) {
    if (len <= 1) return;
    int mid = (len-1) >> 1;
    merge1(arr, mid + 1, layer + 1);
    merge1(arr + mid + 1, len - mid - 1, layer + 1);
    if (layer <= 1) return;
    int i = 0, j = mid + 1, k = 0;
    while (i <= mid && j < len) {
        if (arr[i] < arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    while (j < len) {
        temp[k++] = arr[j++];
    }
    for (int i = 0; i < len; ++i) arr[i] = temp[i];
}

int main() {
    int m;
    cin >> m;
    while(m--) {
        int n;
        cin >> n;
        int* arr = new int[n];
        for (int i = 0; i < n; ++i) {
            cin >> arr[i];
        }
        merge1(arr, n);
        for (int i = 0; i < n; ++i) {
            cout << arr[i];
            if (i != n - 1) cout << " ";
        }
        cout << endl;
    } return 0;
}
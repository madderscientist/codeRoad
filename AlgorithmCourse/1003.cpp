#include <bits/stdc++.h>
using namespace std;

void bubbleSort1(vector<int>& arr) {
    int n = arr.size();
    for (int j = 0; j < n - 1; ++j) {
        if (arr[j] > arr[j + 1]) {
            swap(arr[j], arr[j + 1]);
        }
    }
}


int main() {
    int m;
    cin >> m;
    while(m--) {
        int n;
        cin >> n;
        vector<int> arr(n);
        for (int i = 0; i < n; ++i) {
            cin >> arr[i];
        }
        bubbleSort1(arr);
        for (int i = 0; i < n; ++i) {
            cout << arr[i] << " ";
        }
        cout << endl;
    }
    return 0;
}
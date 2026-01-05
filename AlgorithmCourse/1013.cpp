#include <stdio.h>
using namespace std;

int sum = 0;
// 逆序对
void merge(int* data, int* buffer, int left, int right) {
    if (right - left <= 1) return;
    int mid = (left + right) >> 1;
    merge(data, buffer, left, mid);
    merge(data, buffer, mid, right);
    int i = left, j = mid, k = left;
    while (i < mid && j < right) {
        if (data[i] <= data[j]) {
            buffer[k++] = data[i++];
        } else {
            sum += mid - i;
            buffer[k++] = data[j++];
        }
    }
    while (i < mid) buffer[k++] = data[i++];
    while (j < right) buffer[k++] = data[j++];
    for (int t = left; t < right; t++) data[t] = buffer[t];
}

void solve() {
    int N;
    scanf("%d", &N);
    int data[N];
    for (int i = 0; i < N; i++) {
        scanf("%d", &data[i]);
    }
    int buffer[N];
    sum = 0;
    merge(data, buffer, 0, N);
    printf("%d\n", sum);
}

int main() {
    int m;
    scanf("%d", &m);
    while (m--) solve();
}
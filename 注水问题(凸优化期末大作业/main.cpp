/**
 * @file       		凸优化注水问题解答(期末作业)
 * @author     		madderscientist
 * @version    		1.0
 * @Software 		DEV (https://royqh1979.gitee.io/redpandacpp/)
 * @date       		2022-12-3
 * @description		先从用户处获得向量a, 再求解得水位高, 最后可视化结果
 * @others			使用EGE图形库 (http://misakamm.github.com/xege); GCC编译器
 */
#include <graphics.h>		// EGE图形库两个
#include <stdio.h>
#include <bits/stdc++.h>	// GCC万能头
#define w 45				// 可视化 单位宽度
using namespace std;

/**
 * @brief 打印向量, 输出为 (a, b, ...)\n
 *
 * @param A 待打印的向量
 */
void printVector(vector<float>& A) {
	cout << "( ";
	for (unsigned int i = 0; i < A.size(); i++) {
		cout << A[i];
		if (i == A.size() - 1) cout << " )\n";
		else cout << ", ";
	}
}

/**
 * @brief 获取水位高度
 *
 * @param A 复制构造传值，防止修改原数据
 *
 * @return 水位高度
 */
float getWaterHeight(vector<float> a) {
	// 思路：排序后找区间，区间内水量变化和高度增量成正比
	sort(a.begin(), a.end());
	a.push_back(a.back() + 1);	//保证最大容量大于1

	float* c = new float(a.size() + 1);
	c[0] = 0;
	unsigned int k;
	for (k = 1; k < a.size(); k++) {
		c[k] = (a[k] - a[k - 1]) * k + c[k - 1];
		if (c[k] >= 1) break;
	}
	float height = a[k - 1] + (1 - c[k - 1]) / k;	// 水位高度
	delete[]c;
	return height;
}

/**
 * @brief 使用ege.h库可视化
 *
 * @param a 向量a
 * @param height 水位高度
 */
void visualization(vector<float>& a, float height) {
	int L = w * a.size();
	if(L<100) L=100;
	initgraph(L, L + 20);	// 高度的20像素为xi的输出位置
	// 为了画图美观：找到最高的一条
	float highest = height;
	for (unsigned int i = 0; i < a.size(); i++) {
		if (a[i] > highest) highest = a[i];
	}
	// 画水位
	setfillcolor(GRAY);
	fillrect(0, L * (1 - height / highest), L, L);
	// 画柱子
	setfillcolor(WHITE);
	for (unsigned int i = 0; i < a.size(); i++)
		fillrect(i * w, L * (1 - a[i] / highest), (i + 1)*w, L);
	// 标ai的值 柱子顶端
	setcolor(BLUE);
	setbkmode(TRANSPARENT);
	for (unsigned int i = 0; i < a.size(); i++) {
		int h = L * (1 - a[i] / highest);
		if (h > L - 18) h = L - 18;
		xyprintf(i * w + 2, h, "%.3f", a[i]);
	}
	// 标xi的值 标在最底下
	setcolor(WHITE);
	for (unsigned int i = 0; i < a.size(); i++) {
		float xi = height - a[i];
		xyprintf(i * w, L, "%.3f", xi < 0 ? 0 : xi);
	}
	// 标水位 左上角
	setcolor(GREEN);
	xyprintf(0, 0, "water height: %f", height);
	cin >> highest;
	closegraph();
}

/**
 * @brief 数据录入 空格分割 回车结束
 *
 * @param A Vector容器
 */
void cinVector(vector<float>& A) {
	cout << "Input vector a, separated by spaces and ended by an enter:\n";
	float ai;
	while (cin >> ai) {
		if (ai >= 0) A.push_back(ai);
		if (cin.get() == '\n') break;
	}
}

/**
 * @brief 随机生成数据
 *
 * @param A Vector容器
 */
void randomData(vector<float>& A, float max = 1, int d=13) {
	srand(time(NULL));
	for (int i = 0; i < d; i++) {
		float x = (float)(rand()%6666) / 6666 * max;
		A.push_back(x);
	}
}

int main(int argc, char *argv[]) {
	vector<float> a;
	// 可以选 随机生成 or 手动输入		$main.exe 1:手动输入
	if(argc > 1){
		if(argv[1]) cinVector(a);
	}
	else randomData(a);

	cout << "This is the vector a:\n";
	printVector(a);

	float height = getWaterHeight(a);
	cout << "\nwater height:" << height <<endl;

	// 在控制台输出向量x
	float sum = 0;
	cout << "| index\t|   a\t|   x\t|\n";
	for (unsigned int i = 0; i < a.size(); i++) {
		float xi = height - a[i];
		xi = xi < 0 ? 0 : xi;
		printf("|   %d\t|%.4f\t|%.4f\t|\n",i,a[i],xi);
		sum-=log(a[i]+xi);
	}
	cout<<"\nminimize value p*: " << sum << endl;

	visualization(a, height);
	return 0;
}

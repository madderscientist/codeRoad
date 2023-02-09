#define _CRT_SECURE_NO_WARNINGS
#include<graphics.h>
#include<stdio.h>
#include<conio.h>
#include<ctime>
#include<cstdlib>
#include<cmath>
#include<fstream>
using namespace std;
#define C 10
#define L 19
#define SIZE 30
int DaTa[240] = {
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,8,8,8,8,8,8,8,8,8,8,9,
	9,9,9,9,9,9,9,9,9,9,9,9
};
int position[7][4][4] = {
	0,-12,12,13,
	0,-1,1,-11,
	0,-12,-13,12,
	0,-1,1,11,

	0,-12,12,11,
	0,-1,1,13,
	0,-12,-11,12,
	0,-1,1,-13,

	0,-1,12,13,
	0,1,-11,12,
	0,1,-12,-13,
	0,-12,-1,11,

	0,1,12,11,
	0,-12,1,13,
	0,-1,-11,-12,
	0,-1,-13,12,

	0,-1,1,12,
	0,-12,12,1,
	0,-1,1,-12,
	0,-12,12,-1,

	0,1,12,13,
	0,1,12,13,
	0,1,12,13,
	0,1,12,13,

	0,-1,1,2,
	0,-12,-24,12,
	0,-1,-2,1,
	0,-12,12,24
};
COLORREF yanse[6] = { RED,GREEN,BLUE,YELLOW,WHITE,BROWN };
int kind,dire;		//标记当前形状
int center = 6;		//标记当前中心点位置
int currentcolour = 0;
int scores = 0;
bool gameover = false;
bool pausesignal = false;

int COL(int x) { return x % 12; }
int ROW(int x) { return x / 12; }

void drawtra(int weizhi,int color) {		//画一个方形
	setfillcolor(yanse[color]);
	int col = (COL(weizhi) - 1) * SIZE;
	int row = ROW(weizhi) * SIZE;
	fillrectangle(col+1, row+1, col + SIZE-1, row + SIZE-1);	//格子中间留空隙4像素空间
}

bool ifmove(int x,int y,int cent){				//判断是否能变位置（变形状,传入变换后的
	for (int i = 0; i < 4; i++) {
		int dian = position[x][y][i] + cent;
		if (dian > 0 && DaTa[dian]!=8) { return false; }	//超上界没关系
	}
	return true;
}

void DRAW() {		//先画底下存的，再画当前的
	cleardevice();
	int i = 1;
	for (; i < 227; i++) {
		if (DaTa[i] != 9 && DaTa[i] != 8) drawtra(i, DaTa[i]);
	}
	int x;
	for(i=0;i<4;i++){
		x = position[kind][dire][i] + center;
		if (x > 0) drawtra(x, currentcolour);
	}
}

void creatnew() {
	center = 6;
	kind = rand() % 7;
	dire=rand() % 4;
	currentcolour = rand() % 6;
}
void score() {
	int linenum = 0;		//	可以消去的行数
	int i = 0;
	bool ifempty;
	for (int k = 18; k >= 0; k--) {
		ifempty = true;
		for (i = 1 + 12 * k; i < 11 + 12 * k; i++) {
			if (DaTa[i] == 8) {
				ifempty = false;
				break;
			}
		}
		if (ifempty) {
			linenum++;
		}
		else if (linenum != 0) {
			for (i = 1 + 12 * k; i < 11 + 12 * k; i++) DaTa[i + linenum*12] = DaTa[i];
		}
	}
	if (linenum != 0) {
		i = 1;
		for (int k = 0; k < linenum; k++) {
			for (; i < k * 12 + 11; i++)
				DaTa[i] = 8;
			i = i + 2;
		}
		scores = scores + pow(2,linenum);
	}
}
void keydown() {
	while (1) {
		char down = _getch();
		bool ifchange = false;
		switch (down) {
		case 'A':case 'a': {
			if (ifmove(kind, dire, center - 1)) { center--; ifchange = true; }
			break;
		}
		case 'D':case 'd': {
			if (ifmove(kind, dire, center + 1)) { center++; ifchange = true; }
			break;
		}
		case 'S':case 's': {
			for (int i = 1;; i++) {
				if (ifmove(kind, dire, center + 12)) {
					center = center + 12;
					ifchange = true;
				}
				else break;
			}
			break;
		}
		case 'Q':case 'q': {
			if (ifmove(kind, (dire + 1) % 4, center)) { dire = (dire + 1) % 4; ifchange = true; }
			break;
		}
		case 'E':case 'e': {
			if (ifmove(kind, (dire + 5) % 4, center)) { dire = (dire + 5) % 4; ifchange = true; }
			break;
		}
		case 'p':case 'P': {
			pausesignal =!pausesignal;
			break;
		}
		}
		if (ifchange) { DRAW(); }
	}
}

int main() {
	srand(time(NULL));
	initgraph(C*SIZE, L*SIZE);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)keydown, NULL, NULL, NULL);
	//判断能不能动，能动就改center或者p，然后画全部
	creatnew();
	while (1) {
		DRAW();
		Sleep(400);
		while (pausesignal) {
			Sleep(3000);
		}
		if (ifmove(kind, dire, center + 12)) {
			center += 12;
		}
		else {		//数据存入，并消去一行试试。
			for (int i = 0; i < 4; i++) {
				int temp = center + position[kind][dire][i];
				if (temp >= 0)
				DaTa[temp] = currentcolour;
			}
			score();
			creatnew();
			if (!ifmove(kind, dire, center)) break;
		}
	}

	closegraph();
	time_t timer;
	timer = time(NULL);
	ofstream my;
	my.open("scores.txt", ios::app);
	my << ctime(&timer) <<'\t' << scores << "  scores\n";
	my.close();
	return 0;
}
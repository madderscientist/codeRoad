#define _CRT_SECURE_NO_WARNINGS
#include<graphics.h>
#include<iostream>
#include<stdio.h>
#include<conio.h>
#include<ctime>
#include<cstdlib>
#include<fstream>
#include "linklist.h"
using namespace std;
short L = 20;
short C = 20;
linklist<short> snack(1, (short)(L + 1)* C / 2);
COLORREF yanse[] = { RED,WHITE,GREEN };
short SIZEE = 16;
short EXTRATIME=200;
short MINTIME = 40;
short FoodLocation;								//��Χ��[1,L*C]
short direction = -1;
short scores = 0;
short mode;
bool controlable = true;


short COL(short x) { return (x % C + C) % C; }				//λ�ã���1��ʼ
short ROW(short x) { return x / C; }				//��0��ʼ


void drawtra(short weizhi, short color = 1) {		//��һ������
	if (color > 0) color = 1;
	else if (color == -1) color = 2;
	setfillcolor(yanse[color]);
	short col = COL(weizhi) * SIZEE;		//���㿪ʼ
	short row = (ROW(weizhi)) * SIZEE;
	fillrectangle(col + 1, row + 1, col + SIZEE - 1, row + SIZEE - 1);
}

bool ifover() {		//ѹ��
	node<short>* p = snack.head;
	for (short i = 1; i <= snack.length; i++) {
		node<short>* Q = p->next;
		bool youwu = false;
		for (short J = i + 1; J <= snack.length; J++) {
			if (Q->data == p->data) { youwu = true; break; }
			Q = Q->next;
		}
		if (youwu) { return true; }
		p = p->next;
	}
	return false;
}


void creatnew() {
	short newlocation = 0;
	do {
		newlocation = rand() % (L * C) + 1;
	} while (snack.indexof(newlocation) != -1);
	FoodLocation = newlocation;
}
void DRAW() {
	cleardevice();
	drawtra(FoodLocation, -1);
	node<short>* p = snack.head;
	for (short i = 0; i < snack.length; i++) {
		drawtra(p->data, i);
		p = p->next;
	}
}

bool nextstep() {
	short current = snack.visit(1) + direction;
	//�ж��Ƿ�ײǽ
	short row1 = ROW(current) + 1;
	short row2 = ROW(snack.visit(1)) + 1;
	short col1 = COL(current);
	short col2 = COL(snack.visit(1));
	if (row1 != row2) {
		if (mode != 1 && ((row1 <= 0 || row1 > L) || col1 != col2)) { return true; }
		if (col1 == col2) {
			current = (current + L * C) % (L * C);
		}
		else {
			if (row1 < row2) current = current + C;
			else current = current - C;
		}
	}
	if (current != FoodLocation) {
		snack.remove(snack.length);
	}
	else {
		scores++;
		creatnew();
	}
	snack.insert(1, current);
	if (mode == 3) return false;
	else return ifover();
}
char down;
void keydown() {
	while (1) {
		if (controlable) {
			down = _getch();
			switch (down) {
			case 'A':case 'a': {
				if (direction != 1) direction = -1;
				controlable = false;
				break;
			}
			case 'D':case 'd': {
				if (direction != -1) direction = 1;
				controlable = false;
				break;
			}
			case 'S':case 's': {
				if (direction != -C) direction = C;
				controlable = false;
				break;
			}
			case 'W':case 'w': {
				if (direction != C) direction = -C;
				controlable = false;
				break;
			}
			}
		}
	}
}

short main() {
	srand(time(NULL));
	ifstream config("config.txt", ios::in);
	if (!config) cout <<"����ϷͬһĿ¼��ӡ�config.txt���ı��ļ�����������Ϸ����������һ�����ֻ�һ�С�\n����˳��:\n��(Ĭ��20)\n��(Ĭ��20)\n��λ����(���أ�Ĭ��16)\n����ʱ����(���룬Ĭ��200)\n���ʱ����(���룬Ĭ��40)\n�����У�����ʱ������Ϸ���ж����٣����ʱ��=���ʱ��+����ʱ�䡿\n\n";
	else {
		config >> L >> C >> SIZEE >> EXTRATIME >> MINTIME;
		cout << "���ö�ȡ�ɹ�����"<<C<<"�񣬸�"<<L<<"��ÿ���"<<SIZEE<<"���أ�����ʱ��"<<EXTRATIME<<"���룬���ʱ��"<<MINTIME<<"����\n\n";
	}
	cout << "��Ϸģʽ�������ţ�\n1.�޽�\t2.�н�\t3.�н��ѹ��\n";
	cin >> mode;
	initgraph(C * SIZEE, L * SIZEE);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)keydown, NULL, NULL, NULL);
	creatnew();
	do {
		DRAW();
		controlable = true;
		Sleep(MINTIME + EXTRATIME / (scores+2-sqrt(scores+2)));
	} while (!nextstep());
	Sleep(1000);
	closegraph();
	time_t timer;
	timer = time(NULL);
	ofstream my;
	my.open("snack scores.txt", ios::app);
	my << ctime(&timer) << "MODE " << mode << '\t' << scores << "  scores\n";
	my.close();
	return 0;
}
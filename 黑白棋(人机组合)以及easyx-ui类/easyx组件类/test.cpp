#define _CRT_SECURE_NO_WARNINGS
#include<graphics.h>
#include<conio.h>
#include"buttonlist.h"
#include<iostream>
using namespace std;
linklist bl;
MOUSEMSG m;
bool whetherstart = false;	//Ϊ�˼�������ϵİ�ť����
//ѡ����Ϣ
void AchoseB(); char Wwho = 0;
void AchoseA(); char Bwho = 0;
//�����Ѷ� | �����Ѷ�
void f11(); void f21();
void f12(); void f22();
void f13(); void f23();
void f14(); void f24();
void ok();		//��ʼ��Ϸ
void regret();	//����
void helpme();	//����
void outmessage(char id, bool ifclear = true);

void click() {
	while (1) {
		m = GetMouseMsg();
		switch (m.uMsg) {
		case WM_LBUTTONDOWN:
			if (m.x <= 277 || m.x >= 757 || m.y <= 10 || m.y >= 490 || !whetherstart) bl.findevent(m.x, m.y);
			break;
		case WM_RBUTTONDOWN:
			break;
		}
	}
	FlushMouseMsgBuffer();
}
void outmessage(char id, bool ifclear) {
	if (ifclear) { putimage(808, 40, 176, 98, &bgpic, 808, 40); }
	settextcolor(BLACK);
	setbkmode(0);
	switch (id) {
	case 1:
		outtextxy(808, 65, "�ֵ�");
		break;
	case 0:
		outtextxy(808, 50, "˫����û��λ����");
		outtextxy(816, 75, "��Ϸ������");
		break;
	case -1:
		outtextxy(808, 40, "���ֿ���,��Ȩ�黹");
		outmessage(1, false);
		break;
	case 2:			//����
		outtextxy(808, 65, "С������Ҫ�Ұ����ǰ�");
		break;
	case 3:			//����
		outtextxy(808, 50, "�������޻ڡ�֪����");
		outtextxy(814, 75, "�����㲻�Ǿ���");
		break;
	case 4:			//λ�ò���
		outtextxy(808, 65, "�׳գ����ﲻ����");
		break;
	}
}
void AchoseB() {
	button* temp = bl.visit(1);
	if (Wwho != 0) {
		temp->resetstyle("���", NULL, NULL, NULL);
		Wwho = 0;
		bl.findandremove(11); cout << "1";
		bl.findandremove(12); cout << "2";
		bl.findandremove(13); cout << 3;
		bl.findandremove(14); cout << 4;
	}
	else {
		temp->resetstyle("����", NULL, NULL, NULL);
		Wwho = 1;
		bl.creatnew(11, 107, 290, 70, 35, f11, "�Ѷ�2", NULL, WHITE, BLACK);
		bl.creatnew(12, 107, 340, 70, 35, f12, "�Ѷ�4", NULL, WHITE, BLACK);
		bl.creatnew(13, 107, 390, 70, 35, f13, "�Ѷ�6", NULL, WHITE, BLACK);
		bl.creatnew(14, 107, 440, 70, 35, f14, "�Ѷ�8", NULL, WHITE, BLACK);
	}
}
void AchoseA() {
	button* temp = bl.visit(2);
	if (Bwho != 0) {
		temp->resetstyle("���", NULL, NULL, NULL);
		Bwho = 0;
		bl.findandremove(21);
		bl.findandremove(22);
		bl.findandremove(23);
		bl.findandremove(24);
	}
	else {
		temp->resetstyle("����", NULL, NULL, NULL);
		Bwho = 1;
		bl.creatnew(21, 858, 15, 70, 35, f21, "�Ѷ�2", NULL, BLACK, WHITE);
		bl.creatnew(22, 858, 65, 70, 35, f22, "�Ѷ�4", NULL, BLACK, WHITE);
		bl.creatnew(23, 858, 115, 70, 35, f23, "�Ѷ�6", NULL, BLACK, WHITE);
		bl.creatnew(24, 858, 165, 70, 35, f24, "�Ѷ�8", NULL, BLACK, WHITE);
	}
}
void f11() {
	settextcolor(BLACK);
	setbkcolor(WHITE);
	setbkmode(OPAQUE);
	Wwho = 2;
	outtextxy(68, 121, "2");
}
void f12() {
	settextcolor(BLACK);
	setbkcolor(WHITE);
	setbkmode(OPAQUE);
	Wwho = 4;
	outtextxy(68, 121, "4");
}
void f13() {
	settextcolor(BLACK);
	setbkcolor(WHITE);
	setbkmode(OPAQUE);
	Wwho = 6;
	outtextxy(68, 121, "6");
}
void f14() {
	settextcolor(BLACK);
	setbkcolor(WHITE);
	setbkmode(OPAQUE);
	Wwho = 8;
	outtextxy(68, 121, "8");
}
void f21() {
	settextcolor(WHITE);
	setbkcolor(BLACK);
	setbkmode(OPAQUE);
	Bwho = 2;
	outtextxy(832, 307, "2");
}
void f22() {
	settextcolor(WHITE);
	setbkcolor(BLACK);
	setbkmode(OPAQUE);
	Bwho = 4;
	outtextxy(832, 307, "4");
}
void f23() {
	settextcolor(WHITE);
	setbkcolor(BLACK);
	setbkmode(OPAQUE);
	Bwho = 6;
	outtextxy(832, 307, "6");
}
void f24() {
	settextcolor(WHITE);
	setbkcolor(BLACK);
	setbkmode(OPAQUE);
	Bwho = 8;
	outtextxy(832, 307, "8");
}
void ok() {
	bl.findandremove(1);
	bl.findandremove(2);
	if (Bwho != 0) {
		bl.findandremove(21);
		bl.findandremove(22);
		bl.findandremove(23);
		bl.findandremove(24);
	}
	if (Wwho != 0) {
		bl.findandremove(11);
		bl.findandremove(12);
		bl.findandremove(13);
		bl.findandremove(14);
	}
	whetherstart = true;
	bl.findandremove(3);
	bl.creatnew(4, 70, 310, 140, 60, regret, "����", NULL, RED, GREEN);
	bl.creatnew(5, 70, 400, 140, 60, helpme, "����", NULL, RED, GREEN);
	putimage(277, 10, 480, 480, &bgpic, 277, 10);
}
void regret() {
	outmessage(3);
}
void helpme() {
	outmessage(2);
}
void ini() {
	//��ʼ������
	initgraph(1032,500,SHOWCONSOLE);
	IMAGE tit, gai;
	loadimage(&bgpic, "BD.png",1032,500);
	//��logo
	loadimage(&tit, "title.png");
	loadimage(&gai, "gai.png");
	putimage(0, 0, &bgpic);
	putimage(295, 11, &tit, SRCAND);
	putimage(295, 11, &gai, SRCPAINT);
	//������ť
	bl.creatnew(1, 68, 121, 136, 74, AchoseB, "���", NULL, WHITE, BLACK);
	bl.creatnew(2, 832, 307, 136, 74, AchoseA, "���", NULL, BLACK, WHITE);
	bl.creatnew(3, 448, 370, 136, 74, ok, "��ʼ", "orangebutton.png", YELLOW, RED);
}
int main() {
	ini();
	click();
	_getch();
	closegraph();
	return 0;
}
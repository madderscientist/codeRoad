#define _CRT_SECURE_NO_WARNINGS
#include<graphics.h>
#include<conio.h>
#include"buttonlist.h"
#include<iostream>
#include<stdio.h>
#include<fstream>
#include<cmath>
#include "Board.h"
#include <ctime>
#include <string>
#include"resource.h"
#define XX 277		//棋盘位置
#define YY 10
using namespace std;
extern float** qSqu;
class QKS {
public:
	mutable float** qs;
	float qList[10] = { 36,6,6,6,1,1,1,1 };
	char score;

	QKS() { qs = nullptr, score = 0; }
	~QKS() {
		for (int i = 0; i < 8; i++) delete[] qs[i];
		delete[] qs;
		qs = nullptr;
		score = 0;
	}

	void Use() const {
		if (qs != nullptr) {
			qSqu = qs;
			return;
		}
		static const char SQK[8][8] = {
			{0,1,2,3,3,2,1,0},
			{1,4,5,6,6,5,4,1},
			{2,5,7,8,8,7,5,2},
			{3,6,8,9,9,8,6,3},
			{3,6,8,9,9,8,6,3},
			{2,5,7,8,8,7,5,2},
			{1,4,5,6,6,5,4,1},
			{0,1,2,3,3,2,1,0}
		};
		qs = new float* [8];
		for (int i = 0; i < 8; i++) qs[i] = new float[8];
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				qs[i][j] = qList[SQK[i][j]];
		qSqu = qs;
	}
};
int endx = XX + SIZE * 8, endy = YY + SIZE * 8;		//棋盘的右下角坐标
short Ws = 0, Bs = 0;						//黑 白分数
linklist bl;
MOUSEMSG m;
ofstream fout;
BD *game;
BD *regretgame;								//悔棋专用
QKS WV0;
bool whetherstart = false;				//为了检测棋盘上的按钮而设
bool ifcontinue = true;					//为了结束游戏
bool iftip = false;								//是否显示提示
bool havehelp = false;						//计数是否帮助，用于嘲讽语言输出
char xy;											//位置信息
/*----------其他函数-----------*/
void f1(char);
void f2(char);
void player();
void computer(int dif);
void ComputerAfterPlayer();
void getscore();
void outmessage(char id, bool ifclear = true, bool ifclear2 = true);
/*--------流程控制函数--------*/
char turnjudge();
void ini();
void click();
void createlog();
void ending();
/*--------按钮动作函数-------*/
//选手信息
void AchoseB(); char Wwho = 0;		//记录白子模式
void AchoseA(); char Bwho = 0;		//记录黑子模式
//白棋难度 | 黑旗难度
void f11(); void f21();
void f12(); void f22();
void f13(); void f23();
void f14(); void f24();
void ok();			//开始游戏
void regret();	//悔棋
void helpme();	//求助

int main() {
	ini();
	click();
	ending();
	xy = _getch();
	closegraph();
	return 0;
}

inline char turnjudge() {
	char zhuangtai = game->Refresh();
	outmessage(zhuangtai);
	if (zhuangtai == 0) ifcontinue = false;
	return zhuangtai;
}
void player() {
	if (game->Putable(xy)) {
		game->drawone(xy, XX, YY);
		delete regretgame;
		regretgame = new BD();
		*regretgame = *game;
		game->PutAt(xy);
		getscore();


		havehelp = false;

		if (if_log) {
			fout << endl << "###Put at " << xy / 8 << " " << xy % 8 << endl;
			game->output(fout);
		}

		xy = game->Refresh();					//状态判断。xy多用！是传输状态的纽带
		outmessage(xy);

		Sleep(500);					//玩家下棋只会在click()中用，所以把画图也放进来
		if (iftip) putimage(XX, YY, SIZE * 8, SIZE * 8, &bgpic, XX, YY);
		game->draw(XX, YY);
	}
	else {
		outmessage(4);			//下得不对，输出嘲讽
		xy = 1;
	}
}
void computer(int dif) {
	outmessage(6, true, false);
	difficulty = dif;
	xy = game->autoPut();
	game->drawone(game->ns[xy], XX, YY);
	game->Put(game->ns[xy], game->dir[xy]);
	getscore();
	if (if_log) {
		fout << endl << "###Put at " << xy / 8 << " " << xy % 8 << endl;
		game->output(fout);
	}
	xy = turnjudge();
}
void	ComputerAfterPlayer() {
	if (xy == 1 && (game->tts > 0 && Wwho != 0 || game->tts < 0 && Bwho != 0))
	{
		do {
			Sleep(600);
			computer(game->tts > 0 ? Wwho : Bwho);
			Sleep(500);
			if (iftip) putimage(XX, YY, SIZE * 8, SIZE * 8, &bgpic, XX, YY);
			game->draw(XX, YY);
		} while (xy == -1);
	}
}
void click() {
	while (ifcontinue) {
		m = GetMouseMsg();
		switch (m.uMsg) {

		case WM_LBUTTONDOWN:			//左击判断是按钮事件，还是下棋
			if (m.x <= XX || m.x >= endx || m.y <= YY || m.y >= endy || !whetherstart) bl.findevent(m.x, m.y);
			else {
				if (Wwho != 0 && Bwho != 0) break;			//修bug。防止互撕时人可以操作（不知道为什么成了双线程，按理说互撕时cilck()不被运行啊？
				xy = (m.x - XX) / SIZE + (m.y - YY) / SIZE * 8;
				player();
				if (xy == 0) {
					ifcontinue = false;
				}
				else ComputerAfterPlayer();
			}
			break;


		case WM_RBUTTONDOWN:			//右击显示可下位置
			if (whetherstart) {
				if (iftip) {									//已经有提示则清除提示，否则提示
					iftip = false;
					putimage(XX, YY, SIZE * 8, SIZE * 8, &bgpic, XX, YY);
					game->draw(XX, YY);
				}
				else {
					iftip = true;
					game->ShowWhereCan(XX, YY);
				}
			}
			break;
		}
		FlushMouseMsgBuffer();
	}
}
void outmessage(char id, bool ifclear1, bool ifclear2) {
	if (ifclear1) putimage(808, 65, 176, 25, &bgpic, 808, 65);
	if (ifclear2) putimage(808, 90, 176, 25, &bgpic, 808, 90);
	settextcolor(BLACK);
	setbkmode(0);
	switch (id) {
	case 1:
		putimage(808, 40, 175, 24, &bgpic, 808, 40);
		if (game->tts < 0)	outtextxy(808, 40, "轮到黑棋");
		else outtextxy(808, 40, "轮到白棋");
		break;
	case 0:
		outtextxy(808, 65, "双方都没有位置下");
		outtextxy(816, 90, "游戏结束！");
		break;
	case -1:
		outtextxy(808, 65, "无字可下,棋权归还");
		outmessage(1, false, false);
		break;
	case 2:			//帮我
		outtextxy(808, 90, "小辣鸡，要我帮了是吧");
		break;
	case 3: 			//悔棋
		outtextxy(808, 65, "“落子无悔”知道吗？");
		outtextxy(814, 90, "还好你不是君子");
		outmessage(1, false, false);
		break;
	case 4:			//位置不对
		outtextxy(808, 65, "白痴，那里不能下");
		break;
	case 5:			//输赢
	{
		LOGFONT f;
		gettextstyle(&f);
		f.lfHeight = 200;
		f.lfWidth = 110;
		_tcscpy(f.lfFaceName, _T("隶书"));
		settextstyle(&f);
		settextcolor(RED);
		RECT a = { 0,0,1036,460 };
		if (Ws == Bs) drawtext("平局", &a, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		else drawtext((Ws > Bs) ? "白方获胜" : "黑方获胜", &a, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		break;
	}
	case 6:						//显示电脑在运算
		outtextxy(808, 65, "计算中……");
		break;
	case 7:						//多次悔棋的嘲讽
		outtextxy(808, 65, "还想悔？");
		outtextxy(808, 90, "给你悔一次就不错了！");
		break;
	case 8:						//白方身份输出
		switch (Wwho) {
		case 0:
			outtextxy(124, 146, "玩家");
			break;
		default:
			outtextxy(118, 146, "电脑");
			char diff[2];
			sprintf(diff, "%d", Wwho);
			outtextxy(155, 146, diff);
			break;
		}
		break;
	case 9:						//黑方身份输出
		settextcolor(WHITE);
		switch (Bwho) {
		case 0:
			outtextxy(884, 335, "玩家");
			break;
		default:
			outtextxy(876, 335, "电脑");
			char diff[2];
			sprintf(diff, "%d", Bwho);
			outtextxy(913, 335, diff);
			break;
		}
		break;
	}
}
void AchoseB() {
	button* temp = bl.visit(1);
	if (Wwho != 0) {
		temp->resetstyle("玩家");
		Wwho = 0;
		bl.findandremove(11);
		bl.findandremove(12);
		bl.findandremove(13);
		bl.findandremove(14);
	}
	else {
		temp->resetstyle("电脑");
		Wwho = 2; f1(2);
		bl.creatnew(11, 107, 290, 70, 35, f11, "难度2", NULL, WHITE, BLACK);
		bl.creatnew(12, 107, 340, 70, 35, f12, "难度4", NULL, WHITE, BLACK);
		bl.creatnew(13, 107, 390, 70, 35, f13, "难度6", NULL, WHITE, BLACK);
		bl.creatnew(14, 107, 440, 70, 35, f14, "难度8", NULL, WHITE, BLACK);
	}
}
void AchoseA() {
	button* temp = bl.visit(2);
	if (Bwho != 0) {
		temp->resetstyle("玩家", NULL, NULL, NULL);
		Bwho = 0;
		bl.findandremove(21);
		bl.findandremove(22);
		bl.findandremove(23);
		bl.findandremove(24);
	}
	else {
		temp->resetstyle("电脑", NULL, NULL, NULL);
		Bwho = 2; f2(2);
		bl.creatnew(21, 858, 15, 70, 35, f21, "难度2", NULL, BLACK, WHITE);
		bl.creatnew(22, 858, 65, 70, 35, f22, "难度4", NULL, BLACK, WHITE);
		bl.creatnew(23, 858, 115, 70, 35, f23, "难度6", NULL, BLACK, WHITE);
		bl.creatnew(24, 858, 165, 70, 35, f24, "难度8", NULL, BLACK, WHITE);
	}
}
inline void f1(char x) {
	settextcolor(BLACK);
	setbkcolor(WHITE);
	setbkmode(OPAQUE);
	Wwho = x;
	outtextxy(68, 121, (char)(x + 48));
}
inline void f2(char x) {
	settextcolor(WHITE);
	setbkcolor(BLACK);
	setbkmode(OPAQUE);
	Bwho = x;
	outtextxy(832, 307, (char)(x + 48));
}
void f11() {
	f1(2);
}
void f12() {
	f1(4);
}
void f13() {
	f1(6);
}
void f14() {
	f1(8);
}
void f21() {
	f2(2);
}
void f22() {
	f2(4);
}
void f23() {
	f2(6);
}
void f24() {
	f2(8);
}
void ok() {
	game = new BD;
	regretgame = new BD;
	//按钮管理
	bl.findandremove(1);
	bl.findandremove(2);
	bl.findandremove(10);
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
	if (Bwho == 0 || Wwho == 0) {
		bl.creatnew(4, 70, 310, 140, 60, regret, "悔棋", NULL, RED, GREEN);
		bl.creatnew(5, 70, 400, 140, 60, helpme, "帮我", NULL, RED, GREEN);
	}
	//画面处理
	putimage(XX, YY, SIZE * 8, SIZE * 8, &bgpic, XX, YY);
	game->draw(XX, YY);
	//输出对战双方身份
	outmessage(9, false, false);
	outmessage(8, false, false);

	if (if_log) createlog();
	//游戏开始
	if (Bwho != 0) {						//如果黑棋是电脑就先下
		computer(Bwho);
		Sleep(500);
		game->draw(XX, YY);
		if (Wwho != 0) {				//如果白棋也是电脑就开撕
			while (ifcontinue) {
				computer(game->tts > 0 ? Wwho : Bwho);
				Sleep(500);
				if (iftip) putimage(XX, YY, SIZE * 8, SIZE * 8, &bgpic, XX, YY);
				game->draw(XX, YY);
				Sleep(900);
			}
		}
	}
	else {
		outmessage(1);
		getscore();
	}
}
void regret() {
	if (havehelp) outmessage(7);
	else {
		putimage(XX, YY, SIZE * 8, SIZE * 8, &bgpic, XX, YY);
		regretgame->draw(XX, YY);
		delete game;
		game = new BD;
		*game = *regretgame;
		outmessage(3);
		getscore();
		havehelp = true;
	}
}
void helpme() {
	delete regretgame;
	regretgame = new BD;
	*regretgame = *game;
	outmessage(2);
	computer(6);
	Sleep(500);
	if (iftip) putimage(XX, YY, SIZE * 8, SIZE * 8, &bgpic, XX, YY);
	game->draw(XX, YY);
	havehelp = false;
	ComputerAfterPlayer();
}

void ini() {
	//初始化窗口
	initgraph(1032, 500);
	IMAGE tit, gai;
	loadimage(&bgpic, "IMAGE", "background", 1032, 500);
	//贴logo
	loadimage(&tit, "IMAGE", "title");
	loadimage(&gai, "IMAGE", "gai");
	putimage(0, 0, &bgpic);
	putimage(295, 11, &tit, SRCAND);
	putimage(295, 11, &gai, SRCPAINT);
	//创建按钮
	bl.creatnew(1, 68, 121, 136, 74, AchoseB, "玩家", NULL, WHITE, BLACK);
	bl.creatnew(2, 832, 307, 136, 74, AchoseA, "玩家", NULL, BLACK, WHITE);
	bl.creatnew(3, 448, 370, 136, 74, ok, "开始", NULL, YELLOW, RED);

	ifstream fin("WeightValue.dat");
	fin >> if_log;
	if (fin.is_open()) {
		for (int i = 0; i < 10; i++) fin >> WV0.qList[i];
	}
	WV0.Use();
}
void createlog() {
	time_t curtime;
	time(&curtime);
	fout.open(to_string(curtime) + ".log");
	if (!fout.is_open()) {
		cerr << "File opening error!" << endl;
		system("pause");
	}
}
void getscore() {
	char sco[3];
	Ws = game->WhoseScore(true);
	sprintf(sco, "%d", Ws);
	putimage(156, 15, 15, 15, &bgpic, 156, 15);
	outtextxy(156, 15, sco);
	Bs = game->WhoseScore(false);
	sprintf(sco, "%d", Bs);
	putimage(878, 473, 15, 15, &bgpic, 878, 473);
	outtextxy(878, 473, sco);
}
void ending() {
	short emp = (64 - abs(game->tts)) / 2;
	Ws += emp;
	Bs += emp;
	getscore();
	outmessage(5, false, false);
}

/*按钮对应关系
* ID					按钮
*
* 1					白方模式
* 11					白方电脑难度2
* 12					白方电脑难度4
* 13					白方电脑难度6
* 14					白方电脑难度8
*
* 2					黑方模式
* 21					黑方电脑难度2
* 22					黑方电脑难度4
* 23					黑方电脑难度6
* 24					黑方电脑难度8
*
* 3					开始
* 4					悔棋
* 5					帮忙
*/

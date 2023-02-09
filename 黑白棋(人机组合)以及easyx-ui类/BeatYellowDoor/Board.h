#ifndef BOARD_H_
#define BOARD_H_
#include <graphics.h>
#include <iostream>

#define SIZE 60				//一格的边长
#define Rpro 2.2			//一格的边长除以半径
#define nsSize 27			//可以走的位置数量的保守估计，后续调整

extern int difficulty;			//难度0-9
extern bool if_log;			//是否生成log文件
//using namespace std;

typedef unsigned long long ULL;
typedef unsigned char UC;


//八个方向
static const short dx[8] = { -1,-1,0,1,1,1,0,-1 };
static const short dy[8] = { 0,-1,-1,-1,0,1,1,1 };


class BD {
private:
	char Ps[8][8];
public:
	//以下在Refresh中更新
	mutable float h0;	//启发函数，计算方法：
					//角子权8，边子权2，其余子1，
					//若已经结束，启发函数=127（电脑赢），-127（玩家赢），0（平局）
					//else 启发函数=A棋子权和-B棋子权和（直观：电脑领先了多少）

	mutable char* ns;	//可以走的下一步，ns[0]是有效位数
	mutable UC* dir;	//对应的方向
	mutable char tts;	//总步数（+-64即结束，负值表示棋权属于B，正值A，一般在put里面更新，只有没有地方下才会在refresh里面更新

	BD() :h0(0), tts(-4),
		Ps{ {},{},{},
		{0,0,0,1,-1},
		{0,0,0,-1,1},
		{},{},{} }
	{	
		ns = new char[nsSize]();
		dir = new UC[nsSize]();
		Refresh();
	}

	~BD() { 
		delete[] ns;
		delete[] dir;
	}
	//构造org在m处放置后的棋盘（保证能放）
	BD(const BD& org, const char& i);
	BD(BD&& org)noexcept :h0(org.h0), tts(org.tts) {
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				Ps[i][j] = org.Ps[i][j];
		ns = org.ns; dir = org.dir;
		org.ns = nullptr; org.dir = nullptr;
	}
	BD(const BD& org) = delete;
	void operator=(BD& org);


	//放置
	void is(char& i, char& j) const;		//判断一个点是否可以
	bool Putable(const char& x);
	void PutAt(const char& m);
	void Put(const char& m, const char& D);

	void SpecialH0() const;
	void RefreshValues() const;		//刷新ns和dir
	char Refresh() const;				//刷新，判断是否有位置下，有则返回1，无则换一方，返回-1，双方都不行，返回0
	bool ifEnd() const {		//结束
		return (tts == 64 || tts == -64 || *ns == 0);
	}
	const char autoPut() const;//自动落子，返回ns和dir的索引i
	void draw(int X = 0, int Y = 0);		//画棋盘
	void drawone(char& x, int X = 0, int Y = 0);//画一个子，黑白根据tts正负判断，正则白，负则黑，用于动画演示，只对游戏体验有贡献
	void ShowWhereCan(int X = 0, int Y = 0);
	int WhoseScore(bool);		//返回一方的分数，真则白，假则黑

	void output(std::ostream& os = std::cout) const {
		if (if_log) {

			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (Ps[i][j] == 0) os << ' ';
					else if (Ps[i][j] > 0) os << 'O';
					else os << 'X';
					os << ' ';
				}
				os << std::endl;
			}
			os << "tts =" << (int)tts << std::endl;
			os << "h0  =" << h0 << std::endl;

		}
	}

};

#endif // !BOARD_H_

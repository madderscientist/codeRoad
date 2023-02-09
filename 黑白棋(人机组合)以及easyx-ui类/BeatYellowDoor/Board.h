#ifndef BOARD_H_
#define BOARD_H_
#include <graphics.h>
#include <iostream>

#define SIZE 60				//һ��ı߳�
#define Rpro 2.2			//һ��ı߳����԰뾶
#define nsSize 27			//�����ߵ�λ�������ı��ع��ƣ���������

extern int difficulty;			//�Ѷ�0-9
extern bool if_log;			//�Ƿ�����log�ļ�
//using namespace std;

typedef unsigned long long ULL;
typedef unsigned char UC;


//�˸�����
static const short dx[8] = { -1,-1,0,1,1,1,0,-1 };
static const short dy[8] = { 0,-1,-1,-1,0,1,1,1 };


class BD {
private:
	char Ps[8][8];
public:
	//������Refresh�и���
	mutable float h0;	//�������������㷽����
					//����Ȩ8������Ȩ2��������1��
					//���Ѿ���������������=127������Ӯ����-127�����Ӯ����0��ƽ�֣�
					//else ��������=A����Ȩ��-B����Ȩ�ͣ�ֱ�ۣ����������˶��٣�

	mutable char* ns;	//�����ߵ���һ����ns[0]����Чλ��
	mutable UC* dir;	//��Ӧ�ķ���
	mutable char tts;	//�ܲ�����+-64����������ֵ��ʾ��Ȩ����B����ֵA��һ����put������£�ֻ��û�еط��²Ż���refresh�������

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
	//����org��m�����ú�����̣���֤�ܷţ�
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


	//����
	void is(char& i, char& j) const;		//�ж�һ�����Ƿ����
	bool Putable(const char& x);
	void PutAt(const char& m);
	void Put(const char& m, const char& D);

	void SpecialH0() const;
	void RefreshValues() const;		//ˢ��ns��dir
	char Refresh() const;				//ˢ�£��ж��Ƿ���λ���£����򷵻�1������һ��������-1��˫�������У�����0
	bool ifEnd() const {		//����
		return (tts == 64 || tts == -64 || *ns == 0);
	}
	const char autoPut() const;//�Զ����ӣ�����ns��dir������i
	void draw(int X = 0, int Y = 0);		//������
	void drawone(char& x, int X = 0, int Y = 0);//��һ���ӣ��ڰ׸���tts�����жϣ�����ף�����ڣ����ڶ�����ʾ��ֻ����Ϸ�����й���
	void ShowWhereCan(int X = 0, int Y = 0);
	int WhoseScore(bool);		//����һ���ķ���������ף������

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

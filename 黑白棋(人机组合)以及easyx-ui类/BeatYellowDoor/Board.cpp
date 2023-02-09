#include "Board.h"
#include <math.h>
int difficulty = 6;			//�Ѷ�0-9
bool if_log = false;
void BD::operator=(BD& ano) {
	for (char i = 0; i < 8; i++)
		for (char j = 0; j < 8; j++)
			Ps[i][j] = ano.Ps[i][j];
	for (char i = 0; i < nsSize; i++) {
		ns[i] = ano.ns[i];
		dir[i] = ano.dir[i];
	}
	tts = ano.tts;
	h0 = ano.h0;
}
void BD::is(char& x, char& y) const {
	if (Ps[y][x]) return;

	UC direction = 0;		//�ݴ淽��
	for (int k = 0; k < 8; k++) {		//�˸������
		short tempx = x + dx[k], tempy = y + dy[k];
		bool flag = false;
		while (1) {
			if (tempx < 0 || tempy < 0 || tempx > 7 || tempy > 7) {
				flag = false;
				break;
			}
			if ((short)Ps[tempy][tempx] * tts < 0) {			//���ݲ����������ж���ɫ
				tempx += dx[k];
				tempy += dy[k];
				flag = true;
			}
			else {
				break;
			}
		}
		if (flag && Ps[tempy][tempx]) {
			direction |= 1 << k;
		}
	}

	if (direction) {						//��
		dir[++ns[0]] = direction;
		ns[ns[0]] = x + y * 8;
	}
}
bool BD::Putable(const char& x) {
	for (int i = 1; i <= ns[0]; i++) if (ns[i] == x) return true;
	return false;
}

float** qSqu;

inline void BD::RefreshValues() const {
	h0 = 0;
	ns[0] = 0;
	for (char i = 0; i < 8; i++) {
		for (char j = 0; j < 8; j++) {
			h0 += Ps[i][j] * qSqu[i][j];				//����h0
			is(j, i);							//����ns��dir
		}
	}
}
void BD::SpecialH0() const {
	char pre = 0, i, j;
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
			pre += Ps[i][j];
	if (pre == 0) h0 = 0;
	else if (pre > 0) h0 = INFINITY;
	else h0 = -INFINITY;
}

char BD::Refresh() const {
	if (tts == 64 || tts == -64) {
		SpecialH0();
		ns[0] = 0;
		return 0;
	}

	RefreshValues();
	if (ns[0] > 0) return 1;

	tts *= -1;			//ûλ����
	RefreshValues();
	if (ns[0] > 0) return -1;
	else {				//����û��λ���£���Ϸ������
		SpecialH0();
		return 0;
	}
}

void BD::PutAt(const char& m) {
	int i = 1;
	for (; ns[i] != m; i++);
	Put(ns[i],dir[i]);
}

void BD::Put(const char& m, const char& D) {		//�ж��ܲ��ܷŵ��жϷ���main������
	//char m = ns[i], D = dir[i];
	short x = m % 8;
	short y = m / 8;
	//��Ȩ���
	/*
	char data = (tts > 0) ? 1 : -1;
	if (x == 0 || x == 7 || y == 0 || y == 7) {
		if (m == 0 || m == 7 || m == 56 || m == 63) data *= 8;		//����Ϊ8
		else data *= 2;																		//����Ϊ2
	}
	Ps[y][x] = data;*/

	Ps[y][x] = (tts > 0) ? 1 : -1;

	/*------��ת-----*/

		//1.�ҵ�λ��
		// abandoned
	//2.����ת
	for (char j = 0; j < 8; j++) {
		if (D & 1 << j) {					//����������
			int tempx = x + dx[j], tempy = y + dy[j];
			while ((short)Ps[tempy][tempx] * tts < 0) {
				Ps[tempy][tempx] *= -1;
				tempx += dx[j];
				tempy += dy[j];
			}
		}
	}
	tts = (tts < 0) ? 1 - tts : -1 - tts;
}


BD::BD(const BD& org, const char& i) :h0(org.h0), tts(org.tts) {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			Ps[i][j] = org.Ps[i][j];
	ns = new char[nsSize]();
	dir = new UC[nsSize]();
	Put(org.ns[i], org.dir[i]);
}


void BD::draw(int X,int Y) {		//X���������Ͻ�x��Y���������Ͻ�y��ʹ��ǰ��Ҫ����
	setcolor(BLACK);
	//������
	int scale = 8 * SIZE;
	rectangle(X, Y, X + scale, Y + scale);
	for (char i = 1; i < 8; i++) {
		int dx = i * SIZE;
		line(dx+X, Y, dx+X, scale+Y);
		line(X, dx+Y, scale+X, dx+Y);
	}
	//������
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (Ps[i][j]) {
				if (Ps[i][j] < 0) setfillcolor(BLACK);
				else setfillcolor(WHITE);
				fillcircle((0.5 + j) * SIZE+X, (0.5 + i) * SIZE+Y, (float)SIZE / Rpro);
			}
		}
	}
}
void BD::drawone(char& x, int X, int Y) {
	int i = x % 8, j = x / 8;
	setfillcolor((tts > 0) ? WHITE : BLACK);
	fillcircle((0.5 + i) * SIZE+X, (0.5 + j) * SIZE+Y, (float)SIZE / Rpro);
}
void BD::ShowWhereCan(int X, int Y) {
	setfillcolor(RGB(128, 225, 128));
	for (int i = 1; i <= ns[0]; i++) {
		int x = ns[i] % 8, y = ns[i] / 8;
		fillrectangle(x * SIZE+X, y * SIZE+Y, (x + 1) * SIZE+X, (y + 1) * SIZE+Y);
	}
}
int BD::WhoseScore(bool which) {
	int scores = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((which && Ps[i][j] > 0) || (!which && Ps[i][j] < 0)) scores++;
		}
	}
	return scores;
}
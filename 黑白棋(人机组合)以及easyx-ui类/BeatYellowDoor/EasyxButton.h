#ifndef EASYXBUTTON_H_
#define EASYXBUTTON_H_
#include <stdio.h>
#include <graphics.h>
#include<easyx.h>
#include <string>
#include"EasyxComponent.h"

class button :public COMP {
public:
	char* content;
	char* pic;			//����ͼƬ
	COLORREF bgcolor;
	COLORREF textcolor;
	UINT sty;			//Ĭ�� ˮƽ���У���ֱ���У�һ����ʾ

	button(int x, int y, int w, int h, p fun, const char n[] = "", const char pict[] = NULL, COLORREF bg = WHITE, COLORREF te = BLACK, UINT st = DT_CENTER | DT_VCENTER | DT_SINGLELINE);	//���Ͻ�x��y�����ߣ��������ִ�еĺ�������ʾ�ı�����ť����������NULL����������ɫ���ı���ɫ���ı���ʽ
	button(button& ano);
	~button();
	virtual void draw();
	//���������ж��Ƿ���
	virtual void receiveEvent(int cx, int cy);
	//�����趨��ʽ�������£����ĵĵط��ͷ�NULL
	void resetstyle(const char n[] = NULL, const char p[]=NULL, COLORREF bg = NULL, COLORREF te = NULL, UINT st = NULL);
	//ȡ��ͼƬ��ʾ��������
	void cancelpic();
};

button::button(int x, int y, int w, int h, p fun, const char n[], const char pict[], COLORREF bg, COLORREF te, UINT st):
	COMP(x,y,w,h,fun),bgcolor(bg),textcolor(te),sty(st)
{
	if (pict != NULL) {
		pic = new char[strlen(pict) + 1];
		strcpy(pic, pict);
	}
	else pic = NULL;
	content = new char[strlen(n) + 1];
	strcpy(content, n);
}

button::button(button& ano):
	COMP(ano.area->left, ano.area->top, ano.area->right, ano.area->bottom, ano.clickAction), bgcolor(ano.bgcolor), textcolor(ano.textcolor),sty(ano.sty)
{
	content = new char[strlen(ano.content) + 1];
	strcpy(content, ano.content);
	pic = new char[strlen(ano.pic) + 1];
	strcpy(pic, ano.pic);
}
button::~button() {
	delete[]content;
}
void button::draw() {
	setbkmode(TRANSPARENT);
	if (pic != NULL) {
		IMAGE bp;
		loadimage(&bp, pic, area->right - area->left, area->bottom - area->top);
		putimage(area->left, area->top, &bp);
	}
	else {
		setfillcolor(bgcolor);
		solidrectangle(area->left, area->top, area->right, area->bottom);
	}
	settextcolor(textcolor);
	drawtext(content, area, sty);
}
void button::resetstyle(const char n[], const char pict[], COLORREF bg, COLORREF te, UINT st) {
	if (n != NULL) {
		delete[]content;
		content = new char[strlen(n) + 1];
		strcpy(content, n);
	}
	if (pict != NULL) {
		if (pic != NULL)	delete[]pic;
		pic = new char[strlen(pict) + 1];
		strcpy(pic, pict);
	}
	if(bg!=NULL) bgcolor = bg;
	if (te != NULL) textcolor = te;
	if (st != NULL) sty = st;
	setrop2(R2_COPYPEN);
	draw();
}
void button::receiveEvent(int cx, int cy) {
	if (isClickMe(cx, cy)) {
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clickAction, NULL, NULL, NULL);
	}
}
void button::cancelpic() {
	delete[]pic; 
	pic = NULL;
	draw();
}
#endif

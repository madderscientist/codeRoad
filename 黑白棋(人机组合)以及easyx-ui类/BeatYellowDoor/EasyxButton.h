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
	char* pic;			//背景图片
	COLORREF bgcolor;
	COLORREF textcolor;
	UINT sty;			//默认 水平居中，垂直居中，一行显示

	button(int x, int y, int w, int h, p fun, const char n[] = "", const char pict[] = NULL, COLORREF bg = WHITE, COLORREF te = BLACK, UINT st = DT_CENTER | DT_VCENTER | DT_SINGLELINE);	//左上角x、y，宽、高，被点击后执行的函数，显示文本，按钮背景（无则NULL），背景颜色，文本颜色，文本格式
	button(button& ano);
	~button();
	virtual void draw();
	//根据坐标判断是否点击
	virtual void receiveEvent(int cx, int cy);
	//重新设定样式，并更新，不改的地方就放NULL
	void resetstyle(const char n[] = NULL, const char p[]=NULL, COLORREF bg = NULL, COLORREF te = NULL, UINT st = NULL);
	//取消图片显示，并更新
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

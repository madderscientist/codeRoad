#pragma once
#ifndef EASYXCOMPONENT_H_
#define EASYXCOMPONENT_H_
#include<graphics.h>
typedef void (*p)();
/*用左上角x、y和宽度、高度构造！*/
class COMP{
public:
	inline bool isClickMe(int cx, int cy);
	RECT *area;
	COMP(int x, int y, int w, int h, p fun);
	COMP(COMP& ano);
	void (*clickAction)();
	virtual void receiveEvent(int cx, int cy);
	virtual void draw() = 0;
};

COMP::COMP(int x, int y, int w, int h, p fun):clickAction(fun) {
	area = new RECT();
	area->left = x;
	area->top = y;
	area->right = x + w;
	area->bottom = y + h;
}
COMP::COMP(COMP& ano) {
	area = new RECT();
	area->left = ano.area->left;
	area->top = ano.area->top;
	area->right = ano.area->right;
	area->bottom = ano.area->bottom;
}
bool COMP::isClickMe(int cx, int cy) {
	return cx >= area->left && cy >= area->top && area->right >= cx && area->bottom >= cy;
}

void COMP::receiveEvent(int cx, int cy) {
	if (isClickMe(cx, cy)) clickAction();
}
#endif
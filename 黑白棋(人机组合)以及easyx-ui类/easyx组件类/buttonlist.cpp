#include"buttonlist.h"
void linklist::creatnew(int num, int x, int y, int w, int h, p fun, const char p[], const char n[], COLORREF te, UINT st) {
	head = new node(new button(x, y, w, h, fun, p, n, st), num, head);
	head->data->draw();
	length++;
}
void linklist::creatnew(int num, int x, int y, int w, int h, p fun, const char n[], COLORREF bg, COLORREF te, UINT st) {
	head = new node(new button(x, y, w, h, fun, n, bg, te, st), num, head);
	head->data->draw();
	length++;
}
#ifndef LINKLIST_H_
#define LINKLIST_H_
#include "EasyxButton.h"
IMAGE bgpic;
struct node {
	button* data;
	int id;
	node* next;
	node(button* x, int num, node* n = NULL) :data(x), next(n),id(num) {}
};
class linklist {
	void delbut(button* a) {
		putimage(a->area->left, a->area->top, a->area->right - a->area->left+1, a->area->bottom - a->area->top+1, &bgpic, a->area->left, a->area->top);
		delete a;
	}
	void dele(node* p) {
		if (p != tail) {
			delbut(p->data);
			dele((*p).next);
		}
		delete p;
	}
public:
	node* head;
	node* tail;
	int length;
	linklist() :length(0),tail(NULL),head(NULL) {}
	linklist(linklist& A) {
		node* p = tail;
		node* p2 = A.head;
		length = A.length;
		for (int i = 1; i <= length; ++i) {
			p = new node((*p2).data, (*p2).id, p);
			p2 = (*p2).next;
		}
		head = p;
	}

	~linklist() {
		dele(head);
	}

	int size() { return length; }
	//头插↓
	void creatnew(int num, int x, int y, int w, int h, p fun, const char *n = "", const char *pict = NULL, COLORREF bg = WHITE, COLORREF te = BLACK, UINT st = DT_CENTER | DT_VCENTER | DT_SINGLELINE) {
		head = new node(new button(x, y, w, h, fun, n, pict, bg, te, st), num, head);
		head->data->draw();
		length++;
	}
	//按序号删
	void remove(int index) {
		if (index <= length) {
			length--;
			node* p = head;
			if (index == 1) {
				p = (*p).next;
				delbut(head->data);
				delete head; head = p; return;
			}
			else {
				for (int i = 2; i < index; ++i) {
					p = (*p).next;
				}
				node* p2 = (*p).next;
				delbut(p2->data);
				(*p).next = (*p2).next;
				delete p2;
			}
		}
	}
	button* & visit(int ID) {
		node* p = head;							//此时p已经指向了第一个数据
			while (p->id != ID) {			//懒得做越界判断了
				p = (*p).next;
			}
		return (*p).data;
	}
	//按id找位置
	int indexof(int num, int n = 1) {
		node* p = head;
		for (int i = n; i <= length; i++) {
			if (i >= n && p->id == num) return i;
			p = p->next;
		}
		return -1;
	}
	//删id
	void findandremove(int num) {
		node* p2 = head;
		node* p = head->next;
		if (p2->id == num) {
			delbut(head->data);
			length--;
			delete head; head = p; return;
		}
		while (p != tail) {
			if (p->id == num) {
				delbut(p->data);
				length--; p2->next = p->next;
				delete p; return;
			}
			p2 = p;
			p = p->next;
		}
	}
	void findevent(int x,int y) {
		node* p = head;
		while (p != tail) {
			p->data->receiveEvent(x,y);
			p = p->next;
		}
	}
};
#endif

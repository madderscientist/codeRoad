/*��������
���壺linklist <��������>��ʶ��(������,��ʼֵ);��ʼֵĬ��Ϊ0 
��ȡ�����ȣ�size()
�����ֵ��insert(������λ��,�����ֵ)
����ɾ����remove(Ҫɾ��λ��) 
�����ض��visit(Ҫ���ʵ��������)
�ı�ĳλ�õ�ֵ��change(λ��,ֵ)
������indexof(����ֵ,�ӵڼ��ʼ����);Ĭ�ϴӵ�һ�ʼ�����ظ�ֵ�ĵ�һ������ֵ
�����print()
headinsert(ֵ)<=>insert(1,ֵ)
tailinsert(ֵ)<=>insetr(1+size(),ֵ) 
�ж��Ƿ�Ϊ�գ�ifempty() 
*/
#include <iostream>
#include <string>
#include <windows.h>
#include <cmath>
using namespace std;
template <class T>
struct node{
    T data;
    node *next;
	node(T x,node* n=NULL):data(x),next(n){}
};
template <class T>
class linklist{
private:
	void dele(node<T>* p){
		if((*p).next!=tail)
		dele((*p).next);
		delete p;
	}
public:
	node<T>* head;
	node<T>* tail;
	int length;

	linklist(int n=1,T value=0){
		node<T>* p=NULL;
		tail = p;
		length=n;
		for(int i=1;i<=n;++i){
			p=new node<T>(value,p);
		}
		head=p;
	}

	linklist(linklist &A){
		node<T>* p=tail;
		node<T>* p2=A.head;
		length=A.length;
		for(int i=1;i<=length;++i){
			p=new node<T>((*p2).data,p);
			p2=(*p2).next;
		}
		head=p;
	}


	~linklist(){
		dele(head);
	}

	int size(){return length;}

	void insert(int index,T value){
		if(index<length+2){
			length++;
			node<T> *p=head;
			if(index==1){head=new node<T>(value,head);}
			else{
				for(int i=2;i<index;i++){
				p=(*p).next;
			}
			(*p).next=new node<T>(value,(*p).next);
			}
		}
	}

	void remove(int index){
		if(index<=length){
			length--;
			node<T> *p=head;
			if(index==1){
				p=(*p).next;delete head;head=p;
			}
			else{
				for(int i=2;i<index;++i){
					p=(*p).next;
				}
				node<T> *p2=(*p).next;
				(*p).next=(*p2).next;
				delete p2;
			}
		}
	}

	T& visit(int index){
		node<T>* p=head;					//��ʱp�Ѿ�ָ���˵�һ������
		for(int i=1;i<index;++i){		//ÿ��n�Σ���ָ��ڣ�n+1�������ݣ�����i��1��ʼ�����Ĵ�����index-1
			p=(*p).next;
		}
		return (*p).data;
	}

	void change(int index,T value){
		node<T>* p=head;
		for(int i=1;i<index;++i){
			p=(*p).next;
		}
		(*p).data=value;
	}

	int indexof(T value,int n=1){
		node<T> *p=head;
		for(int i=n;i<=length;i++){
			if(i>=n&&p->data==value) return i;
			p=p->next;
		}
		return -1;
	}

	void print(){
		node<T> *p=head;
		for(int i=0;i<length;++i){
			cout<<(*p).data;
			p=(*p).next;
			if(i<length-1)cout<<',';
		}
		cout<<endl;
	}
	void headinsert(T value){insert(1,value);}
	void tailinsert(T value){insert(length+1,value);}
	bool ifempty(){return length?true:false;}
};

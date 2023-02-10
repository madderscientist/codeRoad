/*分数矩阵类
矩阵为方阵，每个元素都是分数
matrix m(阶数)，默认每个元素都是0；或matrix m("放了矩阵的文件名.txt");
fra x=m.id(1,3)：选出第2行第4个元素
支持矩阵之间的+-*，支持矩阵合并a&m：新矩阵是a和m横向排列的矩阵 
维数=m.dimension();
m.Gauss();高斯消元,会改变矩阵
m.Det();用高斯消元求行列式
m.saveas("文件名");输出分数类矩阵
m.print();把矩阵整个输出
m.Trace();计算迹，返回分数类
m.Transpose();返回转置之后的矩阵
m.Inverse();返回方阵的逆矩阵 
*/
#ifndef FRAMATRIX_H_
#define FRAMATRIX_H_

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include "fraction.h"
using namespace std;
bool ifempty(char *name) {			//判断是否为空文本
	ifstream my;
	my.open(name);
	char ch;
	my>>ch;
	my.close();
	if(ch==0) return true;
	else return false;
}
int ColumnNum(char *name) {			//数几列
	ifstream my;
	my.open(name);
	int n=0;
	int ch;
	fra FRA;
	while(ch!=EOF&&ch!='\n') {
		my>>FRA;
		ch=my.get();
		n++;
	}
	my.close();
	return n;
}
int LineNum(char *name) {			//数几行
	ifstream my;
	my.open(name);
	char ch=my.get();
	int i=1;
	bool mode;		//防止最后一行之后换行,最后一行不写东西不算。
	while(ch!=EOF) {
		mode=false;
		if(ch=='\n') {
			++i;
			mode=true;
		}
		ch=my.get();
	}
	if(mode) --i;
	return i;
}


class matrix {
		fra **m;
		int L;			//行数line
		int C;			//列数column
		void Build(int n,int u=-1) { 
			L=n;
			C=u<0?n:u;
			m=new fra*[n];
			for(int k=0; k<n; k++)
				m[k]=new fra[C];
		}
	public:
		matrix(int n,int u=-1) {
			Build(n,u);   //已知阶数构造
		}
		matrix(matrix &x) {
			Build(x.L,x.C);
			for(int i=0; i<L; i++)
				for(int j=0; j<C; j++)
					m[i][j]=x.m[i][j];
		}
		matrix(char *name) {				//用文件名构造矩阵
			if(ifempty(name)) cout<<name<<"中没有矩阵！";
			else {
				Build(LineNum(name),ColumnNum(name));
				ifstream my;
				my.open(name);
				char ch=0;
				for(int i=0; i<L*C&&ch!=EOF; i++) {
					my>>m[i/C][i%C];
					ch=my.get();
				}
				my.close();
			}
		}

		~matrix() {
			for(int k=0; k<L; k++) {
				delete []m[k];
			}
			delete []m;
		}

		fra& id(int n,int m) {		//选出特定元素
			if(n>=L||m>=C) {
				cout<<"超出矩阵大小！\n";
				fra *wrong;
				return *wrong;
			}
			return *(*(this->m+n)+m);
		}
		matrix& operator=(matrix &x);
		matrix& operator+(matrix &x);
		matrix& operator-(matrix &x);
		matrix& operator*(matrix &x);
		matrix& operator&(matrix &x);
		template <class X>
		matrix& operator*(X x);
		template <class X>
		friend matrix& operator*(X x,matrix &y) {
			return y*x;
		}

		int dimension() {
			return L;
		}
		void print();
		void saveas(char *name);
		fra Trace();
		int Gauss(bool simest=true);
		fra &Det();
		matrix& Inverse();
		matrix& Transpose();
};
matrix& matrix::operator=(matrix &x){
	for(int k=0;k<L;k++) delete []m[k];
	Build(x.L,x.C);
	for(int i=0;i<L;i++)
		for(int j=0;j<C;j++)
			m[i][j]=x.m[i][j];
	return *this;
}
matrix& matrix::operator+(matrix &x) {
	if(this->L!=x.L) {
		cout<<"不能相加！\n";
		return *this;
	}
	matrix *q=new matrix(L,C);
	for(int i=0; i<L; i++) {
		for(int j=0; j<C; j++) {
			q->m[i][j]=m[i][j]+x.m[i][j];
		}
	}
	return *q;
}
matrix& matrix::operator-(matrix &x) {
	if(this->L!=x.L) {
		cout<<"不能相减！\n";
		return *this;
	}
	matrix *q=new matrix(L,C);
	for(int i=0; i<L; i++) {
		for(int j=0; j<C; j++) {
			q->m[i][j]=m[i][j]-x.m[i][j];
		}
	}
	return *q;
}
matrix& matrix::operator*(matrix &x) {
	if(this->C!=x.L) {
		cout<<"不能相乘！\n";
		return *this;
	}
	matrix *q=new matrix(L,x.C);
	for(int i=0; i<L; i++) {		//行
		for(int j=0; j<C; j++) {	//列
			for(int k=0; k<L; k++) q->m[i][j]=q->m[i][j]+m[i][k]*x.m[k][j];
		}
	}
	return *q;
}
matrix& matrix::operator&(matrix &x) {
	matrix *q=new matrix(max(L,x.L),C+x.C);
	int i,j;
	for(i=0;i<L;i++)
		for(j=0;j<C;j++)
			q->m[i][j]=this->m[i][j];
	for(i=0;i<x.L;i++)
		for(j=0;j<x.C;j++)
			q->m[i][j+C]=x.m[i][j];
	return *q;
}

template <class X>
matrix& matrix::operator*(X x) {
	matrix *q=new matrix(*this);
	for(int i=0; i<L; i++) {
		for(int j=0; j<C; j++) {
			q->m[i][j]=q->m[i][j]*x;
		}
	}
	return *q;
}

void matrix::print() {
	for(int i=0; i<L; i++) {
		for(int j=0; j<C; j++) {
			cout<<m[i][j];
			if(j!=C-1) cout<<'\t';
		}
		cout<<endl;
	}
}
void matrix::saveas(char *name) {
	ofstream os;
	os.open(name);
	for(int i=0; i<L; i++) {
		for(int j=0; j<C; j++) {
			os<<m[i][j];
			if(j!=C-1) os<<'\t';
		}
		if(i!=L-1) os<<endl;
	}
	os.close();
}
fra matrix::Trace() {
	fra sum;
	if(C!=L) {
		cout<<"不是方阵，没有迹！\n";
		return sum;
	}
	for(int i=0; i<L; i++) {
		sum=sum+m[i][i];
	}
	return sum;
}
int matrix::Gauss(bool simest) {			//列最小元高斯消元 ,true消成行最简形，false消成上三角形
	int basex=0,basey=0;
	int pm=1;
	while(basex<C&&basey<L) {
		int x=basey,y=x+1;				//变量多用，x此处是最小元的位置，y此处是循环标志变量
		fra temp=m[basey][basex];		//变量多用，此时temp是最小值

		for(; y<L; ++y) {				//找最小元,如果绝对值不为零 且 比原来更小或原来就是零 
			if(m[y][basex].fenzi()!=0 && (abs(temp)>abs(m[y][basex]) || temp.fenzi()==0)) {
				temp=m[y][basex];
				x=y;
			}
		}

		if(temp.fenzi()!=0) {			//如果存在非零元就做消元
			if(x!=basey) {				//如果就是最小，就不换
				fra *swap=m[basey];
				m[basey]=m[x];
				m[x]=swap;
				pm*=-1;
			}
				//开始消元，此后x、y用作消元循环变量
			for(y=simest?0:basey; y<L; ++y) {
				if(y==basey) continue;
				if(m[y][basex].fenzi()!=0) {
					temp=m[y][basex]/m[basey][basex];
					m[y][basex]=0;
					for(x=basex+1; x<C; x++) m[y][x]=m[y][x]-(m[basey][x]*temp);
				}
			}
			++basey;
		}
		//不管是不是全零，都要往右推
		++basex;
	}
	return pm;
}
fra& matrix::Det() {
	fra *result=new fra();
	if(C!=L) {
		cout<<"不是方阵，不能求行列式！\n";
		return *result;
	}
	matrix x(*this);
	*result=x.Gauss(false);
	for(int i=0; i<x.L; i++) {
		*result=*result*x.m[i][i];
	}
	return *result;
}
matrix& matrix::Inverse() {
	if(L!=C){
		cout<<"不是方阵，不能求逆！\n";
		return *this;
	}
	//构造单位矩阵
	int i,j;
	matrix *q=new matrix(L);
	for(i=0;i<L;++i) q->m[i][i]=1;
	//消元
	matrix result((*this)&(*q)); 
	int det=result.Gauss();
	//判断是否可逆 
	for(i=0;i<L;i++) det=det*result.m[i][i].fenzi();
	if(det==0){
		cout<<"该矩阵不可逆！\n";
		return *this;
	}
	for(i=0;i<L;++i)
		for(j=0;j<L;++j)
			q->m[i][j]=result.m[i][j+L]/result.m[i][i];
	return *q;
}
matrix& matrix::Transpose() {
	matrix *result=new matrix(C,L);
	for(int i=0; i<L; ++i)
		for(int j=0; j<C; ++j)
			result->m[j][i]=this->m[i][j];
	return *result;
}
#endif

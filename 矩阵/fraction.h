/*
分数类：
定义：	fra a或fra a(x,y),其中x、y是小数或整数,不支持fra a=2之类定义时赋值，但支持fra a=x,其中x为分数 
		fra a(x),其中x为分数
		支持cin>>a>>b,请输入一个分数如：24.12/123。若输入多个，之间用空格隔开 
		注意：使用小数构造时可能会有精度损失
输出：	a.print();或
		cout<<a;
		a.value 返回小数形式 
		a.fenmu()返回分母
		a.fenzi()返回分子 
运算：	支持 加 减 乘 除，返回分数类;
其他：	会自动约分。 
*/
#ifndef FRACTION_H_
#define FRACTION_H_

#include <iostream>
#include <string>
#include <cmath>
using namespace std;
inline int gcf(int a,int b){		//greatest common factor 最大公约数 辗转相除法
	int x=abs(a),y=abs(b);
	while(x!=0&&y!=0){
		if(x>y) x=x%y;
		else y=y%x;
	}
	return (x==0)?y:x;
}
class fra {
		int z;		//分子
		int m;		//分母
	public:
		fra():z(0),m(1){}
		fra(int Z,int M=1):z(Z),m(M) {simplify();}
		fra(double n,double m=1.0);
		fra(double n,int m=1);
		fra(int n,double m=1.0);
		fra(fra &x):z(x.z),m(x.m){} 

		void operator=(fra &x);
		void operator=(int x);
		void operator=(double x);
		fra &operator+(fra &x);
		fra &operator+(int x);
		fra &operator+(double x);
		fra &operator-(fra &x);
		fra &operator-(int x);
		fra &operator-(double x);
		fra &operator*(fra &x);
		fra &operator*(int x);
		fra &operator*(double x);
		fra &operator/(fra &x);
		fra &operator/(int x);
		fra &operator/(double x);
		fra &operator^(int x);
		
		template <class X>
		fra &operator+=(X x){*this=*this+x;return *this;}
		template <class X>
		fra &operator-=(X x){*this=*this-x;return *this;}
		template <class X>
		fra &operator*=(X x){*this=*this*x;return *this;}
		template <class X>
		fra &operator/=(X x){*this=*this/x;return *this;}
		
		bool operator==(fra &x){return (z*x.m==m*x.z);}
		bool operator>=(fra &x){return (z*x.m>=m*x.z);}
		bool operator<=(fra &x){return (z*x.m<=m*x.z);}
		bool operator>(fra &x){return (z*x.m>m*x.z);}
		bool operator<(fra &x){return (z*x.m<m*x.z);}
		bool operator!=(fra &x){return (z*x.m!=m*x.z);}

		template <class X>
		friend X & operator<<(X &out,fra &x);
		template <class X>
		friend X & operator>>(X &in,fra &x);
		
		friend fra & operator/(int s,fra&x);
		friend fra & operator*(int s,fra&x);
		friend fra & operator+(int s,fra&x);
		friend fra & operator-(int s,fra&x);
		
		friend fra & operator/(double s,fra&x);
		friend fra & operator*(double s,fra&x);
		friend fra & operator+(double s,fra&x);
		friend fra & operator-(double s,fra&x);
		friend fra & abs(const fra& x);
		double value();
		void simplify();
		void print();
		int fenzi(){return z;}
		int fenmu(){return m;}
};
void fra::simplify(){
	if(z==0){m=1;return;}
	int x=gcf(z,m);
	z=z/x;
	m=m/x;
	if(m<0){
		this->z*=-1;this->m*=-1;
	}
}

void fra::operator=(fra &x){z=x.z;m=x.m;}
void fra::operator=(int x){z=x;m=1;}

fra& fra::operator*(fra &x){
	int a=gcf(z,x.m);
	int b=gcf(m,x.z);
	fra *temp=new fra((z/a)*(x.z/b),(m/b)*(x.m/a));
	if(temp->m<0){temp->m*=-1;temp->z*=-1;} 
	return *temp;
}
fra& fra::operator/(fra &x){
	if(x.z==0){
		cout<<"分子不能为零！"<<endl;
		return *this;
	}
	int a=gcf(z,x.z);
	int b=gcf(m,x.m);
	fra *temp=new fra((z/a)*(x.m/b),(m/b)*(x.z/a));
	if(temp->m<0){temp->m*=-1;temp->z*=-1;}
	return *temp;
}
fra& fra::operator+(fra &x){
	fra *temp=new fra(z*x.m+m*x.z,m*x.m);
	temp->simplify();
	return *temp;
}
fra& fra::operator-(fra &x){
	fra* temp=new fra(z*x.m-m*x.z,m*x.m);
	temp->simplify();
	return *temp;
}

fra& fra::operator/(int x){
	int c=gcf(z,x); 
	fra *w=new fra(z/c,x/c*m);
	if(w->m<0){w->m*=-1;w->z*=-1;}
	return *w;
}
fra& fra::operator+(int x){
	fra *w;
	w=new fra(z+x*m,m);
	return *w;
}
fra& fra::operator-(int x){
	fra *w;
	w=new fra(z-x*m,m);
	return *w;
}
fra& fra::operator*(int x){
	int c=gcf(x,m);
	fra *w=new fra(x/c*z,m/c);
	if(w->m<0){w->m*=-1;w->z*=-1;}
	return *w;
}

double fra::value(){
	double a=z;
	return a/m;
}

fra& ArrToFra(char *a) {
	int i=0,pm=1,num=0;	//位置、正负、数字 
	int mode=-1;		//小数整数模式 
	bool chose=true;	//分子分母模式 
	fra *fenzi;
	fra *fenmu; 

	for(; a[i]!='\0'&&a[i]!=0; i++) {		//控制整个读取
		if(a[i]=='-') {			//正负判断 
			pm=-1;
			continue;
		}
		
		if(a[i]=='/'){			//判断分子分母 
			if(chose){
				if(mode==-1) mode=0;
				fenzi= new fra(num*pm,(int)pow(10,mode));				
				mode=-1;pm=1;num=0;		//重置参数 
				chose=false;
				continue;			
			}
			else {cout<<"多个除号，";goto ED;}
		}
		
		if(a[i]=='.'){			//判断整小数 
			if(mode==-1){
				mode=0;
				continue;
			}
			else {cout<<"多个小数点，";goto ED;}
		}
		
		if(a[i]<='9'&&a[i]>='0') {	//看看是不是只有这几个字符，是否符合书写规范
			if(mode>-1) mode++;
			num=num*10+(a[i]-'0');
		}
		else {
			cout<<a[i++]<<endl; 
			cout<<"不是数字，";
			goto ED;
		} 
	}
	
	if(mode==-1) mode=0;	
	if(chose){					//如果没有写分母，直接返回分子 
		fenzi=new fra(num,(int)pow(10,mode));
		fenzi->simplify();
		return *fenzi;
	}
	
	if(num==0) {cout<<"没写分母，";goto ED;}			//淘汰写了/但不写分母的情况 
	fenmu=new fra(num*pm,(int)pow(10,mode));
	return (*fenzi)/(*fenmu);

ED:	cout<<"输入错误！";
	fra *wrong;
	return *wrong;
}

fra &DouToFra(double x){
	char a[50];
	sprintf(a, "%.8lf", x);
	return ArrToFra(a);
}

fra::fra(double n,double m){
	fra s=DouToFra(n)/DouToFra(m);
	this->z=s.z;this->m=s.m;
}
fra::fra(int n,double m){
	fra s=n/DouToFra(m);
	this->z=s.z;this->m=s.m;
}
fra::fra(double n,int m){
	fra s=DouToFra(n)/m;
	this->z=s.z;this->m=s.m;
}

fra& fra::operator-(double x){
	fra *q;
	q=new fra(DouToFra(x));
	*q=*this-*q;
	return *q;
} 
fra& fra::operator+(double x){
	fra *q;
	q=new fra(DouToFra(x));
	*q=*this+*q;
	return *q;
} 
fra& fra::operator/(double x){
	fra *q;
	q=new fra(DouToFra(x));
	*q=*this/(*q);
	return *q;
}
fra& fra::operator*(double x){
	fra *q;
	q=new fra(DouToFra(x));
	*q=*this*(*q);
	return *q;
} 
void fra::operator=(double x){
	*this=DouToFra(x);
}
fra& fra::operator^(int x){
	if(x==0) return *(new fra(1,1));
	fra *q;
	if(x>0) q=new fra(*this);
	else {
		if(this->z==0){
			cout<<"错误：分母为零而指数为复数！\n";
			return *q;
		}
		else {
			q=new fra(this->m,this->z);x=0-x;
		}		
	}
	q->z=pow(q->z,x);q->m=pow(q->m,x);
	return *q;
}

fra & operator/(int s,fra&x){
	fra *w;
	if(x.z==0){
		cout<<"分母不能为零！";
		return x;
	}
	w=new fra(s*x.m,x.z);
	return *w;
}
fra & operator*(int s,fra&x){
	fra *w;
	w=new fra(s*x.z,x.m);
	return *w;
}
fra & operator+(int s,fra&x){
	fra *w;
	w=new fra(s*x.m+x.z,x.m);
	return *w;
}
fra & operator-(int s,fra&x){
	fra *w;
	w=new fra(s*x.m-x.z,x.m);
	return *w;
}

fra & operator-(double s,fra&x){
	fra *q;
	q=new fra(DouToFra(s));
	*q=*q-x;
	return *q;
}
fra & operator+(double s,fra&x){
	fra *q;
	q=new fra(DouToFra(s));
	*q=*q+x;
	return *q;
}
fra & operator/(double s,fra&x){
	fra *q;
	q=new fra(DouToFra(s));
	*q=*q/x;
	return *q;
}
fra & operator*(double s,fra&x){
	fra *q;
	q=new fra(DouToFra(s));
	*q=(*q)*x;
	return *q;
}

template <class X>
X & operator<<(X &os,fra &x){
	if(x.m==1) os<<x.z;
	else os<<x.z<<'/'<<x.m;
	return os;
}
template <class X>
X & operator>>(X &in,fra &x){
	char input[50];
	in>>input;
	x=ArrToFra(input);
	return in;
}
fra& abs(const fra& x){
	fra *q=new fra((x.z>0)?x.z:-x.z,x.m);
	return *q;
}

void fra::print(){
	if(m==1) cout<<z;
	else
	cout<<z<<'/'<<m;
}

#endif

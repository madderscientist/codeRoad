#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <bitset>
#include <cmath>
#include "fraction.h" 
using namespace std;
void ShowMatrix(fra **a,int mAx);
void GetMatrix(fra **&a,int &mAx){
	int i,j,ch;
	mAx=0;
	cout<<"请输入一个方阵，最大50*50，各元素为整数，元素间用空格或英文逗号间隔：\n";
	//输入第一行，得到列数，确定行数
	fra *temp;
	temp=new fra [50];
	for(i=0;i<50;i++){
		cin>>temp[i];
		ch=cin.get();
		if(ch=='\n'){
			mAx=i+1;
			break;
		}
	}
	//精准创建分数类数组，拒绝浪费空间！ 
	a=new fra*[mAx];
	for(i=0;i<mAx;i++){
		a[i]=new fra[mAx];
	}
	for(i=0;i<mAx;i++){
		a[0][i]=temp[i];
	}
	delete []temp;
	//清空超过10个的数，用百度得到的清除输入缓冲区内容的函数
	cin.sync();
	//输入后面的
	for(i=1;i<mAx;i++){//控制行
		ch=1;
		for(j=0;j<mAx;j++){
			if(ch=='\n'){cout<<"此行还没输满，本行继续输入上一行的数据：";}
			cin>>a[i][j];
			ch=cin.get();
		}
		cin.sync();
	}
}


void ShowMatrix(fra **a,int mAx){
	int i,j;
	for(i=0;i<mAx;i++){
		for(j=0;j<mAx;j++){
			cout<<a[i][j]<<' ';
		}
		cout<<endl;
	}
}

void diagsum(fra **a,int &mAx){
	fra s;
	for(int i=0;i<mAx;i++){
		s=s+a[i][i];
	}
	cout<<s;
}

int main(){
	fra **a;
	int n;
	GetMatrix(a,n);
	cout<<"\n这是您输入的矩阵：\n";
	ShowMatrix(a,n);
	cout<<"对角线和为：";
	diagsum(a,n);
return 0;}


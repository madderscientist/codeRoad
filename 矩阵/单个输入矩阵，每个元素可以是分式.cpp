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
	cout<<"������һ���������50*50����Ԫ��Ϊ������Ԫ�ؼ��ÿո��Ӣ�Ķ��ż����\n";
	//�����һ�У��õ�������ȷ������
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
	//��׼�������������飬�ܾ��˷ѿռ䣡 
	a=new fra*[mAx];
	for(i=0;i<mAx;i++){
		a[i]=new fra[mAx];
	}
	for(i=0;i<mAx;i++){
		a[0][i]=temp[i];
	}
	delete []temp;
	//��ճ���10���������ðٶȵõ���������뻺�������ݵĺ���
	cin.sync();
	//��������
	for(i=1;i<mAx;i++){//������
		ch=1;
		for(j=0;j<mAx;j++){
			if(ch=='\n'){cout<<"���л�û���������м���������һ�е����ݣ�";}
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
	cout<<"\n����������ľ���\n";
	ShowMatrix(a,n);
	cout<<"�Խ��ߺ�Ϊ��";
	diagsum(a,n);
return 0;}


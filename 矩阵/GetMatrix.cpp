#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
using namespace std;
int main() {
	char input[20];
	int n;
	cout<<"������һ�����������ɾ��󵽡�matrix.txt����\n";
	cin>>input;
	cout<<"��������������\n";
	cin>>n;
	int N=n*n;


	ofstream my;
	my.open("matrix.txt");
	
	
	int i=0,I=0;//i��ѭ����I�Ƹ��� 
	int len=strlen(input);
	while(I<N) {
		int x=input[i%len]-'0';
		if(x==0) {
			I++; 
			my<<0;
			if(I%n==0) my<<'\n';
			else my<<' ';
		}
		else {
			long long y=pow(x,i/len+1);
			int small[50];
			int j=0;
			while(y!=0) {
				small[j++]=y%10;
				y=y/10;
				if(j+I==N) break;
			}
			for(int k=j-1;k>=0;k--){
				my<<small[k];
				I++;
				if(I==N) break;
				if(I%n==0) my<<'\n';
				else my<<' ';
			}
		}
	i++; 
	}
	my.close();
	return 0;
}


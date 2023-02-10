#include <iostream>
#include "FraMatrix.h" 
using namespace std;
int  main(){
	matrix a("matrix.txt");
	matrix b("matrix2.txt");
	matrix c(3);
	c=(a+5*b)*a; 
	c.print();
	c.Gauss(false);
	c=c&b&a;
	c.print();
	return 0;
}

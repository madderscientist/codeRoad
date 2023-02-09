#include "midi.h"
#include <iostream>
using namespace std;
int main(int argc, char* argv[]){
	MidiReader m(argc>1?argv[1]: "believe me.mid");
	cout << "OK!\n";
	m.play();
	return 0;
}

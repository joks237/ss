#include "../inc/emulator.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]) {

	string filename;

	if(argc == 2) {
		filename = argv[1];
	} else {
		cout << "Error, too few arguments!" << endl;
		return -1;
	}
	Emulator e =  Emulator(filename);

	e.proccessMemory();

	//e.printMem();
	//e.test(5, 4294967295, "FFC");

	return 0;
}
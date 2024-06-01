#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <stdio.h>
using namespace std;

class Exception {
public:
	Exception() {}
	string syntaxException();
	string labelException();
	string symbolUndefined();
	string terminalFormatException();
	string wrongNumberOfArgumentsInCMD();
	string wrongFileName();
	string moreThanOneDirectiveException();
	string instructionBeforeLabel();
	string labelInSection();
	string symbolAlreadyExists();
	string firstPassException();
	string instructionsAfterEnd();
	string outputFileException();
	string secondPassException();

};
#endif

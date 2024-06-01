#include "../inc/exception.h"


string Exception::syntaxException() {
	return "Sintaksna greska!";
}

string Exception::labelException() {
	return "Labela nije lepo definisana!";
}

string Exception::symbolUndefined() {
	return "Simbol nije definisan: ";
}

string Exception::terminalFormatException() {
	return "Pogresan format u komandnoj liniji!";
}

string Exception::wrongNumberOfArgumentsInCMD() {
	return "Pogresan broj argumenata komandne linije!";
}

string Exception::wrongFileName() {
	return "Fajl sa tim imenom ne postoji!";
}

string Exception::moreThanOneDirectiveException() {
	return "Vise od jedne direktive u liniji: ";
}

string Exception::instructionBeforeLabel() {
	return "Postoji instrukcija pre labele na liniji: ";
}

string Exception::labelInSection() {
	return "Labela mora biti unutar sekcije";
}

string Exception::symbolAlreadyExists() {
	return "Simbol sa ovim imenom vec postoji u tabeli";
}

string Exception::firstPassException() {
	return "Greska u prvom prolazu!";
}

string Exception::instructionsAfterEnd() {
	return "Ne moze se nalaziti instrukcija nakon .end direktive!";
}

string Exception::outputFileException() {
	return "Greska pri upisu u izlazni fajl!";
}

string Exception::secondPassException() {
	return "Greska u drugom prolazu!";
}
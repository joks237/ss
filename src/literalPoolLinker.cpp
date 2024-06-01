#include "../inc/literalPoolLinker.h"



void LiteralPool::setOffset(string offset, string symName) {
	int i = 0;
	while (i < listOfLiterals->getSize()) {
		if (listOfLiterals->getWithoutRemovingFromList(i)->literalVal == symName) {
			listOfLiterals->getWithoutRemovingFromList(i)->offset = offset;
		
		}
		i++;
	}
}

string LiteralPool::getOffset(string symName) {
	int i = 0;
	while (i < listOfLiterals->getSize()) {
		if (listOfLiterals->getWithoutRemovingFromList(i)->literalVal == symName) {
			return listOfLiterals->getWithoutRemovingFromList(i)->offset;

		}
		i++;
	}
	return "";
}


string LiteralPool::getSymbolName(int addr) {
	int size = listOfLiterals->getSize();
	for (int i = 0; i < size; i++) {
		if (listOfLiterals->getWithoutRemovingFromList(i)->literalAddres == addr) return listOfLiterals->getWithoutRemovingFromList(i)->literalVal;
	}
}

void LiteralPool::printPool() {
	for (int i = 0; i < listOfLiterals->getSize(); i++) {
		cout << listOfLiterals->getWithoutRemovingFromList(i)->literalVal << "    "  << listOfLiterals->getWithoutRemovingFromList(i)->literalAddres << "     " << listOfLiterals->getWithoutRemovingFromList(i)->offset;
		cout << endl;
	}
}
#include "../inc/literalPool.h"


LiteralPool::LiteralPool(string sectionName) : sectionName(sectionName), size(0) {
	listOfLiterals = new List<Literal>();
}

int LiteralPool::countBytes(string hexString) {
	int numChars = hexString.length();
	int numBytes = numChars / 2; // Each byte corresponds to 2 characters

	if (numChars % 2 != 0) {
		// If the number of characters is odd, we have an incomplete byte
		numBytes++;
	}

	return numBytes;
}

void LiteralPool::fixAddressesForLiterals() {
		regex symbol("(([a-zA-Z][a-zA-Z0-9_]*))");
		regex hexRegex("^0x[0-9A-Fa-f]+$");
		smatch val;
		int valLength = 0;
		Literal* lit = this->listOfLiterals->getWithoutRemovingFromList(0);
		lit->literalAddres = this->getLiteralPoolBegining();
		if (regex_match(lit->literalVal, val, symbol)) {
			valLength = 4;
		}
		else { 
			
			if (regex_match(lit->literalVal, val, hexRegex)) {
				lit->literalVal = removeHexPrefix(lit->literalVal);
			}
			else {
				lit->literalVal = decToHex(lit->literalVal);
				lit->literalVal = toUpperCase(lit->literalVal);
			}
			valLength += 4;
		}
		
	
	for (int i = 1; i < listOfLiterals->getSize(); i++) {
		lit = listOfLiterals->getWithoutRemovingFromList(i);
		lit->literalAddres = valLength + this->getLiteralPoolBegining();
		if (regex_match(lit->literalVal, val, symbol)) {
			valLength += 4;
		}
		else {

			if (regex_match(lit->literalVal, val, hexRegex)) {
				lit->literalVal = removeHexPrefix(lit->literalVal);
			}
			else {
				lit->literalVal = decToHex(lit->literalVal);
				lit->literalVal = toUpperCase(lit->literalVal);
			}
			valLength += 4;
		}
		
	}
	
	this->setSize(valLength);
}


bool LiteralPool::checkIfLiteralAlreadyExists(string literal) {
	int i = 0;
	while (i < listOfLiterals->getSize()) {
		if (listOfLiterals->getWithoutRemovingFromList(i)->literalVal == literal) return true;
		i++;
	}
	return false;
}

int LiteralPool::returnAddresForLiteral(string literalVal) {
	int i = 0;
	Literal* lit = nullptr;
	while (i < listOfLiterals->getSize()) {
		if (listOfLiterals->getWithoutRemovingFromList(i)->literalVal == literalVal) {
			lit = listOfLiterals->getWithoutRemovingFromList(i);
			break;
		}
		i++;
	}
	if (lit != nullptr)
		return lit->literalAddres;
	else return 0;
}

void LiteralPool::setOffset(string offset, string symName) {
	int i = 0;
	while (i < listOfLiterals->getSize()) {
		if (listOfLiterals->getWithoutRemovingFromList(i)->literalVal == symName) {
			listOfLiterals->getWithoutRemovingFromList(i)->offset = offset;
			break;
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
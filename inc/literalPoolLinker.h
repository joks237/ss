#ifndef LITERAL_POOL_LINKER_HPP
#define LITERAL_POOL_LINKER_HPP


#include <stdio.h>
#include <string>
#include <regex>
#include <sstream>

#include "listLinker.h"


using namespace std;


struct Literal {
	int literalAddres;
	string literalVal;
	string offset;
	Literal(int address, const std::string& value, string offset)
		: literalAddres(address), literalVal(value), offset(offset) {
	}


};

class LiteralPool {
private:
	string sectionName;
	List<Literal>* listOfLiterals;
	int size;
	int literalPoolBeginig;

public:
	LiteralPool() {}
	LiteralPool(string name) {
		listOfLiterals = new List<Literal>();
	};
	

	void setSize(int size) {
		this->size = size;
	}

	int getSize() {
		return size;
	}

	List<Literal>* getListOfLiterals() {
		return this->listOfLiterals;
	}

	void addLiteralToList(Literal* lit) {
		this->listOfLiterals->addLast(lit);
	}

	void setSectionName(string sectionName) {
		this->sectionName = sectionName;
	}

	string getSectionName() {
		return this->sectionName;
	}

	void setOffset(string offset, string symName);

	string getOffset(string symName);

	string getSymbolName(int addr);

	void fixOffs(string name, int offset);

	void printPool();

	bool checkIfLiteralAlreadyExists(string literal) {
		int i = 0;
		while (i < listOfLiterals->getSize()) {
			if (listOfLiterals->getWithoutRemovingFromList(i)->literalVal == literal) return true;
			i++;
		}
		return false;
	}
};
#endif

#ifndef LITERAL_POOL_HPP
#define LITERAL_POOL_HPP

#include <stdio.h>
#include <string>
#include <regex>
#include <sstream>

#include "list.h"


using namespace std;


struct Literal {
	int literalAddres;
	string literalVal;
	string offset;
	Literal(int address, const std::string& value)
		: literalAddres(address), literalVal(value), offset("0") {
	}


};

class LiteralPool {
private:
	string sectionName;
	List<Literal>* listOfLiterals;
	int size;
	int literalPoolBeginig;
	
public:
	LiteralPool() {};
	LiteralPool(string sectionName);

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

	void setLiteralPoolBegining(int begining) {
		this->literalPoolBeginig = begining;
	}

	int getLiteralPoolBegining() {
		return this->literalPoolBeginig;
	}

	void fixAddressesForLiterals();

	int countBytes(string hexString);

	bool checkIfLiteralAlreadyExists(string literalVal);

	int returnAddresForLiteral(string literalVal);

	void setOffset(string offset, string symName);

	string getOffset(string symName);

	string decToHex(string dec_val) {
		stringstream stream;
		int dec_val1 = stoi(dec_val);
		stream << hex << dec_val1;
		return stream.str();
	}


	string toUpperCase(string line) {
		std::string uppercaseHex = line;
		for (char& ch : uppercaseHex) {
			ch = std::toupper(ch);
		}
		return uppercaseHex;
	}
	string removeHexPrefix(string line) {
		if (line.size() >= 2 && line.substr(0, 2) == "0x") {
			return line.substr(2); // Return the substring without the "0x" prefix
		}
		return line;
	}
};
#endif
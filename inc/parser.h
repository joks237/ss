#ifndef PARSER_HPP
#define PARSER_HPP
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include "listLinker.h"
#include "sectionLinker.h"
#include "symbolLinker.h"
#include "relocationRecordLinker.h"
using namespace std;
class Parser {
private:
	List<Symbol>* listOfSymbols;
	List<Section>* listOfSections;
	List<RelocationRecord>* listOfRelocationRecords;
	vector<string> byteCodes;
	string filename;

public:
	Parser(string filename) : filename(filename) {
		listOfSymbols = new List<Symbol>();
		listOfSections = new List<Section>();
		listOfRelocationRecords = new List<RelocationRecord>();
	}

	bool processFile(string filename);
	vector<string> tokens(string line);
	Section* getSectionByName(string name);
	List<Symbol>* getListOfSymbols() {
		return listOfSymbols;
	}
	List<Section>* getListOfSections() {
		return listOfSections;
	}
	List<RelocationRecord>* getListOfRelocationRecords() {
		return listOfRelocationRecords;
	}

	string getSectionNameForNumber(int number, string filename);
	string getSymbolNameForNumber(int number, string filename);

	void resolveSymbolSections();
	void resolveRelocationSymbols();
};
#endif

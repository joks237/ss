#ifndef LINKER_HPP
#define LINKER_HPP

#include <stdio.h>
#include "symbolLinker.h"
#include "sectionLinker.h"
#include "relocationRecordLinker.h"
#include <string>
#include "listLinker.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

struct SectionAddr {
	string newAddress;
	string sectionName;
	
	SectionAddr(string newAddress,string sectionName)
		: newAddress(newAddress), sectionName(sectionName) {
	}


};


class Linker {
private:
	static List<Symbol>* allSymbols;
	static List<Section>* allSections;
	static List<RelocationRecord>* allRelocationRecords;
	static List<Section>* linkerSections;
	static List<Symbol>* linkerSymbols;
	static List<Symbol>* pomList;
	static List<LiteralPool>* litPools;
	List<SectionAddr>* secAddr;
	static bool isUND;
	static int memory;
public:
	Linker() {
		secAddr = new List<SectionAddr>;
	}

	bool addSymbolsToList(List<Symbol>* list);
	bool addSectionsToList(List<Section>* list);
	bool addRelocationRecordsToList(List<RelocationRecord>* list);
	bool checkSymbol(Symbol* symbol);
	bool checkIfSectionAlreadyExists(string name);
	bool checkIfSymbolAlreadyExists(string name);
	bool checkIfSymbolInPom(string name);
	Symbol* getSymbolForSection(string name, string filename);
	void changeSectionsOffset();
	void pasteSections();
	void resolveSymbolsInSymbolTable();
	int hexToDec(string hex);
	int getOffsetSection(string section, string filename);
	int getOffsetForSection(string section);
	string getSymbolNameForSectionAndOffset(int sectionName, int offset);
	int getSectionNum(string num, string filename);
	int getSectionNumForSymbol(string name);
	Symbol* definedSymbol(string name);
	Symbol* symbolSection(string name, int sec, string filename);
	Symbol* getSymbolForSectionNumAndFileName(int sec, string filename);
	Symbol* getSymbolByNameAndFilename(string name, string filename);
	Symbol* getSymbolByName(string name);
	Section* getSectionByName(string name);
	Section* getSectionByNameAndFilename(string name, string filename);
	Symbol* getSymbol(string name);
	Symbol* getSymbolLinker(string name);
	LiteralPool* getLitPoolForSection(string section, string filename);
	void fixRelocs();
	void washSymbols();
	void setRbrSymbolAndSection();
	void fixSectionForSymbol();
	int getSectionNumberByName(string name);
	void printHexaFile(string addr1, string addr2, string sec1, string sec2, string filename);
	string removeHexPrefix(string line);
	string fixAddr(string addr, string off);
	string toUpperCase(string line);
	string getNewAddresForSection(string name);
	string decToHex(int decimal);
	std::string replaceFirstOccurrence(const std::string& str, const std::string& oldString, const std::string& newString);
	void printLinkerTables();
	void setNewAddresses(string addr1, string addr2, string sec1, string sec2);
	void fixPoolOffset();
	void fixWord();
	void fixByteCodeForSection(string result, string name);
	int getNumberOfLiterals(string name, string filename);
	string addTwoHex(string hexOne, string hexTwo);
	List<Symbol>* getList() {
		return allSymbols;
	}
	List<Section>* getListSections() {
		return allSections;
	}

	List<RelocationRecord>* getAllRelocationRecords() {
		return allRelocationRecords;
	}


	string padNumber(string number, int digits);

	vector<string> fixLiteralsInPoolForByteCode(string name);

	vector<string> findXXX(const std::vector<std::string>& strings, char target);

	int countPairsOfX(const std::string& str, char target);

	string convertToLittleEndian(const std::string& hexString);

	std::string replaceX(const std::string& str, const std::string& newString);

	std::string removeX(const std::string& inputString);

	std::string padStringToEightCharacters(const std::string& str);
	std::string replaceXsWithNewChars(const std::string& strWithXs, const std::string& strWithoutXs);
};

#endif

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <regex>
#include <sstream>
#include <bitset>
#include <stdio.h>
#include <iomanip>
#include <vector>
#include "exception.h"
#include "symbol.h"
#include "list.h"
#include "section.h"
#include "literalPool.h"
#include "relocationRecord.h"

using namespace std;


class Assembler {
private:

	string removeWhitespaces(string line);
	string getFirstWord(string inputString);
	string getSecondWord(string inputString);
	string getThirdWord(string inputString);
	string getFourthWord(string inputString);
	bool isLiteral(string line);
	bool isSymbol(string line);
	bool isLiteralValue(string line);
	bool isSymbolValue(string line);
	bool isReg(string line);
	bool isFromMemoryReg(string line);
	bool isFromMemoryWithOffsetLiteral(string line);
	bool isFromMemoryWithOffsetSymbol(string line);

	List<Symbol> *symbolTable;
	List<Section>* sectionTable;
	List<RelocationRecord>* relocationRecords;

	Exception e;
	
	static	int currentSection;
	static int locationCounter;
	static string currentSectionName;
	
public:
	Assembler() {
		symbolTable = new List<Symbol>();
		sectionTable = new List<Section>();
		relocationRecords = new List<RelocationRecord>();
	};

	
	vector<string> extractExternVariables(string line);
	vector<string> extractGlobalVariables(string line);
	vector<string> extractWordVariables(string line);

	int getSkipBytes(string line);
	int countWordsAfterDotWord(string line);
	int getSectionNumberForSymbolName(string name);
	int getOffsetForSymbolName(string name);
	int getRbrForSymbolName(string name);
	int getInstructionNumber(string line);
	int getNumberOfDirective(string dirName);
	int hexToDecimal(string hex_val);
	int binToDecimal(string bin_val);
	int findWordPosition(string line);

	string removeExtraSpaces(string line);
	string readCauseReg(int cause);
	string removeLeadingSpaces(string line);
	string removeTrailingWhitespaces(string line);
	string extractAfterColon(string line);
	string extractLabel(string line);
	string extractLabelWithoutSemicolon(string line);
	string removeDollarSign(string line);
	string padNumber(string number, int digits);
	string addBlankSpace(string line);
	string extractRegister(string line);
	string extractOffsetLiteral(string line);
	string extractOffsetSymbol(string line);
	string toUpperCase(string line);
	string removeHexPrefix(string line);
	string findAndCheckLiteral(string line);
	string findAndCheckSymbol(string line);
	string returnSectionName(int sectionNumber);
	string resolveInstruction(string line);
	string prettifyString(string line);
	string getSectionName(string line);
	string checkReg(string reg);
	string checkOpCode(string opName, string mod);
	string decToHex(string dec_val);
	string decToBinary(int dec_val);

	Section* getOldSection();

	void addLiteralToPool(string lit);
	void setSymbolToBeGlobal(string name);
	void insertIntoCurrentSectionBytes(string line);
	void addSymbolToTableIfAlreadyExistsInTable(Symbol* symbol, string label);
	void addSymbolToTableIfItDoesntExist(string name);
	void clearTable();
	void insertIntoSectionTableBeforeFirstPassUND();
	void printSymbolTable(ostream& file);
	void printSectionTable(ostream& file);
	void printRelocationRecords(ostream& file);
	void printBytesInCode(ostream& file);
	void writeInFile(string filename);

	bool checkIfRegexMatch(string line);
	bool checkIfIsEnd(string line);
	bool setOffsetToSymbol(string symbolName);
	bool checkIfAllSymbolsAreDefined();
	bool checkForMoreThanOneDirectiveInLine(string line);
	bool checkIfThereIsAInstructionBeforeLabel(string line);
	bool checkForSymbolInTable(string label);
	bool checkIfSymbolIsExtern(string name);
	bool checkIfSymbolIsGlobal(string name);
	bool checkIfSymbolInPool(string symbolVal);
	bool checkIfSymbolIsLocal(string name);

	bool firstPass(string line);
	bool secondPass(vector<string> filename);

	
};
#endif


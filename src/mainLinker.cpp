#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include "../inc/parser.h"
#include "../inc/linker.h"
#include <iomanip>
#include <vector>
using namespace std;

string removeHexPrefix(string line) {
	if (line.size() >= 2 && line.substr(0, 2) == "0x") {
		return line.substr(2); // Return the substring without the "0x" prefix
	}
	return line;
}


string extractCode(string input) {
	size_t atPos = input.find('@');
	string code = input.substr(input.find('=') + 1, atPos - input.find('=') - 1);
	return code;
}

std::string extractAddress(const std::string& input) {
    std::size_t atPos = input.find('@');
    std::string address = input.substr(atPos + 1, input.length() - atPos - 1);

    return removeHexPrefix(address);
}

int main(int argc, char *argv[]) {


	if(argc < 12) {
		cout << "Too few arguments!" << endl;
		return -1;
	}
	
	vector<string> inputFiles;
	string filenameOut;

	string addr1;
	string sec1;
	string addr2;
	string sec2;
	
		sec1 = extractCode(argv[2]);
		addr1 = extractAddress(argv[2]);
		sec2 = extractCode(argv[3]);
		addr2 = extractAddress(argv[3]);
		filenameOut = argv[5];
		
		for(int i = 6; i < 12; i++) {
			inputFiles.push_back(argv[i]);
		}
	

	Linker l;

	for(int i = 0; i < inputFiles.size(); i++) {
		Parser* parser = new Parser(inputFiles.at(i));
		if(!parser->processFile(inputFiles.at(i))) {
			cout << "Error while processing file with filename " << inputFiles.at(i) << endl;
			return -1;
		}
		List<Symbol>* listN = parser->getListOfSymbols();
		if(!l.addSymbolsToList(listN)) {
			cout << "Linkerska greska pri ubacivanju simbola za fajl " << inputFiles.at(i) << endl;
			return -1;
		}
		if(!l.addSectionsToList(parser->getListOfSections())) {
			cout << "Linkerska greska pri ubacivanju sekcija za fajl " << inputFiles.at(i) << endl;
			return -1;
		}
		if(!l.addRelocationRecordsToList(parser->getListOfRelocationRecords())) {
			cout << "Linkerska greska pri ubacivanju relokacionih zapisa za fajl " << inputFiles.at(i) << endl;
			return -1;
		}

	}

	l.changeSectionsOffset();

	l.resolveSymbolsInSymbolTable();
	l.fixRelocs();
	l.pasteSections();
	l.washSymbols();
	l.setRbrSymbolAndSection();
	l.fixSectionForSymbol();
//l.printLinkerTables();
	
	l.setNewAddresses(addr1, addr2, sec1, sec2);
	l.printHexaFile(addr1, addr2, sec1, sec2, filenameOut);
	return 0;
}
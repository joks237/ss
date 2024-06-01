#include "../inc/linker.h"
#include <iomanip>

List<Symbol>* Linker::allSymbols = new List<Symbol>();
List<Section>* Linker::allSections = new List<Section>();
List<Section>* Linker::linkerSections = new List<Section>();
List<Symbol>* Linker::linkerSymbols = new List<Symbol>();
List<Symbol>* Linker::pomList = new List<Symbol>();
List<RelocationRecord>* Linker::allRelocationRecords = new List<RelocationRecord>();
bool Linker::isUND = false;
int Linker::memory = 0;


regex literal("([0-9a-fA-F]+|[0-9]+)");
regex symbol("(([a-zA-Z][a-zA-Z0-9_]*))");

bool Linker::addSymbolsToList(List<Symbol>* list) {
	int listSize = list->getSize();
	
	for (int i = 0; i < listSize; i++) {
		Symbol* newSym = list->getWithoutRemovingFromList(i);
		if (newSym->getName() == "UND") {
			if (!isUND) {
				isUND = true;
				allSymbols->addLast(newSym);
			}
			continue;
		}
		if (!checkSymbol(newSym)) return false;
		else allSymbols->addLast(newSym);
	}

	return true;
}

bool Linker::addSectionsToList(List<Section>* list) {
	int listSize = list->getSize();
	for(int i = 0; i < listSize; i++) {
		Section* newSec = list->getWithoutRemovingFromList(i);
		allSections->addLast(newSec);
	}
	return true;
}

bool Linker::addRelocationRecordsToList(List<RelocationRecord>* list) {
	int listSize = list->getSize();
	for (int i = 0; i < listSize; i++) {
		RelocationRecord* newSec = list->getWithoutRemovingFromList(i);
		allRelocationRecords->addLast(newSec);
	}
	return true;
}


bool Linker::checkSymbol(Symbol* symbol) {
	int allSymbolsListSize = allSymbols->getSize(); 
	for (int i = 0; i < allSymbolsListSize; i++) {
		Symbol* sym = allSymbols->getWithoutRemovingFromList(i);
		
		if ((sym->getName() == symbol->getName()) && (sym->getIsGlobal() && !(sym->getSection() == 0)) && (symbol->getIsGlobal() && !(symbol->getSection() == 0))) {
			return false; 
		
		}
	}
	return true;
}

Symbol* Linker::getSymbolForSection(string name, string filename) {
	int symbolTableSize = allSymbols->getSize();
	for (int i = 0; i < symbolTableSize; i++) {
		if ((allSymbols->getWithoutRemovingFromList(i)->getName() == name) && (allSymbols->getWithoutRemovingFromList(i)->getFilename() == filename)) return allSymbols->getWithoutRemovingFromList(i);
	}
	return nullptr;
}

bool Linker::checkIfSectionAlreadyExists(string name) {
	int linkerSectionsSize = linkerSections->getSize();
	for (int i = 0; i < linkerSectionsSize; i++) {
		if (linkerSections->getWithoutRemovingFromList(i)->getName() == name) return true;
	}
	return false;
}


void Linker::changeSectionsOffset() {
	int sectionsSize = allSections->getSize();
	for (int i = 0; i < sectionsSize; i++) {
		Section* section = allSections->getWithoutRemovingFromList(i);
		if (section->getName() != "UND") {
			if (getSymbolForSection(section->getName(), section->getFilename()) != nullptr) {
				Symbol* changingOffset = getSymbolForSection(section->getName(), section->getFilename());
				changingOffset->setOffset(memory);
			}
			memory += section->getWholeSectionSize();
		}
	}

}

string Linker::padNumber(string number, int digits) {
	if (number.length() >= digits) {
		return number;
	}
	else {
		string newString = std::string(digits - number.length(), 'X') + number;
		return newString;
	}
}

int Linker::getNumberOfLiterals(string name, string filename) {
	int sectionSize = allSections->getSize();
	for (int i = 0; i < sectionSize; i++) {
		if (allSections->getWithoutRemovingFromList(i)->getName() == name && allSections->getWithoutRemovingFromList(i)->getFilename() == filename) {
			return allSections->getWithoutRemovingFromList(i)->getLiteralPool()->getListOfLiterals()->getSize();
		}
	}
}

void Linker::pasteSections() {
	int sectionsSize = allSections->getSize();
	for (int i = 0; i < sectionsSize; i++) {
		if (!checkIfSectionAlreadyExists(allSections->getWithoutRemovingFromList(i)->getName())) {
			Section* section1 = allSections->getWithoutRemovingFromList(i);
			if (section1->getLiteralPoolSize() > 0) {
				int m = getNumberOfLiterals(section1->getName(), section1->getFilename());
				string poolReplace = padNumber("X", m*8);
				section1->insertIntoBytes(poolReplace);
			}
				for (int j = i + 1; j < sectionsSize; j++) {
					Section* section2 = allSections->getWithoutRemovingFromList(j);
					if (section1->getName() == section2->getName()) {

						if (section2->getLiteralPoolSize() > 0) { //postoji pool
							section1->setLiteralPoolSize(section1->getLiteralPoolSize() + section2->getLiteralPoolSize());
						}
						vector<string> data = section2->getBytes();
						for (string pom : data) section1->insertIntoBytes(pom);
						if (section2->getLiteralPoolSize() > 0) {
							int n = getNumberOfLiterals(section2->getName(), section2->getFilename());
							string poolReplace2 = padNumber("X", n*8);
							section1->insertIntoBytes(poolReplace2);
						}


						if (section2->getLiteralPoolSize() > 0) {
							List<Literal>* lit2 = section2->getLiteralPool()->getListOfLiterals();
							for (int k = 0; k < lit2->getSize(); k++) {
								section1->getLiteralPool()->addLiteralToList(lit2->getWithoutRemovingFromList(k));
							}
						}

						section1->setLiteralPoolSize(section1->getLiteralPoolSize() + section2->getLiteralPoolSize());
						section1->setWholeSectionSize(section1->getWholeSectionSize() + section2->getWholeSectionSize());

					}
				}
				linkerSections->addLast(section1);
			}
		
	}
}

int Linker::getSectionNum(string sec, string filename) {
	int size = allSections->getSize();
	for (int i = 0; i < size; i++) {
		if (allSections->getWithoutRemovingFromList(i)->getName() == sec && allSections->getWithoutRemovingFromList(i)->getFilename() == filename) {
			return allSections->getWithoutRemovingFromList(i)->getIdSection();
		}
	}
	return -1;
}

int Linker::hexToDec(string hex_val) {
	int dec_val;
	stringstream stream;
	stream << hex << hex_val;
	stream >> dec_val;
	return dec_val;
}

string Linker::getSymbolNameForSectionAndOffset(int sectionName, int offset) {
	
	int size = allSymbols->getSize();
	for (int i = 0; i < size; i++) {
		if (allSymbols->getWithoutRemovingFromList(i)->getOffset() == offset && allSymbols->getWithoutRemovingFromList(i)->getSection()== sectionName) {
			return allSymbols->getWithoutRemovingFromList(i)->getName();
		}
	}
	return "";
}

int Linker::getOffsetSection(string section, string filename) {
	int size = allSymbols->getSize();
	
	for (int i = 0; i < size; i++) {
		if (allSymbols->getWithoutRemovingFromList(i)->getName() == section && allSymbols->getWithoutRemovingFromList(i)->getFilename() == filename) {
			return allSymbols->getWithoutRemovingFromList(i)->getOffset();
		}
	}

}

int Linker::getSectionNumForSymbol(string name) {
	int size = allSymbols->getSize();
	for (int i = 0; i < size; i++) {
		if (allSymbols->getWithoutRemovingFromList(i)->getName() == name) {
			return allSymbols->getWithoutRemovingFromList(i)->getSection();
		}
	}
	return -2;
}

Symbol* Linker::definedSymbol(string name) {
	int size = allSymbols->getSize();
	for (int i = 0; i < size; i++) {
		if (allSymbols->getWithoutRemovingFromList(i)->getName() == name && allSymbols->getWithoutRemovingFromList(i)->getSection() != 0) {
			return allSymbols->getWithoutRemovingFromList(i);
		}
	}
	return nullptr;
}

bool Linker::checkIfSymbolInPom(string name) {

	int pomListSize = pomList->getSize();
	for (int i = 0; i < pomListSize; i++) {

		if (pomList->getWithoutRemovingFromList(i)->getName() == name) return true;
	}
	return false;

}

void Linker::resolveSymbolsInSymbolTable() {
	int size = allSymbols->getSize();
	int sizeRel = allRelocationRecords->getSize();
	for (int i = 0; i < size; i++) {
		Symbol* sym = allSymbols->getWithoutRemovingFromList(i);

		for (int j = 0; j < sizeRel; j++) {
			RelocationRecord* rel = allRelocationRecords->getWithoutRemovingFromList(j);
			int offset = hexToDec(rel->getAddend());

			string symName = getSymbolNameForSectionAndOffset(getSectionNum(rel->getSymbolRbr(), rel->getFilename()), offset);
			if (symName != "") {
				if (symName == sym->getName() && (!checkIfSymbolInPom(sym->getName())) && sym->getOffset() != -1) {
					int offsetSec = getOffsetSection(rel->getSymbolRbr(), rel->getFilename());
					int newOffset = offsetSec + offset;
					sym->setOffset(newOffset);
			
					pomList->addLast(sym);
				}
			}
			
		}
		
		if (sym->getSection() == 0 && sym->getName() != "UND" && (!checkIfSymbolInPom(sym->getName()))) {
			Symbol* definedSymbol1 = definedSymbol(sym->getName());
			
			if (definedSymbol1 != nullptr) {
				int offsetSec = getOffsetSection(definedSymbol1->getSectionName(), definedSymbol1->getFilename());
				sym->setOffset(definedSymbol1->getOffset() + offsetSec);
				sym->setSectionName(definedSymbol1->getSectionName());
				definedSymbol1->setOffset(definedSymbol1->getOffset() + offsetSec);
			}
			else {
				cout << "LINKERSKA GRESKA" << sym->getName() <<endl;
			}
		}
	}


}

Symbol* Linker::getSymbolForSectionNumAndFileName(int sec, string filename) {
	int size = allSymbols->getSize();
	for (int i = 0; i < size; i++) {
		if (allSymbols->getWithoutRemovingFromList(i)->getSection() == sec && allSymbols->getWithoutRemovingFromList(i)->getFilename() == filename) {
			return allSymbols->getWithoutRemovingFromList(i);
		}
	}
	return nullptr;
}

LiteralPool* Linker::getLitPoolForSection(string section, string filename) {
	int size = allSections->getSize();
	for (int i = 0; i < size; i++) {
		if (allSections->getWithoutRemovingFromList(i)->getFilename() == filename && allSections->getWithoutRemovingFromList(i)->getName() == section) {
			return allSections->getWithoutRemovingFromList(i)->getLiteralPool();
		}
	}
	return nullptr;
}

Symbol* Linker::symbolSection(string name, int sec, string filename) {
	int size = allSymbols->getSize();
	for (int i = 0;i < size; i++) {
		if (allSymbols->getWithoutRemovingFromList(i)->getName() == name && allSymbols->getWithoutRemovingFromList(i)->getFilename() == filename && allSymbols->getWithoutRemovingFromList(i)->getSection() == sec) {
			return allSymbols->getWithoutRemovingFromList(i);
		}
	}
	return nullptr;
}

Symbol* Linker::getSymbolByName(string name) {
	int size = allSymbols->getSize();
	for (int i = 0;i < size; i++) {
		if (allSymbols->getWithoutRemovingFromList(i)->getName() == name  && allSymbols->getWithoutRemovingFromList(i)->getSectionName() != "UND") {
			return allSymbols->getWithoutRemovingFromList(i);
		}
	}
	return nullptr;
}

Symbol* Linker::getSymbolByNameAndFilename(string name, string filename) {
	int size = allSymbols->getSize();
	for (int i = 0;i < size; i++) {
		if (allSymbols->getWithoutRemovingFromList(i)->getName() == name && allSymbols->getWithoutRemovingFromList(i)->getFilename() == filename) {
			return allSymbols->getWithoutRemovingFromList(i);
		}
	}
	return nullptr;
}


Section* Linker::getSectionByNameAndFilename(string name, string filename) {
	int size = allSections->getSize();
	for (int i = 0;i < size; i++) {
		if (allSections->getWithoutRemovingFromList(i)->getName() == name && allSections->getWithoutRemovingFromList(i)->getFilename() == filename) {
			return allSections->getWithoutRemovingFromList(i);
		}
	}
	return nullptr;
}

void Linker::fixRelocs() {
	int size = allRelocationRecords->getSize();

	
	for (int i = 0; i < size; i++) {
		
		RelocationRecord* record = allRelocationRecords->getWithoutRemovingFromList(i);
		string offset = record->getOffset();
		int offsetDec = hexToDec(offset);

		string section = record->getSection();
		LiteralPool* litPool = getLitPoolForSection(section, record->getFilename());

		string symName = litPool->getSymbolName(offsetDec);
		if (litPool->checkIfLiteralAlreadyExists(symName)) {
			Symbol* sym = getSymbolByName(symName);
	
			if (sym != nullptr) {
				
				int offsetSym = sym->getOffset();
				string hex = decToHex(offsetSym);
				litPool->setOffset(to_string(offsetSym), symName);
			}
			allRelocationRecords->getWithoutRemovingFromList(i)->setRazresen(true);
		}
		
	}
	
}

bool Linker::checkIfSymbolAlreadyExists(string name) {
	int linkerSymbolsSize = linkerSymbols->getSize();
	for (int i = 0; i < linkerSymbolsSize; i++) {
		if (linkerSymbols->getWithoutRemovingFromList(i)->getName() == name) return true;
	}
	return false;
}


void Linker::washSymbols() {
	int size = allSymbols->getSize();
	for (int i = 0; i < size; i++) {
		string symName = allSymbols->getWithoutRemovingFromList(i)->getName();
		if (symName == "UND") continue;
		if (checkIfSymbolAlreadyExists(symName)) continue;
		linkerSymbols->addLast(allSymbols->getWithoutRemovingFromList(i));
	}

} 

int Linker::getSectionNumberByName(string name) {
	int size1 = linkerSections->getSize();
	for (int i = 0; i < size1; i++) {
		if (linkerSections->getWithoutRemovingFromList(i)->getName() == name) return linkerSections->getWithoutRemovingFromList(i)->getIdSection();
	}
}

void Linker::setRbrSymbolAndSection() {
	int size = linkerSymbols->getSize();
	for (int i = 0;i < size; i++) {
		linkerSymbols->getWithoutRemovingFromList(i)->setNumber(i);
	}
	int size1 = linkerSections->getSize();
	for (int i = 0; i < size1; i++) {
		linkerSections->getWithoutRemovingFromList(i)->setIdSection(i);
	}
}


void Linker::fixSectionForSymbol() {
	int size = linkerSymbols->getSize();
	for (int i = 0;i < size; i++) {
		string name = linkerSymbols->getWithoutRemovingFromList(i)->getSectionName();
		
		int secNum = getSectionNumberByName(name);
		linkerSymbols->getWithoutRemovingFromList(i)->setSection(secNum);
	}
}


void Linker::printLinkerTables() {
	for (int i = 0; i < linkerSymbols->getSize(); i++) {
		cout << linkerSymbols->getWithoutRemovingFromList(i)->getName() << " ";
		cout << linkerSymbols->getWithoutRemovingFromList(i)->getOffset() << " ";
		cout << linkerSymbols->getWithoutRemovingFromList(i)->getSection() << " ";
		cout << linkerSymbols->getWithoutRemovingFromList(i)->getSectionName() << " ";
		cout << linkerSymbols->getWithoutRemovingFromList(i)->getIsGlobal();
		cout << endl;
	}

	for (int i = 0; i < linkerSections->getSize(); i++) {
		cout << linkerSections->getWithoutRemovingFromList(i)->getName();
		cout << linkerSections->getWithoutRemovingFromList(i)->getWholeSectionSize();
		cout << endl;
		cout << "LITERAL POOL" << endl;
		LiteralPool* litPool = linkerSections->getWithoutRemovingFromList(i)->getLiteralPool();
		litPool->printPool();
		cout << endl;
	}
}

Section* Linker::getSectionByName(string name) {
	int size = linkerSections->getSize();
	for (int i = 0;i < size; i++) {
		if (linkerSections->getWithoutRemovingFromList(i)->getName() == name) return linkerSections->getWithoutRemovingFromList(i);
	}
}

string Linker::removeHexPrefix(string line) {
	if (line.size() >= 2 && line.substr(0, 2) == "0x") {
		return line.substr(2); // Return the substring without the "0x" prefix
	}
	return line;
}

string Linker::toUpperCase(string line) {
	std::string uppercaseHex = line;
	for (char& ch : uppercaseHex) {
		ch = std::toupper(ch);
	}
	return uppercaseHex;
}

string Linker::addTwoHex(string hexOne, string hexTwo) {
	istringstream iss1(hexOne);
	istringstream iss2(hexTwo);

	unsigned long long num1, num2;

	iss1 >> std::hex >> num1;
	iss2 >> std::hex >> num2;

	unsigned long long result = num1 + num2;
	std::ostringstream oss;
	oss << uppercase << hex << std::setw(hexTwo.length()) << std::setfill('0') << result;
	std::string hex_result = oss.str();

	return hex_result;
};

string Linker::fixAddr(string hex1, string hex2) {

	std::stringstream ss;
	ss << std::hex << std::stoull(hex1, nullptr, 16) + std::stoull(hex2, nullptr, 16);
	return toUpperCase(ss.str());
}

Symbol* Linker::getSymbol(string name) {
	int size = linkerSymbols->getSize();
	for (int i = 0; i < size; i++) {
		if (linkerSymbols->getWithoutRemovingFromList(i)->getName() == name) {
			return linkerSymbols->getWithoutRemovingFromList(i);
		}
	}
	return nullptr;
}

string Linker::getNewAddresForSection(string name) {
	
	int size = secAddr->getSize();
	for (int i = 0; i < size; i++) {
		if (secAddr->getWithoutRemovingFromList(i)->sectionName == name) {
			return secAddr->getWithoutRemovingFromList(i)->newAddress;
			
		}
	}
}

std::string Linker::decToHex(int decimal) {
	std::stringstream ss;
	ss << std::uppercase << std::hex << decimal;
	std::string hex = ss.str();
	return hex;
}

int Linker::getOffsetForSection(string name) {
	int size = linkerSymbols->getSize();
	for (int i = 0; i < size; i++) {
		if (linkerSymbols->getWithoutRemovingFromList(i)->getName() == name) return linkerSymbols->getWithoutRemovingFromList(i)->getOffset();
	}
}

void Linker::fixPoolOffset() {

	int size = linkerSections->getSize();
	for (int i = 0; i < size; i++) {
		
		if (linkerSections->getWithoutRemovingFromList(i)->getLiteralPoolSize() > 0) {
			
			LiteralPool* litPool = linkerSections->getWithoutRemovingFromList(i)->getLiteralPool();
			for (int j = 0; j < litPool->getListOfLiterals()->getSize(); j++) {
				
				if (getSymbol(litPool->getListOfLiterals()->getWithoutRemovingFromList(j)->literalVal) != nullptr) {
					
					Symbol* sym = getSymbol(litPool->getListOfLiterals()->getWithoutRemovingFromList(j)->literalVal);
					string newAddresForSection = getNewAddresForSection(sym->getSectionName());
					int offset = getOffsetForSection(sym->getSectionName());
					
					int newOffs = sym->getOffset() - offset;
					
					if (newOffs >= 0) {
						
						string hex = decToHex(newOffs);

						string newOffset = fixAddr(newAddresForSection, hex);
			
						litPool->setOffset(newOffset, sym->getName());
					}
					else {
					
						string hex = decToHex(sym->getOffset());
						string newOffset = addTwoHex(newAddresForSection, hex);
						litPool->setOffset(newOffset, sym->getName());
					}
				}
			}
		}
	}
	//for (int i = 0; i < size; i++) {
	//	cout << "LITERAL POOL: " << endl;
	//	cout << linkerSections->getWithoutRemovingFromList(i)->getName() << endl;
	//	linkerSections->getWithoutRemovingFromList(i)->getLiteralPool()->printPool();
	//}
}

std::string Linker::replaceFirstOccurrence(const std::string& str, const std::string& oldString, const std::string& newString) {
	std::string result = str;
	size_t pos = result.find(oldString);
	if (pos != std::string::npos) {
		std::string littleEndianNewString = convertToLittleEndian(newString);
		result.replace(pos, oldString.length(), littleEndianNewString);
	}
	return result;
}

Symbol* Linker::getSymbolLinker(string name) {
	int size = linkerSymbols->getSize();
	for (int i = 0; i < size; i++) {
		if (linkerSymbols->getWithoutRemovingFromList(i)->getName() == name) {
			return linkerSymbols->getWithoutRemovingFromList(i);
		}
	}
	return nullptr;
}

void Linker::fixByteCodeForSection(const std::string result, const std::string name) {


	int size = linkerSections->getSize();
	for (int j = 0; j < size; j++) {
		if (linkerSections->getWithoutRemovingFromList(j)->getName() == name) {
			std::vector<std::string> byteCode = linkerSections->getWithoutRemovingFromList(j)->getBytes();
			std::string pom;
			for (const std::string& data : byteCode) {
				pom += data;
			}
		
			pom = replaceFirstOccurrence(pom, "????????", result);

			linkerSections->getWithoutRemovingFromList(j)->setBytes(pom);  // Add the updated byte code
		}
	}
}

void Linker::fixWord() {
	string result = "";
	List<RelocationRecord>* newList = new List<RelocationRecord>;

	
	if (allRelocationRecords->getSize() > 0) {

		for (int i = 0; i < linkerSections->getSize(); i++) {
			for (int j = 0; j < allRelocationRecords->getSize(); j++) {
				
				if (allRelocationRecords->getWithoutRemovingFromList(j)->getSection() == linkerSections->getWithoutRemovingFromList(i)->getName() && !allRelocationRecords->getWithoutRemovingFromList(j)->getRazresen()) {
					newList->addLast(allRelocationRecords->getWithoutRemovingFromList(j));

					//relocs for one section

					while (newList->getSize() > 0) {
						RelocationRecord* record = newList->removeFirst();
						string sectionName = record->getSection();
						string prepravljena = record->getSymbolRbr();
						string addend = record->getAddend();
						int secNum = getSectionNum(prepravljena, record->getFilename());
						if (secNum > 0) {
							Symbol* sym = getSymbolForSectionNumAndFileName(secNum, record->getFilename());
							Symbol* sym1 = getSymbolByName(sym->getName());
							Section* sec = getSectionByName(prepravljena);
							if (sym != nullptr) {

								string newAddr = "0";

								for (int j = 0; j < secAddr->getSize(); j++) {
									if (secAddr->getWithoutRemovingFromList(j)->sectionName == sym->getSectionName()) {
										newAddr = secAddr->getWithoutRemovingFromList(j)->newAddress;
										break;
									}
								}
									string sum = fixAddr(newAddr, addend);


									fixByteCodeForSection(sum, sectionName);
								
							}
						
						}
						else {

							Symbol* symbol = getSymbolLinker(prepravljena);

							if (symbol != nullptr) {

								string newAddr = "0";

								int offset = getOffsetForSection(symbol->getSectionName());

								int newOffs = symbol->getOffset() - offset;
								
								if (newOffs >= 0) {
									string hexOffset = decToHex(newOffs);
									for (int j = 0; j < secAddr->getSize(); j++) {

										if (secAddr->getWithoutRemovingFromList(j)->sectionName == symbol->getSectionName()) {

											newAddr = secAddr->getWithoutRemovingFromList(j)->newAddress;
											break;
										}
									}
				
									string sum = fixAddr(newAddr, hexOffset);

									fixByteCodeForSection(sum, sectionName);
								}
							}

					
						}
					}
				}
			}
		}
	}

}

void Linker::setNewAddresses(string addr1, string addr2, string sec1, string sec2) {
	vector<string> byteCode1;
	vector<string> byteCode2;
	Section* section1 = getSectionByName(sec1);
	Section* section2 = getSectionByName(sec2);
	byteCode1 = section1->getBytes();
	byteCode2 = section2->getBytes();

	SectionAddr* secAddrNew1 = new SectionAddr(addr1, sec1);
	SectionAddr* secAddrNew2 = new SectionAddr(addr2, sec2);
	secAddr->addLast(secAddrNew1);
	secAddr->addLast(secAddrNew2);
	string inputString;
	for (string data : byteCode1) {
		inputString.append(data);
	}
	int length = inputString.length();

	for (int i = 0; i < length; i += 16) {
				
		addr1 = fixAddr(addr1, "8");
	
	}
	
	string inputString2;
	for (string data : byteCode2) {
		inputString2.append(data);
	}
	int length2 = inputString2.length();

	for (int i = 0; i < length2; i += 16) {
		addr2 = fixAddr(addr2, "8");
		
	}
	if (getSectionByName(sec2)->getLiteralPoolSize() > 0) {
		int litPoolSize = getSectionByName(sec2)->getLiteralPoolSize();
		string hex = decToHex(litPoolSize);
		addr2 = fixAddr(addr2, hex);
	}
	//other sections

	int size = linkerSections->getSize();
	for (int i = 0; i < size; i++) {

		if (linkerSections->getWithoutRemovingFromList(i)->getName() != sec1 && linkerSections->getWithoutRemovingFromList(i)->getName() != sec2 && linkerSections->getWithoutRemovingFromList(i)->getName() != "UND") {
			SectionAddr* secAddrN = new SectionAddr(addr2, linkerSections->getWithoutRemovingFromList(i)->getName());
			secAddr->addLast(secAddrN);
			string inputString3;
			vector<string> dataCode = linkerSections->getWithoutRemovingFromList(i)->getBytes();
			for (string data : dataCode) {
				inputString3.append(data);
			}
			int length3 = inputString3.length();

			for (int i = 0; i < length3; i += 16) {
				addr2 = fixAddr(addr2, "8");
			}
			
		}
	}

	/*for (int i = 0; i < secAddr->getSize(); i++) {
		cout << secAddr->getWithoutRemovingFromList(i)->sectionName << " " << secAddr->getWithoutRemovingFromList(i)->newAddress << endl;
	}*/

	fixPoolOffset();
	fixWord();
}


int Linker::countPairsOfX(const std::string& str, char target) {
	int count = 0;
	bool isInPair = false;

	for (char c : str) {
		if (c == target) {
			if (!isInPair) {
				isInPair = true;
				count++;
			}
			else {
				isInPair = false;
			}
		}
		else {
			isInPair = false;
		}
	}

	return count;
}

std::string Linker::convertToLittleEndian(const std::string& hexString) {
	std::string littleEndian;
	size_t length = hexString.length();

	// Process the string in pairs of two characters
	for (size_t i = 0; i < length; i += 2) {
		std::string pair = hexString.substr(i, 2);  // Get the current pair

		// Insert the pair at the beginning of the result string
		littleEndian.insert(0, pair);
	}

	return littleEndian;
}


vector<string> Linker::findXXX(const std::vector<std::string>& strings, char target) {
	std::vector<std::string> results;
	for (const std::string& str : strings) {
		std::string temp;
		std::size_t startPos = 0;
		std::size_t endPos = 0;
		while ((startPos = str.find(target, endPos)) != std::string::npos) {
			endPos = str.find_first_not_of(target, startPos);
			if (endPos == std::string::npos) {
				temp = str.substr(startPos);
			}
			else {
				temp = str.substr(startPos, endPos - startPos);
			}
			results.push_back(temp);
		}
	}
	return results;
}

std::string Linker::padStringToEightCharacters(const std::string& str) {
	int numChars = str.length();
	if (numChars >= 8) {
		return str;
	}
	else {
		int numZeros = 8 - numChars;
		std::string paddedStr = std::string(numZeros, '0') + str;
		return paddedStr;
	}
}


vector<string> Linker::fixLiteralsInPoolForByteCode(string name) {
	int size = linkerSections->getSize();
	vector<string> result;
	smatch val;
	for (int i = 0;i < size; i++) {
		if (linkerSections->getWithoutRemovingFromList(i)->getName() == name) {
			vector<string> bytesForSection = linkerSections->getWithoutRemovingFromList(i)->getBytes();
			vector<string> target = findXXX(bytesForSection, 'X');
			int startIndex = 0;
			int endIndex = 0;
			for (string data : target) {
				
				int n = countPairsOfX(data, 'X');
				int num = n / 4;
				endIndex = startIndex + num;

				List<Literal>* list = linkerSections->getWithoutRemovingFromList(i)->getLiteralPool()->getListOfLiterals();
				

					for (startIndex; startIndex < endIndex; startIndex++) {

						if (regex_match(list->getWithoutRemovingFromList(startIndex)->literalVal, val, literal)) {

							
							string literalValue = list->getWithoutRemovingFromList(startIndex)->literalVal;
					
							literalValue = padStringToEightCharacters(literalValue);
							string littleEndian = convertToLittleEndian(literalValue);

							result.push_back(littleEndian);
						}
						else {

							string offset = list->getWithoutRemovingFromList(startIndex)->offset;
						
							offset = padStringToEightCharacters(offset);
							string littleEndianSim = convertToLittleEndian(offset);

							result.push_back(littleEndianSim);
						}

					
				}
					startIndex = endIndex;
					
			
				
			}
	
			
			return result;
		}
	}

}

std::string Linker::removeX(const std::string& inputString) {
	std::string updatedString = inputString;
	updatedString.erase(std::remove(updatedString.begin(), updatedString.end(), 'X'), updatedString.end());
	return updatedString;
}

std::string Linker::replaceX(const std::string& str, const std::string& newString) {
	std::string updatedStr = str;
	size_t posX = 0;
	if ((posX = updatedStr.find("X", posX)) != std::string::npos) {
		updatedStr.replace(posX, 1, newString);
		posX += newString.length();
	}
	updatedStr = removeX(updatedStr);
	return updatedStr;
}

std::string Linker::replaceXsWithNewChars(const std::string& strWithXs, const std::string& strWithoutXs) {

	std::string result = strWithXs;
	size_t newXIndex = 0;
	for (size_t i = 0; i < strWithXs.length(); ++i) {
		if (strWithXs[i] == 'X') {
			result[i] = strWithoutXs[newXIndex % strWithoutXs.length()];
			newXIndex++;
		}
	}
	return result;
}

void Linker::printHexaFile(string addr1, string addr2, string sec1, string sec2, string filename) {
	ofstream file;
	file.open(filename);

	vector<string> byteCode1;
	vector<string> byteCode2;
	vector<string> newBytes;
	Section* section1 = getSectionByName(sec1);
	Section* section2 = getSectionByName(sec2);
	byteCode1 = section1->getBytes();
	byteCode2 = section2->getBytes();
	addr1 = removeHexPrefix(addr1);
	addr2 = removeHexPrefix(addr2);

	if (section1->getLiteralPoolSize() > 0) {
		
		newBytes = fixLiteralsInPoolForByteCode(sec1);
	}

	string newStr;
	for (string blabla : newBytes) {
		newStr += blabla;
	}
	string oldStr;
	for (string blabla : byteCode1) {
		oldStr += blabla;
	}
	string inputString;
	inputString = replaceXsWithNewChars(oldStr, newStr);
	
	/*for (string data : byteCode1) {
		inputString.append(data);
	}*/
	int length = inputString.length();;

	for (int i = 0; i < length; i += 16) {
		file << addr1 << ": ";
		for (int j = i; j < i + 16 && j < length; j += 2) {
			file << inputString.substr(j, 2) << " ";
		}
		addr1 = fixAddr(addr1, "8");
		file << std::endl;
	}
	file << std::endl;
	string newStr2 = "";

	if (section2->getLiteralPoolSize() > 0) {

		newBytes = fixLiteralsInPoolForByteCode(sec2);
	
		for (string blabla : newBytes) {
			newStr2 += blabla;
		}
	}
	

	string oldStr2;
	for (string blabla : byteCode2) {
		oldStr2 += blabla;
	}

		
	string inputString2;
	inputString2 = replaceXsWithNewChars(oldStr2, newStr2);

	int length2 = inputString2.length();

	for (int i = 0; i < length2; i += 16) {
		file << addr2 << ": ";
		for (int j = i; j < i + 16 && j < length2; j += 2) {
			file << inputString2.substr(j, 2) << " ";
		}
		addr2 = fixAddr(addr2, "8");
		file << std::endl;
	}
	if (getSectionByName(sec2)->getLiteralPoolSize() > 0) {
		int litPoolSize = getSectionByName(sec2)->getLiteralPoolSize();
		string hex = decToHex(litPoolSize);
		addr2 = fixAddr(addr2, hex);
	}
	file << std::endl;
	//other sections

	int size = linkerSections->getSize();
	for (int i = 0; i < size; i++) {
		
		if (linkerSections->getWithoutRemovingFromList(i)->getName() != sec1 && linkerSections->getWithoutRemovingFromList(i)->getName() != sec2 && linkerSections->getWithoutRemovingFromList(i)->getName() != "UND") {
			string inputString3;

			vector<string> dataCode = linkerSections->getWithoutRemovingFromList(i)->getBytes();
			string oldStr3;
			for (string data : dataCode) {
				oldStr3 += data;
			}
			string newStr3 = "";
			if (linkerSections->getWithoutRemovingFromList(i)->getLiteralPoolSize() > 0) {


				newBytes = fixLiteralsInPoolForByteCode(linkerSections->getWithoutRemovingFromList(i)->getName());
				
				for (string data : newBytes) {
					newStr3 += data;
				}
			}

	

			inputString3 = replaceXsWithNewChars(oldStr3, newStr3);
		
			int length3 = inputString3.length();

			for (int i = 0; i < length3; i += 16) {
				file << addr2 << ": ";
				for (int j = i; j < i + 16 && j < length3; j += 2) {
					file << inputString3.substr(j, 2) << " ";
				}
				addr2 = fixAddr(addr2, "8");
				file << std::endl;

			}

			file << std::endl;

		}
	}

	file.close();

}
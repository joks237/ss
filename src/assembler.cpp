#include "../inc/assembler.h"
#include <iostream>
#include <fstream>
#include <set>


int Assembler::locationCounter = 0;
int Assembler::currentSection = 0;
string Assembler::currentSectionName = "UND";
//literali i simboli

regex literal("(0x[0-9a-fA-F]+|[0-9]+)");
regex symbol("(([a-zA-Z][a-zA-Z0-9_]*))");

// assembly directives
regex global(".global ([a-zA-Z][a-zA-Z0-9_]*)(,? ([a-zA-Z][a-zA-Z0-9_]*))*");
regex externn(".extern ([a-zA-Z][a-zA-Z0-9_]*)(,? ([a-zA-Z][a-zA-Z0-9_]*))*");
regex section(".section ([a-zA-Z][a-zA-Z0-9_]*)");
regex word(".word ([a-zA-Z0-9_]*|((0x[0-9a-fA-F]+|[0-9]+)))(,? ([a-zA-Z0-9_]*|((0x[0-9a-fA-F]+|[0-9]+))))*");
regex skip(".skip (0x[0-9a-fA-F]+|[0-9]+)");
regex endd(".end");
//instructions without parameters
regex withoutParam("halt|int|iret|ret");
//call instruction
regex callLiteral("(call) (0x[0-9a-fA-F]+|[0-9]+)");
regex callSymbol("(call) ([a-zA-Z][a-zA-Z0-9_]*)");
//arithmetic and logical instructions
regex arrAndLogInstr("(xchg|add|sub|mul|div|and|or|xor|shl|shr) %(r([0-9]|1[0-5])|sp|pc), %(r([0-9]|1[0-5])|sp|pc|sp|pc)");
//push, pop and not instruction (instructions with one parameter)
regex notPushPopInstr("(not|push|pop) %(r([0-9]|1[0-5])|sp|pc)");
//jump instruction
regex jumpLiteralValue("(jmp) (0x[0-9a-fA-F]+|[0-9]+)");
regex jumpSymbolValue("(jmp) ([a-zA-Z][a-zA-Z0-9_]*)");
//branch instructions
regex branchLiteral("(beq|bne|bgt) %(r([0-9]|1[0-5])|sp|pc), %(r([0-9]|1[0-5])|sp|pc), (0x[0-9a-fA-F]+|[0-9]+)");
regex branchSymbol("(beq|bne|bgt) %(r([0-9]|1[0-5])|sp|pc), %(r([0-9]|1[0-5])|sp|pc), ([a-zA-Z][a-zA-Z0-9_]*)");
//load instructions
regex loadLiteralValue("(ld) \\$(0x[0-9a-fA-F]+|[0-9]+), %(r([0-9]|1[0-5])|sp|pc)");
regex loadSymbolValue("ld \\$([a-zA-Z][a-zA-Z0-9_]+), %(r([0-9]|1[0-5])|sp|pc)");
regex loadLiteralFromMemory("(ld) (0x[0-9a-fA-F]+|[0-9]+), %(r([0-9]|1[0-5])|sp|pc)");
regex loadSymbolFromMemory("(ld) ([a-zA-Z][a-zA-Z0-9_]*), %(r([0-9]|1[0-5])|sp|pc)");
regex loadFromReg("(ld) %(r([0-9]|1[0-5])|sp|pc|status|handler|cause), %(r([0-9]|1[0-5])|sp|pc)");
regex loadFromMemoryReg("(ld) (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause)\\]), %(r([0-9]|1[0-5])|sp|pc)");
regex loadFromMemoryWithOffsetLiteral("(ld) (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (0x[0-9a-fA-F]+|[0-9]+)\\]), %(r([0-9]|1[0-5])|sp|pc)");
regex loadFromMemoryWithOffsetSymbol("(ld) (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (([a-zA-Z][a-zA-Z0-9_]*))\\]), %(r([0-9]|1[0-5])|sp|pc)");
//store instructions
regex storeLiteralValue("(st) %(r([0-9]|1[0-5])|sp|pc), \\$(0x[0-9a-fA-F]+|[0-9]+)");
regex storeSymbolValue("(st) %(r([0-9]|1[0-5])|sp|pc), \\$([a-zA-Z][a-zA-Z0-9_]*)");
regex storeLiteralFromMemory("(st) %(r([0-9]|1[0-5])|sp|pc), (0x[0-9a-fA-F]+|[0-9]+)");
regex storeSymbolFromMemory("(st) %(r([0-9]|1[0-5])|sp|pc), ([a-zA-Z][a-zA-Z0-9_]*)");
regex storeFromReg("(st) %(r([0-9]|1[0-5])|sp|pc), %(r([0-9]|1[0-5])|sp|pc|status|handler|cause)");
regex storeFromMemoryReg("(st) %(r([0-9]|1[0-5])|sp|pc), (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause)\\])");
regex storeFromMemoryWithOffsetLiteral("(st) %(r([0-9]|1[0-5])|sp|pc), (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (0x[0-9a-fA-F]+|[0-9]+)\\])");
regex storeFromMemoryWithOffsetSymbol("(st) %(r([0-9]|1[0-5])|sp|pc), (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (([a-zA-Z][a-zA-Z0-9_]*))\\])");
//read and write control status register
regex readControlStatusReg("(csrrd) %(status|handler|cause), %(r([0-9]|1[0-5])|sp|pc)");
regex writeControlStatusReg("(csrwr) %(r([0-9]|1[0-5])|sp|pc), %(status|handler|cause)");
//pomocni regexi za load i store
regex litVal("\\$(0x[0-9a-fA-F]+|[0-9]+|[0-1]+b)");
regex symVal("\\$(([a-zA-Z][a-zA-Z0-9_]*))");
regex reg("%(r([0-9]|1[0-5])|sp|pc|status|handler|cause)");
regex fromMemoryReg("\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause)\\]");
regex fromMemoryWithOffsetLiteral("\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (0x[0-9a-fA-F]+|[0-9]+)\\]");
regex fromMemoryWithOffsetSymbol("\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (([a-zA-Z][a-zA-Z0-9_]*))\\]");
regex label("^[A-Za-z][A-Za-z0-9_]*:$");
regex directives(".global|.extern|.skip|.word|.end|.section|halt|int|ret|iret|call|ld|st|csrrd|csrwr|add|sub|mul|div|and|or|xor|not|xchg|shl|shr|push|pop|jmp|beq|bne|bgt");
regex hexRegex("^0x[0-9A-Fa-f]+$");
regex binaryRegex("^[01]+b$");
regex onlyDirectiveRegex(".global|.extern|.section|.word|.skip|.end");
regex onlyInstructionRegex("halt|int|ret|iret|call|ld|st|csrrd|csrwr|add|sub|mul|div|and|or|xor|not|xchg|shl|shr|push|pop|jmp|beq|bne|bgt");
regex specialReg("%status|%handler|%cause");

string Assembler::checkReg(string reg) {
	if (reg == "%r0" || reg == "%status") {
		return "0";
	}
	else if (reg == "%r1" || reg == "%handler") {
		return "1";
	}
	else if (reg == "%r2" || reg == "%cause") {
		return "2";
	}
	else if (reg == "%r3") {
		return "3";
	}
	else if (reg == "%r4") {
		return "4";
	}
	else if (reg == "%r5") {
		return "5";
	}
	else if (reg == "%r6") {
		return "6";
	}
	else if (reg == "%r7") {
		return "7";
	}
	else if (reg == "%r8") {
		return "8";
	}
	else if (reg == "%r9") {
		return "9";
	}
	else if (reg == "%r10") {
		return "A";
	}
	else if (reg == "%r11") {
		return "B";
	}
	else if (reg == "%r12") {
		return "C";
	}
	else if (reg == "%r13") {
		return "D";
	}
	else if (reg == "%r14" || reg == "%sp") {
		return "E";
	}
	else if (reg == "%r15" || reg == "%pc") {
		return "F";
	}
	else return "EXC"; //greska
}


string Assembler::decToHex(string dec_val) {
	stringstream stream;
	int dec_val1 = stoi(dec_val);
	stream << hex << dec_val1;
	return stream.str();
}

string Assembler::decToBinary(int dec_val) {
	bitset<32> binary(dec_val);
	return binary.to_string();
}

int Assembler::hexToDecimal(string hex_val) {
	int dec_val;
	stringstream stream;
	stream << hex << hex_val;
	stream >> dec_val;
	return dec_val;
}

int Assembler::binToDecimal(string bin_val) {
	bitset<32> binary(bin_val);
	int dec_val = binary.to_ulong();
	return dec_val;
}

string Assembler::checkOpCode(string opName, string mod) {
	smatch val;

	if (opName == "halt") {
		return "00";
	}
	else if (opName == "int") {
		return "10";
	}
	else if (opName == "call") {
		return "20";
	} 
	else if (opName == "jmp") {
		return "30";
	} 
	else if (opName == "beq") {
		return "31";
	}
	else if (opName == "bne") {
		return "32";
	}
	else if (opName == "bgt") {
		return "33";
	}
	else if (opName == "xchg") {
		return "40";
	}
	else if (opName == "add") {
		return "50";
	}
	else if (opName == "sub") {
		return "51";
	}
	else if (opName == "mul") {
		return "52";
	}
	else if (opName == "div") {
		return "53";
	}
	else if (opName == "not") {
		return "60";
	}
	else if (opName == "and") {
		return "61";
	}
	else if (opName == "or") {
		return "62";
	}
	else if (opName == "xor") {
		return "63";
	}
	else if (opName == "shl") {
		return "70";
	}
	else if (opName == "shr") {
		return "71";
	}
	else if (opName == "st") {
		if (regex_match(mod, val, fromMemoryWithOffsetLiteral)) return "80";
		else if (regex_match(mod, val, fromMemoryReg)) return "81";
	}
	else if (opName == "ld") {
		if (regex_match(mod, val, reg)) return "90";
		else if (regex_match(mod, val, litVal) || regex_match(mod, val, symVal)) return "91";
	}
	
}

string Assembler::addBlankSpace(string line) {


	std::regex pattern(R"(\[%((r(\d+)|pc|sp|handler|cause|status))\s*\+\s*(\w+)\])");
	std::smatch matches;

	if (std::regex_search(line, matches, pattern)) {
		std::string registerValue = matches[2].str();
		std::string offsetValue = matches[4].str();

		std::string separator = " ";
		if (registerValue == "pc" || registerValue == "sp" || registerValue == "handler" ||
			registerValue == "cause" || registerValue == "status") {
			separator = "";
		}

		if (registerValue == "sp") return "[%"+ registerValue + ' ' + '+' + ' ' + offsetValue + "]";
		else if (registerValue == "pc") return "[%" + registerValue + ' ' + '+' + ' ' + offsetValue + "]";
		else if (registerValue == "handler") return "[%" + registerValue + ' ' + '+' + ' ' + offsetValue + "]";
		else if (registerValue == "cause") return "[%" + registerValue + ' ' + '+' + ' ' + offsetValue + "]";
		else if (registerValue == "status") return "[%" + registerValue + ' ' + '+' + ' ' + offsetValue + "]";
		else return "[%" + registerValue + separator + "+" + separator + offsetValue + "]";
	}


	return line;
}

bool Assembler::checkIfRegexMatch(string line) {
	smatch val;
	string opName = getFirstWord(line);
	string secondWord = getSecondWord(line);
	string thirdWord = getThirdWord(line);
	string fourthWord = getFourthWord(line);

	//secondWord.erase(0, secondWord.find_first_not_of(" \t"));

	std::regex whitespacePattern("^\\s+");
	secondWord = removeWhitespaces(secondWord);
	thirdWord = std::regex_replace(thirdWord, whitespacePattern, "");
	fourthWord = std::regex_replace(fourthWord, whitespacePattern, "");


	/*
	std::cout << opName << endl;
	std::cout << secondWord << endl;
std::cout << thirdWord << endl;
	std::cout << fourthWord << endl;*/

	//check for sections, global operators, extern operators, skip and end

	if (opName == ".global") {
		if (regex_match(line, val, global)) return true;
		else { 
			std::cout << "greska u .global" << endl;
			return false; }
	}
	else if (opName == ".extern") {
		if (regex_match(line, val, externn)) return true;
		else return false;
	}
	else if (opName == ".section") {
		if (regex_match(line, val, section)) return true;
		else { 
			std::cout << "greska u .section" << endl;
			return false; }
	}
	else if (opName == ".word") {
		if (regex_match(line, val, word)) return true;
		else return false;
	}
	else if (opName == ".skip") {
		if (regex_match(line, val, skip)) return true;
		else return false;
	}
	else if (opName == ".end") {
		if (regex_match(line, val, endd)) return true;
		else return false;
	}
	else if (regex_match(line, val, label)) {
		return true;
	}

	if (opName == "halt" || opName == "int" || opName == "iret" || opName == "ret") {
		if (regex_match(line, val, withoutParam)) {
			return true;
		}
		else return false;
	}
	else if (opName == "call") {
		if (isLiteral(secondWord)) {
			if (regex_match(line, val, callLiteral)) {
				return true;
			}
			else return false;
		}
		else if (isSymbol(secondWord)) {
			if (regex_match(line, val, callSymbol)) {
				return true;
			}
			else return false;
		}
	}
	else if (opName == "xchg" || opName == "add" || opName == "sub" || opName == "mul" || opName == "div" || opName == "and" || opName == "or" || opName == "xor" || opName == "shl" || opName == "shr") {
		if (regex_match(line, val, arrAndLogInstr)) {
			return true;
		}
		else return false;
	}
	else if (opName == "not" || opName == "push" || opName == "pop") {
		if (regex_match(line, val, notPushPopInstr)) {
			return true;
		}
		else return false;
	}
	else if (opName == "jmp") {
		if (isLiteral(secondWord)) {
			if (regex_match(line, val, jumpLiteralValue)) {
				return true;
			}
			else return false;
		}
		else if (isSymbol(secondWord)) {
			if (regex_match(line, val, jumpSymbolValue)) {
				return true;
			}
			return false;
		}
	}
	else if (opName == "beq" || opName == "bne" || opName == "bgt") {
		if (isLiteral(fourthWord)) {
			if (regex_match(line, val, branchLiteral)) {
				return true;
			}
			else return false;
		} 
		else if (isSymbol(fourthWord)) {
			if (regex_match(line, val, branchSymbol)) {
				return true;
			}
			else return false;
		}
	}

	else if (opName == "ld") {
		if (isLiteral(secondWord)) {
			if (regex_match(line, val, loadLiteralFromMemory)) {
				return true;
			}
			else return false;
		} 
		else if (isSymbol(secondWord)) {
			if (regex_match(line, val, loadSymbolFromMemory)) {
				return true;
			}
			else return false;
		}
		else if (isLiteralValue(secondWord)) {
			if (regex_match(line, val, loadLiteralValue)) {
				return true;
			}
			else return false;
		}
		else if (isSymbolValue(secondWord)) {
			if (regex_match(line, val, loadSymbolValue)) {
				return true;
			}
			else return false;
		}
		else if (isReg(secondWord)) {
			if (regex_match(line, val, loadFromReg)) {
				return true;
			}
			else return false;
		}
		else if (isFromMemoryReg(secondWord)) {
			if (regex_match(line, val, loadFromMemoryReg)) {
				return true;
			}
			else return false;
		}
		else if (isFromMemoryWithOffsetLiteral(secondWord)) {
			if (regex_match(line, val, loadFromMemoryWithOffsetLiteral)) {
				return true;
			}
			else return false;
		}
		else if (isFromMemoryWithOffsetSymbol(secondWord)) {
			if (regex_match(line, val, loadFromMemoryWithOffsetSymbol)) {
				return true;
			}
			else return false;
		}
	}
	else if (opName == "st") {
		if (isLiteral(thirdWord)) {
			if (regex_match(line, val, storeLiteralFromMemory)) return true;
			else return false;
		}
		else if (isSymbol(thirdWord)) {
			if (regex_match(line, val, storeSymbolFromMemory)) return true;
			else return false;
		}
		else if (isLiteralValue(thirdWord)) {
			if (regex_match(line, val, storeLiteralValue)) return true;
			else return false;
		}
		else if (isSymbolValue(thirdWord)) {
			if (regex_match(line, val, storeSymbolValue)) return true;
			else return false;
		}
		else if (isReg(thirdWord)) {
			if (regex_match(line, val, storeFromReg)) return true;
			else return false;
		}
		else if (isFromMemoryReg(thirdWord)) {
			if (regex_match(line, val, storeFromMemoryReg)) return true;
			else return false;
		}
		else if (isFromMemoryWithOffsetLiteral(thirdWord)) {
			if (regex_match(line, val, storeFromMemoryWithOffsetLiteral)) return true;
			else return false;
		}
		else if (isFromMemoryWithOffsetSymbol(thirdWord)) {
			if (regex_match(line, val, storeFromMemoryWithOffsetSymbol)) return true;
			else return false;
		}
	}
	else if (opName == "csrrd") {
		if (regex_match(line, val, readControlStatusReg)) return true;
		else return false;
	}
	else if (opName == "csrwr") {
		if (regex_match(line, val, writeControlStatusReg)) return true;
		else return false;
	}


	return false;
	
}

string Assembler::getFirstWord(string inputString) {
	size_t firstSpacePosition = inputString.find(' ');
	string firstWord = inputString.substr(0, firstSpacePosition);
	return firstWord;
}

string Assembler::getSecondWord(string inputString) {
	size_t spacePos = inputString.find(' ');
	size_t commaPos = inputString.find(',', spacePos + 1);
	string subString;

	subString = inputString.substr(spacePos + 1, commaPos - spacePos - 1);
	return subString;
}

string Assembler::getThirdWord(string inputString) {
	std::istringstream iss(inputString);
	std::string firstWord, secondWord;
	iss >> firstWord >> secondWord;

	// Check if there is a comma in the input string
	size_t commaPos = inputString.find(',');
	if (commaPos != std::string::npos) {
		std::string remainingString = inputString.substr(commaPos + 1);
		std::istringstream remainingIss(remainingString);
		std::string thirdWord;
		if (std::getline(remainingIss, thirdWord, ',')) {
			// Remove any leading/trailing whitespace from the third word
			thirdWord.erase(0, thirdWord.find_first_not_of(" \t"));
			thirdWord.erase(thirdWord.find_last_not_of(" \t") + 1);
			return thirdWord;
		}
	}

	return "";
}

string Assembler::getFourthWord(string inputString) {
	std::istringstream iss(inputString);
	std::string firstWord, secondWord, thirdWord, fourthWord;
	iss >> firstWord >> secondWord >> thirdWord >> fourthWord;

	// Remove any leading whitespace from the fourth word
	fourthWord.erase(0, fourthWord.find_first_not_of(" \t"));

	return fourthWord;
}

bool Assembler::isLiteral(string line) {
	smatch val;
	if (regex_match(line, val, literal)) return true;
	else return false;
}

bool Assembler::isSymbol(string line) {
	smatch val;
	if (regex_match(line, val, symbol)) return true;
	else return false;
}

bool Assembler::isLiteralValue(string line) {
	smatch val;
	if (regex_match(line, val, litVal)) return true;
	else return false;
}

bool Assembler::isSymbolValue(string line) {
	smatch val;
	if (regex_match(line, val, symVal)) return true;
	else return false;
}

bool Assembler::isReg(string line) {
	smatch val;
	if (regex_match(line, val, reg)) return true;
	else return false;
}

bool Assembler::isFromMemoryReg(string line) {
	smatch val;
	if (regex_match(line, val, fromMemoryReg)) return true;
	else return false;
}

bool Assembler::isFromMemoryWithOffsetLiteral(string line) {
	smatch val;
	string addedBlanks = addBlankSpace(line);

	if (regex_match(addedBlanks, val, fromMemoryWithOffsetLiteral)) return true;
	else return false;
}

bool Assembler::isFromMemoryWithOffsetSymbol(string line) {
	smatch val;
	string addedBlanks = addBlankSpace(line);
	if (regex_match(addedBlanks, val, fromMemoryWithOffsetSymbol)) return true;
	else return false;
}


string Assembler::readCauseReg(int cause) {
	string ret = "";
	switch (cause) {
		case 1: ret = "Nekorektna instrukcija.\n"; break;
		case 2: ret = "Zahtev za prekid od tajmera.\n"; break;
		case 3: ret = "Zahtev za prekid od terminala.\n"; break;
		case 4: ret = "Softverski prekid.\n"; break;
	}

	return ret;
}


string Assembler::removeLeadingSpaces(string line) {
	size_t firstNonSpace = line.find_first_not_of(' ');
	if (firstNonSpace == string::npos) {
		return "";
	}
	else {
		return line.substr(firstNonSpace);
	}

}

string Assembler::removeWhitespaces(string line) {
	string result;
	result.reserve(line.length());

	for (char c : line) {
		if (!isspace(c)) {
			result.push_back(c);
		}
	}

	return result;
}

string Assembler::removeTrailingWhitespaces(string line) {
	size_t lastNonSpace = line.find_last_not_of(" \t\n\r\f\v");
	if (lastNonSpace != string::npos) {
		return line.substr(0, lastNonSpace + 1);
	}
	else return line;
}

string Assembler::extractAfterColon(string line) {
	size_t colonPos = line.find(':');
	if (colonPos != string::npos && colonPos < line.length() - 1) {
		return line.substr(colonPos + 1);
	}
	else return "";
}

string Assembler::extractLabel(string line) {
	size_t colonPos = line.find(':');
	if (colonPos != string::npos) {
		return line.substr(0, colonPos + 1);
	}
	else return "";
}


string Assembler::extractLabelWithoutSemicolon(string line) {
	size_t colonPos = line.find(':');
	if (colonPos != string::npos) {
		return line.substr(0, colonPos);
	}
	else return "";
}

string Assembler::prettifyString(string line) {
	regex pattern(R"((%r\d+)\s*,)");
	string res = std::regex_replace(line, pattern, "$1,");
	if (res == line) return "";
	else return res;
}

string Assembler::removeDollarSign(string line) {
	std::string output = line;
	output.erase(std::remove(output.begin(), output.end(), '$'), output.end());
	return output;
}

string Assembler::padNumber(string number, int digits) {
	if (number.length() >= digits) {
		return number.substr(number.length() - digits);
	}
	else {
		string newString = std::string(digits - number.length(), '0') + number;
		return newString;
	}
}

void Assembler::printSymbolTable(ostream& file) {
	file << "Tabela simbola:" << endl;
	file << std::setw(20)<< std::left << "| Number |";
	file << std::setw(20) << std::left << "| Name |";
	file << std::setw(20) << std::left << "| Section |";
	file << std::setw(20) << std::left << "| Offset |";
	file << std::setw(20) << std::left << "| Is Global |";
	file << endl;
	

	Symbol* s = nullptr;
	int i = 0;
	while (i < symbolTable->getSize()) {
		s = symbolTable->getWithoutRemovingFromList(i);
		if (s != nullptr) {
			file << std::setw(20) << std::left << s->getNumber();
			file << std::setw(20) << std::left << s->getName();
			file << std::setw(20) << std::left << s->getSection();
			file << std::setw(20) << std::left << s->getOffset();
			file << std::setw(20) << std::left << s->getIsGlobal();
			file << std::endl;
		}
		i++;
		file << endl;
	}
	file << "Kraj tabele simbola." << endl;
}

void Assembler::printSectionTable(ostream& file) {
	file << "Tabela sekcija:" << endl;
	
	file << std::setw(20) << std::left << "| Number |";
	file << std::setw(20) << std::left << "| Name |";
	file << std::setw(20) << std::left << "| Size |";
	file << std::setw(20) << std::left << "| Literal Pool Begining |";
	file << std::setw(20) << std::left << "| Literal Pool Size |";
	file << std::setw(20) << std::left << "| Whole Section Size |";
	file << endl;
	

	Section* s = nullptr;
	int i = 0;
	while (i < sectionTable->getSize()) {
		s = sectionTable->getWithoutRemovingFromList(i);
		if (s != nullptr) {
			file << std::setw(20) << std::left << s->getIdSection();
			file << std::setw(20) << std::left << s->getName();
			file << std::setw(20) << std::left << s->getSize();
			file << std::setw(20) << std::left << s->getLiteralPool()->getLiteralPoolBegining();
			file << std::setw(20) << std::left << s->getLiteralPool()->getSize();
			file << std::setw(20) << std::left << s->getLiteralPool()->getSize() + s->getSize();
			file << std::endl;
		}
		i++;
		file << endl;
	}

	file << "Kraj tabele sekcija." << endl;
}

void Assembler::printRelocationRecords(ostream& file) {
	file << "Relokacioni zapisi:" << endl;
	
	file << std::setw(20) << std::left << "| Number |";
	file << std::setw(20) << std::left << "| Section |";
	file << std::setw(20) << std::left << "| Offset |";
	file << std::setw(20) << std::left << "| Type of Relocation |";
	file << std::setw(20) << std::left << "| Number |";
	file << std::setw(20) << std::left << "| Addend |";
	file << endl;
	

	RelocationRecord* rr = nullptr;
	int i = 0;
	while (i < relocationRecords->getSize()) {
		rr = relocationRecords->getWithoutRemovingFromList(i);
		if (rr != nullptr) {
			file << std::setw(20) << std::left << rr->getRRId();
			file << std::setw(20) << std::left << rr->getSection();
			file << std::setw(20) << std::left << rr->getOffset();
			file << std::setw(25) << std::left << rr->getTypeOfRelocation();
			file << std::setw(20) << std::left << rr->getSymbolRbr();
			file << std::setw(20) << std::left << rr->getAddend();
			file << std::endl;
		}
		i++;
		file << endl;
	}
	file << "Kraj relokacionih zapisa." << endl;

}


void Assembler::printBytesInCode(ostream& file) {
	Section* s = nullptr;
	int i = 0;
	while (i < sectionTable->getSize()) {
		
		s = sectionTable->getWithoutRemovingFromList(i);
		
		if (s != nullptr) {

			if (s->getBytes().size() != 0) {
				
				file << "Bajt kod: " << endl;
				file << s->getName() << ":" << endl;
				for (int i = 0; i < s->getBytes().size(); i++) {
					file << s->getBytes()[i];
				}
				file << endl;
				file << "Kraj ispisa bajt koda." << endl;
			}
			if (s->getLiteralPool()->getSize() != 0) {
				file << "Bazen literala:" << endl;
				file << s->getName() << ":" << endl;
				file << std::setw(28) << std::left << "| Vrednost |";
				file << std::setw(28) << std::left << " Adresa u bazenu |";
				file << std::setw(28) << std::left << " Offset u bazenu |";
				file << endl;


				for (int i = 0; i < s->getLiteralPool()->getListOfLiterals()->getSize(); i++) {
					file << std::setw(18) << std::left << s->getLiteralPool()->getListOfLiterals()->getWithoutRemovingFromList(i)->literalVal;
					file << std::setw(25) << std::left << s->getLiteralPool()->getListOfLiterals()->getWithoutRemovingFromList(i)->literalAddres;
					file << std::setw(25) << std::left << s->getLiteralPool()->getListOfLiterals()->getWithoutRemovingFromList(i)->offset;
					file << endl;
				}
				file << "Kraj bazena literala.";
			}
			file << endl;
		
		}
		i++;
		file << endl;
	}

}

bool Assembler::checkIfAllSymbolsAreDefined() {
	int i = 0;
	bool returnVal = false;
	Symbol* s = nullptr;
	while (i < symbolTable->getSize()) {
		s = symbolTable->getWithoutRemovingFromList(i);
		if (!s) return false;
		if (s->getSection() == 0 && !(s->getIsExtern())) {
			returnVal = true;
			std::cout << e.symbolUndefined() << s->getName() << endl;
		}
		i++;
	}

	return returnVal;
}


bool Assembler::checkForMoreThanOneDirectiveInLine(string line) {
	smatch match;
	string word;
	stringstream stream(line);
	int numberOfDirectives = 0;

	while (stream >> word) {
		word.erase(remove(word.begin(), word.end(), ','), word.end());

		if (regex_match(word, match, directives)) {
			numberOfDirectives++;
		}
	}

	if (numberOfDirectives > 1) return true;
	else return false;
	 
}

bool Assembler::checkIfThereIsAInstructionBeforeLabel(string line) {
	smatch match;
	string word;
	bool labela = false;
	stringstream stream(line);

	while (stream >> word) {
		word.erase(remove(word.begin(), word.end(), ','), word.end());

		if (regex_match(word, match, label)) {
			labela = true;
		}
	}
	if (labela) {
		if (findWordPosition(line) == 1) return true;
		else return false;
	}
   
	
	return false;

}

int Assembler::findWordPosition(string line) {
	vector<string> words;
	std::string word;
	std::stringstream ss(line);

	while (ss >> word) {
		word.erase(remove(word.begin(), word.end(), ','), word.end());
		if (word.length() > 0 && word.back() == ':') {
			if (!words.empty()) {
				return 1;
			}
		}
		words.push_back(word);
	}

	return 0;
}

bool Assembler::checkForSymbolInTable(string label) {

	int i = 0;
	bool exists = false;
	Symbol* symbol = nullptr;

	while (i < symbolTable->getSize()) {
		if (symbolTable->getWithoutRemovingFromList(i)->getName() == label) {
			symbol = symbolTable->getWithoutRemovingFromList(i);
			exists = true;
			break;
		}

		i++;
	}
	if (exists) {
		return true;
	}
	else return false;
}

int Assembler::getSectionNumberForSymbolName(string name) {
	int i = 0;
	bool exists = false;
	Symbol* symbol = nullptr;
	while (i < symbolTable->getSize()) {
		if (symbolTable->getWithoutRemovingFromList(i)->getName() == name) {
			symbol = symbolTable->getWithoutRemovingFromList(i);
			exists = true;
			break;
		}

		i++;
	}
	if (exists) return symbol->getSection();
	else return -1;
}

int Assembler::getOffsetForSymbolName(string name) {
	int i = 0;
	bool exists = false;
	Symbol* symbol = nullptr;
	while (i < symbolTable->getSize()) {
		if (symbolTable->getWithoutRemovingFromList(i)->getName() == name) {
			symbol = symbolTable->getWithoutRemovingFromList(i);
			exists = true;
			break;
		}

		i++;
	}
	if (exists) return symbol->getOffset();
	else return -2;
}

int Assembler::getRbrForSymbolName(string name) {
	int i = 0;
	bool exists = false;
	Symbol* symbol = nullptr;
	while (i < symbolTable->getSize()) {
		if (symbolTable->getWithoutRemovingFromList(i)->getName() == name) {
			symbol = symbolTable->getWithoutRemovingFromList(i);
			exists = true;
			break;
		}

		i++;
	}
	if (exists) return symbol->getNumber();
	else return -1;
}

void Assembler::addSymbolToTableIfAlreadyExistsInTable(Symbol* symbol, string label) {
	symbol->setSection(currentSection);
	symbol->setOffset(locationCounter);
}


void Assembler::addSymbolToTableIfItDoesntExist(string name) {
	smatch val;
	if (regex_match(name, val, label)) {
		name = extractLabelWithoutSemicolon(name);
	}
	Symbol* s = new Symbol(name, currentSection, locationCounter, false, true, false);
	symbolTable->addLast(s);
}

string Assembler::getSectionName(string line) {
	std::size_t spacePos = line.find(' ');

	// Extract the substring after the first space
	std::string sectionName = line.substr(spacePos + 1);

	// Remove any remaining whitespace characters
	sectionName.erase(std::remove_if(sectionName.begin(), sectionName.end(), ::isspace), sectionName.end());

	return sectionName;
}

int Assembler::getSkipBytes(string line) {
	smatch val;
	int numOfBytes;
	std::size_t spacePos = line.find(' ');
	std::string bytesString = line.substr(spacePos + 1);
	bytesString.erase(std::remove_if(bytesString.begin(), bytesString.end(), ::isspace), bytesString.end());
	if (regex_match(bytesString, val, hexRegex)) numOfBytes = hexToDecimal(bytesString);
	else if (regex_match(bytesString, val, binaryRegex)) { 
		size_t beforeBPos = bytesString.find('b');
		numOfBytes = binToDecimal(bytesString); 
	}
	else numOfBytes = std::stoi(bytesString);
	return numOfBytes;
}

void Assembler::insertIntoSectionTableBeforeFirstPassUND() {
	Section* undefinedSection = new Section("UND", 0);
	this->sectionTable->addLast(undefinedSection);
	Symbol* undefinedSectionSymbol = new Symbol("UND", currentSection, 0, false, true, false);
	this->symbolTable->addLast(undefinedSectionSymbol);
}

int Assembler::countWordsAfterDotWord(string line) {
	string dotWord = ".word";
	size_t dotWordPos = line.find(dotWord);

	if (dotWordPos != string::npos) {
		string words = line.substr(dotWordPos + dotWord.length());
		stringstream ss(words);
		string word;
		int count = 0;

		while (ss >> word) count++;
		return count;
	}

	return 0;
}

Section* Assembler::getOldSection() {
	for (int i = 0; i < sectionTable->getSize(); i++) {
		if (sectionTable->getWithoutRemovingFromList(i)->getName() == currentSectionName) return sectionTable->getWithoutRemovingFromList(i);
	}
	return nullptr;
}

void Assembler::addLiteralToPool(string lit) {
	Section* sec = nullptr;
	for (int i = 0; i < sectionTable->getSize(); i++) {
		if (sectionTable->getWithoutRemovingFromList(i)->getName() == currentSectionName) {
			sec = sectionTable->getWithoutRemovingFromList(i);
			break;
		}
	}
	Literal* literalVal = new Literal(0, lit);
	if(!sec->getLiteralPool()->checkIfLiteralAlreadyExists(lit)) sec->getLiteralPool()->addLiteralToList(literalVal);
	
}

string Assembler::findAndCheckLiteral(string line) {
	std::regex literalRegex("\\b(0x)?([1-9A-Fa-f][0-9A-Fa-f]*)\\b");
	std::smatch match;

	std::string literal = "";
	std::string::const_iterator searchStart(line.cbegin());
	while (std::regex_search(searchStart, line.cend(), match, literalRegex)) {
		std::string prefix = match.str(1);
		literal = match.str(2);
		if (literal.length() > 3 || (literal.length() == 3 && std::stoi(literal, nullptr, 16) > 0xFFF)) {
			return prefix + literal;
		}
		searchStart = match.suffix().first;
	}

	return "";

}

string Assembler::findAndCheckSymbol(string line) {
	std::regex symbolRegex("\\b([a-zA-Z][a-zA-Z0-9_]*)\\b");

	std::smatch match;
	std::string symbol = "";

	if (std::regex_search(line, match, symbolRegex)) {
		symbol = match.str(1);
	}

	return symbol;
}


bool Assembler::firstPass(string line) {
	smatch val;
	bool exists = false;	
	int numOfBytes;
	int numOfWords;
	string firstWord = getFirstWord(line);
	string secondWord = getSecondWord(line);
	string thirdWord = getThirdWord(line);
	string fourthWord = getFourthWord(line);
	if (regex_match(line, val, label)) {
		if (currentSection == 0) {
			std::cout << e.labelInSection() << endl;
			return false;
		}
		else {
			exists = checkForSymbolInTable(line);
			if (exists) return false;
			addSymbolToTableIfItDoesntExist(line);
			return true;

		}
	}
	else if(regex_match(line, val, section)) {
		
		int oldLocationCounter = locationCounter;
		Section* oldSection = getOldSection();
		oldSection->setSize(oldLocationCounter);
		oldSection->getLiteralPool()->setLiteralPoolBegining(oldLocationCounter);
		if(oldSection->getLiteralPool()->getListOfLiterals()->getSize() > 0)
		oldSection->getLiteralPool()->fixAddressesForLiterals();
		string newSectionName = getSectionName(line);

		currentSectionName = newSectionName;
		Section* newSection = new Section(currentSectionName, 0);
		this->sectionTable->addLast(newSection);
		
		
		if (checkForSymbolInTable(newSectionName)) return false;
		
		currentSection++;
		locationCounter = 0;
		addSymbolToTableIfItDoesntExist(currentSectionName);
		return true;
	}
	else if (regex_match(line, val, skip)) {
		numOfBytes = getSkipBytes(line);
		if (numOfBytes + locationCounter > 2047) return false;
		locationCounter += numOfBytes;
		return true;
	}
	else if (regex_match(line, val, word)) {
		if (currentSection == 0) return false;
		numOfWords = countWordsAfterDotWord(line);
		locationCounter += (numOfWords * 4);
		return true;
	}
	else if (regex_match(line, val, global) || regex_match(line, val, externn)) {
		
		return true;
	}
	else if (regex_match(line, val, endd)) {

		int oldLocationCounter = locationCounter;
		Section* oldSection = getOldSection();
		oldSection->setSize(oldLocationCounter);
		oldSection->getLiteralPool()->setLiteralPoolBegining(oldLocationCounter);
		if (oldSection->getLiteralPool()->getListOfLiterals()->getSize() > 0)
		oldSection->getLiteralPool()->fixAddressesForLiterals();
		currentSection = 0;
		currentSectionName = "UND";
		locationCounter = 0;
		return true;
	}
	else if (regex_match(firstWord, val, onlyInstructionRegex)) {
		if (currentSectionName == "UND") return false;
		if (regex_match(line, val, loadFromMemoryWithOffsetLiteral)) {
			string displ = extractOffsetLiteral(line);
			displ = removeHexPrefix(displ);
			if (findAndCheckLiteral(displ) != "") return false;
		} else if (findAndCheckLiteral(line) != "") {
		
			string literalVal = findAndCheckLiteral(line);

			if (regex_match(literalVal, val, hexRegex)) {
				//literalVal = removeHexPrefix(literalVal);
				addLiteralToPool(literalVal);
			}
			else {
				literalVal = decToHex(literalVal);
				literalVal = toUpperCase(literalVal);
				addLiteralToPool(literalVal);
			}	
		}
		else {
			if (regex_match(line, val, jumpSymbolValue)) {
				addLiteralToPool(secondWord);
			}
			else if (regex_match(line, val, callSymbol)) {
				
					addLiteralToPool(secondWord);
				
			}
			else if (regex_match(line, val, branchSymbol)) {
				 addLiteralToPool(fourthWord);
			}
			else if (regex_match(line, val, loadFromMemoryWithOffsetSymbol)) {
				
				string addedBlanks = addBlankSpace(secondWord);
				string displ = extractOffsetSymbol(addedBlanks);
				addLiteralToPool(displ);
			}
			else if (regex_match(line, val, loadSymbolValue)) {
				string wihtoutDollar = removeDollarSign(secondWord);
				addLiteralToPool(wihtoutDollar);
			}
			else if (regex_match(line, val, loadSymbolFromMemory)) {
				addLiteralToPool(secondWord);
			}
			else if (regex_match(line, val, storeSymbolValue)) {
				string wihtoutDollar = removeDollarSign(thirdWord);
				addLiteralToPool(wihtoutDollar);
			}
			else if (regex_match(line, val, storeFromMemoryWithOffsetSymbol)) {
				string addedBlanks = addBlankSpace(thirdWord);
				string displ = extractOffsetSymbol(addedBlanks);
				addLiteralToPool(displ);
			}
			else if (regex_match(line, val, storeSymbolFromMemory)) {
				 addLiteralToPool(thirdWord);
			}
		}
		//TODO: lc srediti
		if (regex_match(line, val, loadLiteralFromMemory) || regex_match(line, val, loadSymbolFromMemory)) locationCounter += 8;
		else locationCounter += 4;
		
		return true;
	}
	return true;
	
}

bool Assembler::setOffsetToSymbol(string symbolName) {
	Section* sec = getOldSection();
	int offset = getOffsetForSymbolName(symbolName);

	int i = 0;
	string symName;
	while (i < sec->getLiteralPool()->getListOfLiterals()->getSize()) {
		symName = sec->getLiteralPool()->getListOfLiterals()->getWithoutRemovingFromList(i)->literalVal;
		if (symName == symbolName) {
			if(checkIfSymbolIsExtern(symName)) sec->getLiteralPool()->setOffset("???", symbolName);
			else sec->getLiteralPool()->setOffset(to_string(offset), symbolName);
			return true;
		}
		i++;
	}
	return false;
}

int Assembler::getNumberOfDirective(string dirName) {
	if (dirName == ".global") return 1;
	else if (dirName == ".extern") return 2;
	else if (dirName == ".section") return 3;
	else if (dirName == ".word") return 4;
	else if (dirName == ".skip") return 5;
	else if (dirName == ".end") return 6;
	else return 0;
}

vector<string> Assembler::extractExternVariables(string line) {
	vector<string> externVariables;
	string externKeyword = ".extern";
	size_t externKeywordsPos = line.find(externKeyword);

	if (externKeywordsPos != string::npos) {
		string variables = line.substr(externKeywordsPos + externKeyword.length());
		stringstream ss(variables);
		string variable;

		while (std::getline(ss, variable, ',')) {
			variable.erase(0, variable.find_first_not_of(' '));
			variable.erase(variable.find_last_not_of(' ') + 1);
			externVariables.push_back(variable);
		}
		
	}

	return externVariables;
}

vector<string> Assembler::extractGlobalVariables(string line) {
	vector<string> globalVariables;
	string globalKeyword = ".global";
	size_t globalKeywordsPos = line.find(globalKeyword);

	if (globalKeywordsPos != string::npos) {
		string variables = line.substr(globalKeywordsPos + globalKeyword.length());
		stringstream ss(variables);
		string variable;

		while (std::getline(ss, variable, ',')) {
			variable.erase(0, variable.find_first_not_of(' '));
			variable.erase(variable.find_last_not_of(' ') + 1);
			globalVariables.push_back(variable);
		}

	}

	return globalVariables;
}

vector<string> Assembler::extractWordVariables(string line) {
	vector<string> wordVariables;
	string wordKeyword = ".word";
	size_t wordKeywordsPos = line.find(wordKeyword);

	if (wordKeywordsPos != string::npos) {
		string variables = line.substr(wordKeywordsPos + wordKeyword.length());
		stringstream ss(variables);
		string variable;

		while (std::getline(ss, variable, ',')) {
			variable.erase(0, variable.find_first_not_of(' '));
			variable.erase(variable.find_last_not_of(' ') + 1);
			wordVariables.push_back(variable);
		}

	}

	return wordVariables;
}

void Assembler::setSymbolToBeGlobal(string name) {
	int i = 0;
	bool exists = false;
	Symbol* symbol = nullptr;

	while (i < symbolTable->getSize()) {
		if (symbolTable->getWithoutRemovingFromList(i)->getName() == name) {
			symbol = symbolTable->getWithoutRemovingFromList(i);
			break;
		}

		i++;
	}
	symbol->setIsLocal(false);
	symbol->setIsGlobal(true);

}

bool Assembler::checkIfSymbolIsExtern(string name) {

	int i = 0;
	bool externSym = false;
	Symbol* symbol = nullptr;
	while (i < symbolTable->getSize()) {
		if (symbolTable->getWithoutRemovingFromList(i)->getName() == name) {
			symbol = symbolTable->getWithoutRemovingFromList(i);
			if (symbol->getIsExtern()) externSym = true;
			break;
		}

		i++;
	}

	return externSym;
}

bool Assembler::checkIfSymbolIsGlobal(string name) {

	int i = 0;
	bool globalSym = false;
	Symbol* symbol = nullptr;
	while (i < symbolTable->getSize()) {
		if (symbolTable->getWithoutRemovingFromList(i)->getName() == name) {
			symbol = symbolTable->getWithoutRemovingFromList(i);
			if (symbol->getIsGlobal()) globalSym = true;
			break;
		}

		i++;
	}

	return globalSym;
}

bool Assembler::checkIfSymbolIsLocal(string name) {

	int i = 0;
	bool localSym = false;
	Symbol* symbol = nullptr;
	while (i < symbolTable->getSize()) {
		if (symbolTable->getWithoutRemovingFromList(i)->getName() == name) {
			symbol = symbolTable->getWithoutRemovingFromList(i);
			if (symbol->getIsLocal()) localSym = true;
			break;
		}

		i++;
	}

	return localSym;
}

bool Assembler::checkIfSymbolInPool(string symbolVal) {
	int i = 0;
	Section* sec = getOldSection();
	if (sec->getLiteralPool()->checkIfLiteralAlreadyExists(symbolVal)) return true;
	else return false;
}

void Assembler::insertIntoCurrentSectionBytes(string line) {
	int i = 0;
	int size = this->sectionTable->getSize();
	while (i < size) {
		if (this->sectionTable->getWithoutRemovingFromList(i)->getName() == currentSectionName) break;
		i++;
	}

	Section* section = this->sectionTable->getWithoutRemovingFromList(i);
	if (!section) return;

	section->insertIntoBytes(line);

}

int Assembler::getInstructionNumber(string word) {
	if (word == "halt") {
		return 1;
	}
	else if (word == "int") {
		return 2;
	}
	else if (word == "ret") {
		return 3;
	}
	else if (word == "iret") {
		return 4;
	}
	else if (word == "call") {
		return 5;
	}
	else if (word == "ld") {
		return 6;
	}
	else if (word == "st") {
		return 7;
	}
	else if (word == "csrrd") {
		return 8;
	}
	else if (word == "csrwr") {
		return 9;
	}
	else if (word == "add") {
		return 10;
	}
	else if (word == "sub") {
		return 11;
	}
	else if (word == "mul") {
		return 12;
	}
	else if (word == "div") {
		return 13;
	}
	else if (word == "and") {
		return 14;
	}
	else if (word == "or") {
		return 15;
	}
	else if (word == "xor") {
		return 16;
	}
	else if (word == "not") {
		return 17;
	}
	else if (word == "xchg") {
		return 18;
	}
	else if (word == "shl") {
		return 19;
	}
	else if (word == "shr") {
		return 20;
	}
	else if (word == "push") {
		return 21;
	}
	else if (word == "pop") {
		return 22;
	}
	else if (word == "jmp") {
		return 23;
	}
	else if (word == "beq") {
		return 24;
	}
	else if (word == "bne") {
		return 25;
	}
	else if (word == "bgt") {
		return 26;
	}
	else {
		return -1; // Return -1 for error case
	}
}

string Assembler::extractRegister(string line) {
	std::regex pattern(R"(\[%((r([0-9]|1[0-5])|sp|pc|status|handler|cause))\b)");
	std::smatch matches;

	if (std::regex_search(line, matches, pattern)) {
		std::string registerValue = "%" + matches[1].str();
		return registerValue;
	}

	return ""; // Return an empty string if no match is found

}

string Assembler::toUpperCase(string line) {
	std::string uppercaseHex = line;
	for (char& ch : uppercaseHex) {
		ch = std::toupper(ch);
	}
	return uppercaseHex;
}

string Assembler::extractOffsetLiteral(string line) {
	std::regex pattern(R"(\[%((r([0-9]|1[0-5])|sp|pc|status|handler|cause))\s*\+\s*((?:0x[0-9a-fA-F]+)|(?:[0-9]+))\])");
	std::smatch matches;

	if (std::regex_search(line, matches, pattern)) {
		std::string offsetValue = matches[4].str();
		return offsetValue;
	}

	return "";
}

string Assembler::extractOffsetSymbol(string line) {
	std::regex pattern(R"(\[%((r([0-9]|1[0-5])|sp|pc|status|handler|cause))\s*\+\s*([a-zA-Z][a-zA-Z0-9_]*)\])");
	std::smatch matches;

	if (std::regex_search(line, matches, pattern)) {
		std::string offsetValue = matches[4].str();
		return offsetValue;
	}

	return "";
}


string Assembler::removeHexPrefix(string line) {
	if (line.size() >= 2 && line.substr(0, 2) == "0x") {
		return line.substr(2); // Return the substring without the "0x" prefix
	}
	return line;
}

string Assembler::returnSectionName(int num) {
	int i = 0;
	while (i < sectionTable->getSize()) {
		if (sectionTable->getWithoutRemovingFromList(i)->getIdSection() == num) return sectionTable->getWithoutRemovingFromList(i)->getName();
		i++;
	}
	return "";
}

string Assembler::resolveInstruction(string line) {
	string result = "";
	smatch val;
	string firstWord = getFirstWord(line);
	string secondWord = getSecondWord(line);
	string thirdWord = getThirdWord(line);
	string fourthWord = getFourthWord(line);

	std::regex whitespacePattern("^\\s+");
	secondWord = removeWhitespaces(secondWord);
	thirdWord = std::regex_replace(thirdWord, whitespacePattern, "");
	fourthWord = std::regex_replace(fourthWord, whitespacePattern, "");

	int instructionNumber = getInstructionNumber(firstWord);
	switch (instructionNumber) {
		//srediti sa poolom instrukcije
		case 1: {
			//halt
			return "00000000";
			
		}
		case 2: {
			//int
			return "10000000";
			
		}
		case 3: {
			//ret
			result.append("93");
			result.append(checkReg("%pc"));
			result.append(checkReg("%sp"));
			result.append("0004");
			return result;	
		}
		case 4: {
			//iret

			result.append("96");
			result.append(checkReg("%status"));
			result.append(checkReg("%pc"));
			result.append("0");
			result.append("000");
			
			return result;
		}
		case 5: {
			//call
		
			if (isLiteral(secondWord)) {
				result.append("2");
				if (!regex_match(secondWord, val, hexRegex)) {
					secondWord = decToHex(secondWord);
					secondWord = toUpperCase(secondWord);
				
				}
				else {
					secondWord = removeHexPrefix(secondWord);
				
				}

				if (findAndCheckLiteral(secondWord) != "") {
					result.append("1");
					result.append(checkReg("%pc"));
					result.append("00");
					int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
					int displ = addr - locationCounter;
					string displ2 = decToHex(to_string(displ));
					displ2 = removeHexPrefix(displ2);
					displ2 = toUpperCase(displ2);
					string padNum = padNumber(displ2, 3);
					result.append(padNum);
				}
				else {
					result.append("0");
					result.append("000");
					string padNum = padNumber(secondWord, 3);
					result.append(padNum);
				}
				return result;
			}
			else {
				result.append("2");
				int sectionNumber = getSectionNumberForSymbolName(secondWord);
				int symbolRbr = getRbrForSymbolName(secondWord);
				int offsetSym = getOffsetForSymbolName(secondWord);
				string offset = decToHex(to_string(offsetSym));
				offset = removeHexPrefix(offset);
				offset = toUpperCase(offset);
				if (returnSectionName(sectionNumber) != currentSectionName && sectionNumber != 0) {

					if(setOffsetToSymbol(secondWord)){
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);

						result.append("1");
						result.append(checkReg("%pc"));
						result.append("00");
						result.append(padNum);
						return result;
					}
				}
				else if (returnSectionName(sectionNumber) == currentSectionName) {
					
					if (setOffsetToSymbol(secondWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);

						result.append("1");
						result.append(checkReg("%pc"));
						result.append("00");
						result.append(padNum);
						return result;
					}
					
				}
				else if(checkIfSymbolIsExtern(secondWord)) {

					if (setOffsetToSymbol(secondWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, "0", "R_X86_64_PC32", secondWord);
						this->relocationRecords->addLast(record);

						result.append("1");
						result.append(checkReg("%pc"));
						result.append("00");
						result.append(padNum);
						return result;
					}
				}
			}
			return "";
		}
		case 6: {
			//ld
			if (isLiteralValue(secondWord)) { //Vrednost literala
				result.append("9");
				string withoutDollar = removeDollarSign(secondWord);
				if (!regex_match(withoutDollar, val, hexRegex)) {
					withoutDollar = decToHex(withoutDollar);
					withoutDollar = toUpperCase(withoutDollar);
				}
				else {
					withoutDollar = removeHexPrefix(withoutDollar);
				}
				if (findAndCheckLiteral(withoutDollar) != "") {
					result.append("2");
					result.append(checkReg(thirdWord));
					result.append(checkReg("%pc"));
					result.append("0");
					int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(withoutDollar);
					int displ = addr - locationCounter;
					string displ2 = decToHex(to_string(displ));
					displ2 = removeHexPrefix(displ2);
					displ2 = toUpperCase(displ2);
					string padNum = padNumber(displ2, 3);
					result.append(padNum);
				}
				else {
					result.append("1");
					result.append(checkReg(thirdWord));
					result.append("00");
					string padNum = padNumber(withoutDollar, 3);
					result.append(padNum);
				}
				return result;
			}
			else if(isSymbolValue(secondWord)) {
				
				result.append("9");
				string withoutDollar = removeDollarSign(secondWord);
				int sectionNumber = getSectionNumberForSymbolName(withoutDollar);
				int symbolRbr = getRbrForSymbolName(withoutDollar);
				int offsetSym = getOffsetForSymbolName(withoutDollar);
				string offset = decToHex(to_string(offsetSym));
				offset = removeHexPrefix(offset);
				offset = toUpperCase(offset);

				if (returnSectionName(sectionNumber) != currentSectionName && sectionNumber != 0) {
					
					if (setOffsetToSymbol(withoutDollar)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(withoutDollar);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);

						result.append("2");
						result.append(checkReg(thirdWord));
						result.append(checkReg("%pc"));
						result.append("0");
						result.append(padNum);
						return result;
					}
				}
				else if (returnSectionName(sectionNumber) == currentSectionName) {
					if (setOffsetToSymbol(withoutDollar)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(withoutDollar);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);

						result.append("2");
						result.append(checkReg(thirdWord));
						result.append(checkReg("%pc"));
						result.append("0");
						result.append(padNum);
						return result;
					}
					
				}
				else if (checkIfSymbolIsExtern(withoutDollar)) {
					
					if (setOffsetToSymbol(withoutDollar)) {
					
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(withoutDollar);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, "0", "R_X86_64_PC32", withoutDollar);
						this->relocationRecords->addLast(record);

						result.append("2");
						result.append(checkReg(thirdWord));
						result.append(checkReg("%pc"));
						result.append("0");
						result.append(padNum);
						return result;
					}
				}

			}
			else if (isReg(secondWord)) { //Vrednost u registru RA
				result.append("91"); 
				result.append(checkReg(thirdWord));
				result.append(checkReg(secondWord));
				result.append("0000");
				return result;
			}
			else if (isLiteral(secondWord)) { //vrednost is memorije na adresi LITERAL
				result.append("9");
				if (!regex_match(secondWord, val, hexRegex)) {
					secondWord = decToHex(secondWord);
					secondWord = toUpperCase(secondWord);
				}
				else {
					secondWord = removeHexPrefix(secondWord);
				}
				if (findAndCheckLiteral(secondWord) != "") {
					result.append("2");
					result.append(checkReg(thirdWord));
					result.append(checkReg("%pc"));
					result.append("0");

					int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
					int displ = addr - locationCounter;
					string displ2 = decToHex(to_string(displ));
					displ2 = removeHexPrefix(displ2);
					displ2 = toUpperCase(displ2);
					string padNum = padNumber(displ2, 3);
					result.append(padNum);

					result.append("92");
					result.append(checkReg(thirdWord));
					result.append(checkReg(thirdWord));
					result.append("0000");
					return result;
				}
				else {
					result.append("2");
					result.append(checkReg(thirdWord));
					result.append("00");
					string padNum = padNumber(secondWord, 3);
					result.append(padNum);
					return result;
				}

			}
			else if (isFromMemoryWithOffsetLiteral(secondWord)) { //Vrednost iz memorije na adresi reg + displ, displ je literal
				string addedBlanks = addBlankSpace(secondWord);
				string regB = extractRegister(addedBlanks);
				string displ = extractOffsetLiteral(addedBlanks);
				result.append("92");
				result.append(checkReg(thirdWord));
				result.append(checkReg(regB));
				result.append("0");
				if (!regex_match(displ, val, hexRegex)) {
					string hex = decToHex(displ);
					hex = toUpperCase(hex);
					string padNum = padNumber(hex, 3);
					result.append(padNum);
				}
				else {
					displ = removeHexPrefix(displ);
					string padNum = padNumber(displ, 3);
					result.append(padNum);
				}
				
				return result;
			}
			else if (isFromMemoryWithOffsetSymbol(secondWord)) {
				result.append("9");
				string addedBlanks = addBlankSpace(secondWord);
				string regB = extractRegister(addedBlanks);
				string displ = extractOffsetSymbol(addedBlanks);
				int offsetSym = getOffsetForSymbolName(displ);
				int sectionNumber = getSectionNumberForSymbolName(displ);
				if (!(returnSectionName(sectionNumber) == currentSectionName)) return "";
					result.append("2");
					result.append(checkReg(thirdWord));
					result.append(checkReg(regB));
					result.append("0");
					int res = offsetSym - locationCounter;

					string hex = decToHex(to_string(res));
					hex = removeHexPrefix(hex);
					hex = toUpperCase(hex);
					string padNum = padNumber(hex, 3);

					return result;

			}
			else if (isFromMemoryReg(secondWord)) { //vrednost iz memorije na adresi reg
				string regB = extractRegister(secondWord);
				result.append("92");
				result.append(checkReg(thirdWord));
				result.append(checkReg(regB));
				result.append("0000");
				return result;
			}
			else if (isSymbol(secondWord)) {
				result.append("9");
				int sectionNumber = getSectionNumberForSymbolName(secondWord);
				int symbolRbr = getRbrForSymbolName(secondWord);
				int offsetSym = getOffsetForSymbolName(secondWord);
				string offset = decToHex(to_string(offsetSym));
				offset = removeHexPrefix(offset);
				offset = toUpperCase(offset);
				if (returnSectionName(sectionNumber) != currentSectionName && sectionNumber != 0) {
					if (setOffsetToSymbol(secondWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);

						result.append("2");

						result.append(checkReg(thirdWord));
						result.append(checkReg("%pc"));
						result.append("0");
						result.append(padNum);

						result.append("92");
						result.append(checkReg(thirdWord));
						result.append(checkReg(thirdWord));
						result.append("0000");
						return result;
					}
				} 
				else if (returnSectionName(sectionNumber) == currentSectionName) {
					if (setOffsetToSymbol(secondWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);

						result.append("2");

						result.append(checkReg(thirdWord));
						result.append(checkReg("%pc"));
						result.append("0");
						result.append(padNum);

						result.append("92");
						result.append(checkReg(thirdWord));
						result.append(checkReg(thirdWord));
						result.append("0000");
						return result;
					}
				}
				else if (checkIfSymbolIsExtern(secondWord)) {
					if (setOffsetToSymbol(secondWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, "0", "R_X86_64_PC32", secondWord);
						this->relocationRecords->addLast(record);
						result.append("2");
						result.append(checkReg(thirdWord));
						result.append(checkReg("%pc"));
						result.append("0");
						result.append(padNum, 3);
						result.append("92");
						result.append(checkReg(thirdWord));
						result.append(checkReg(thirdWord));
						result.append("0000");
						return result;

					}
				}
			}
			return "";
		}
		case 7: {
			//st
			if (isLiteralValue(thirdWord) || isSymbolValue(thirdWord) || isReg(thirdWord)) {
				std::cout << "GRESKA, operand ne moze biti vrednost simbola/literala/registra";
				return "";
			}
			else if (isLiteral(thirdWord)) {
				result.append("8");
				if (!regex_match(thirdWord, val, hexRegex)) {
					thirdWord = decToHex(thirdWord);
					thirdWord = toUpperCase(thirdWord);
				}
				else {
					thirdWord = removeHexPrefix(thirdWord);
				}

				if (findAndCheckLiteral(thirdWord) != "") {
					result.append("2");
					result.append(checkReg("%pc"));
					result.append("0");
					result.append(checkReg(secondWord));
					int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(thirdWord);
					int displ = addr - locationCounter;
					string displ2 = decToHex(to_string(displ));
					displ2 = removeHexPrefix(displ2);
					displ2 = toUpperCase(displ2);
					string padNum = padNumber(displ2, 3);
					result.append(padNum);

				}
				else {
					result.append("0");
					result.append("00");
					result.append(checkReg(secondWord));
					string padNum = padNumber(thirdWord, 3);
					result.append(padNum);
				}
				

				return result;
			}
			else if (isFromMemoryReg(thirdWord)) {
				string regA = extractRegister(thirdWord);
				result.append("80");
				result.append(checkReg(regA));
				result.append("0");
				result.append(checkReg(secondWord));
				result.append("000");
				return result;
			}
			else if (isFromMemoryWithOffsetLiteral(thirdWord)) {
				string addedBlanks = addBlankSpace(thirdWord);
				string regA = extractRegister(addedBlanks);
				string displ = extractOffsetLiteral(addedBlanks);
				result.append("80");
				result.append(checkReg(regA));
				result.append("0");
				result.append(checkReg(secondWord));
				if (!regex_match(displ, val, hexRegex)) {
					string hex = decToHex(displ);
					hex = toUpperCase(hex);
					string padNum = padNumber(hex, 3);
					result.append(padNum);
				}
				else {
					displ = removeHexPrefix(displ);
					string padNum = padNumber(displ, 3);
					result.append(padNum);
				}

				return result;
			}
			else if (isSymbol(thirdWord)) {
				result.append("8");
				int sectionNumber = getSectionNumberForSymbolName(thirdWord);
				int symbolRbr = getRbrForSymbolName(thirdWord);
				int offsetSym = getOffsetForSymbolName(thirdWord);
				string offset = decToHex(to_string(offsetSym));
				offset = removeHexPrefix(offset);
				offset = toUpperCase(offset);
				if (returnSectionName(sectionNumber) != currentSectionName && sectionNumber != 0) {
					if (setOffsetToSymbol(thirdWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(thirdWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);
						result.append("2");
						result.append(checkReg("%pc"));
						result.append("0");
						result.append(checkReg(secondWord));
						result.append(padNum);
						return result;

					}
				}
				else if (returnSectionName(sectionNumber) == currentSectionName) {
					if (setOffsetToSymbol(thirdWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(thirdWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);
						result.append("2");
						result.append(checkReg("%pc"));
						result.append("0");
						result.append(checkReg(secondWord));
						result.append(padNum);
						return result;

					}
				}
				else if (checkIfSymbolIsExtern(thirdWord)) {
					if (setOffsetToSymbol(thirdWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(thirdWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						string padNum = padNumber(displ2, 3);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, "0", "R_X86_64_PC32", thirdWord);
						this->relocationRecords->addLast(record);
						result.append("2");
						result.append(checkReg("%pc"));
						result.append("0");
						result.append(checkReg(secondWord));
						result.append(padNum, 3);
						return result;

					}
				}
			}
			else if (isFromMemoryWithOffsetSymbol(thirdWord)) {
						string addedBlanks = addBlankSpace(thirdWord);
						string regA = extractRegister(addedBlanks);
						string displ = extractOffsetSymbol(addedBlanks);
						int offsetSym = std::stoi(displ);
						int sectionNumber = getSectionNumberForSymbolName(displ);
						if (!(returnSectionName(sectionNumber) == currentSectionName)) return "";

						result.append("80");
						result.append(checkReg(regA));
						result.append("0");
						result.append(checkReg(secondWord));
						int res = offsetSym - locationCounter;

						string hex = decToHex(to_string(res));
						hex = removeHexPrefix(hex);
						hex = toUpperCase(hex);
						string padNum = padNumber(hex, 3);
						result.append(padNum);
						

						return result;
			}
			return "";
		}
		case 8: {
			//csrrd
			result.append("90");
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("0000");
			return result;
		}
		case 9: {
			//csrwr
			result.append("94");
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("0000");
			return result;
		}
		case 10: {
			//add
			result.append("50");
			result.append(checkReg(thirdWord));
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("000");
			return result;
		}
		case 11: {
			//sub
			result.append("51");
			result.append(checkReg(thirdWord));
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("000");
			return result;
		}
		case 12: {
			//mul
			result.append("52");
			result.append(checkReg(thirdWord));
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("000");
			return result;
		}
		case 13: {
			//div
			result.append("53");
			result.append(checkReg(thirdWord));
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("000");
			return result;
		}
		case 14: {
			//and
			result.append("61");
			result.append(checkReg(thirdWord));
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("000");
			return result;
		}
		case 15: {
			//or
			result.append("62");
			result.append(checkReg(thirdWord));
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("000");
			return result;
		}
		case 16: {
			//xor
			result.append("63");
			result.append(checkReg(thirdWord));
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("000");
			return result;
		}
		case 17: {
			//not
			result.append("60");
			result.append(checkReg(secondWord));
			result.append(checkReg(secondWord));
			result.append("0000");
			return result;
		}
		case 18: {
			//xchg
			result.append("50");
			result.append("0");
			result.append(checkReg(secondWord));
			result.append(checkReg(thirdWord));
			result.append("000");
			return result;
		}
		case 19: {
			//shl
			result.append("70");
			result.append(checkReg(thirdWord));
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("000");
			return result;
		}
		case 20: {
			//shr
			result.append("71");
			result.append(checkReg(thirdWord));
			result.append(checkReg(thirdWord));
			result.append(checkReg(secondWord));
			result.append("000");
			return result;
		}
		case 21: {
			//push
			result.append("81");
			result.append(checkReg("%sp"));
			result.append("0");
			result.append(checkReg(secondWord));
			result.append("FFC");
			return result;
		}
		case 22: {
			//pop
			result.append("93");
			result.append(checkReg(secondWord));
			result.append(checkReg("%sp"));
			result.append("0");
			result.append("004");
			return result;
		}
		case 23: {
			
			//jmp
			if (isLiteral(secondWord)) {

				if (!regex_match(secondWord, val, hexRegex)) {
					secondWord = decToHex(secondWord);
					secondWord = toUpperCase(secondWord);
				}
				else {
					secondWord = removeHexPrefix(secondWord);
				}

				if (findAndCheckLiteral(secondWord) != "") {
					result.append("38");
					result.append(checkReg("%pc"));
					result.append("00");
					int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
					int displ = addr - locationCounter;
					string displ2 = decToHex(to_string(displ));
					displ2 = removeHexPrefix(displ2);
					displ2 = toUpperCase(displ2);
					
					string padNum = padNumber(displ2, 3);
					result.append(padNum);
				}
				else {
					result.append("30");
					result.append(checkReg("%pc"));
					result.append("00");
					string padNum = padNumber(secondWord, 3);
					result.append(padNum);
				}

				return result;
			}
			else if(isSymbol(secondWord)) {
				result.append("3");
				int sectionNumber = getSectionNumberForSymbolName(secondWord);
				int symbolRbr = getRbrForSymbolName(secondWord);
				int offsetSym = getOffsetForSymbolName(secondWord);
				string offset = decToHex(to_string(offsetSym));
				offset = removeHexPrefix(offset);
				offset = toUpperCase(offset);
				if (returnSectionName(sectionNumber) != currentSectionName && sectionNumber != 0) {
					
					if (setOffsetToSymbol(secondWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);
						
						string padNum = padNumber(displ2, 3);
						result.append("8");
						result.append(checkReg("%pc"));
						result.append("00");
						result.append(padNum);
						return result;

					}
					
				}
				else if (returnSectionName(sectionNumber) == currentSectionName) {

					if (setOffsetToSymbol(secondWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);

						string padNum = padNumber(displ2, 3);
						result.append("8");
						result.append(checkReg("%pc"));
						result.append("00");
						result.append(padNum);
						return result;

					}
					

				}
				else if (checkIfSymbolIsExtern(secondWord)) {

					if (setOffsetToSymbol(secondWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(secondWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, "0", "R_X86_64_PC32", secondWord);
						this->relocationRecords->addLast(record);
						
						string padNum = padNumber(displ2, 3);
						result.append("8");
						result.append(checkReg("%pc"));
						result.append("00");
						result.append(padNum);
						return result;

					}
				}
			}
			return "";
		}
		case 24: {
			//beq
			if (isLiteral(fourthWord)) {

				if (!regex_match(fourthWord, val, hexRegex)) {
					fourthWord = decToHex(fourthWord);
					fourthWord = toUpperCase(fourthWord);
				}
				else {
					fourthWord = removeHexPrefix(fourthWord);
				}
				if (findAndCheckLiteral(fourthWord) != "") {
					result.append("39");
					result.append(checkReg("%pc"));
					result.append(checkReg(secondWord));
					result.append(checkReg(thirdWord));
					int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
					int displ = addr - locationCounter;
					string displ2 = decToHex(to_string(displ));
					displ2 = removeHexPrefix(displ2);
					displ2 = toUpperCase(displ2);
					
					string padNum = padNumber(displ2, 3);
					result.append(padNum);
				}
				else {
					result.append("31");
					result.append(checkReg("%pc"));
					result.append(checkReg(secondWord));
					result.append(checkReg(thirdWord));
					string padNum = padNumber(fourthWord, 3);
					result.append(padNum);
				}
				return result;
			}
			else if (isSymbol(fourthWord)) {
				result.append("3");
				int sectionNumber = getSectionNumberForSymbolName(fourthWord);
				int symbolRbr = getRbrForSymbolName(fourthWord);
				int offsetSym = getOffsetForSymbolName(fourthWord);
				string offset = decToHex(to_string(offsetSym));
				offset = removeHexPrefix(offset);
				offset = toUpperCase(offset);
				if (returnSectionName(sectionNumber) != currentSectionName && sectionNumber != 0) {

					if (setOffsetToSymbol(fourthWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);
						
						string padNum = padNumber(displ2, 3);
						result.append("9");
						result.append(checkReg("%pc"));
						result.append(checkReg(secondWord));
						result.append(checkReg(thirdWord));
						result.append(padNum);
						return result;

					}

				}
				else if (returnSectionName(sectionNumber) == currentSectionName) {

					if (setOffsetToSymbol(fourthWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);

						string padNum = padNumber(displ2, 3);
						result.append("9");
						result.append(checkReg("%pc"));
						result.append(checkReg(secondWord));
						result.append(checkReg(thirdWord));
						result.append(padNum);
						return result;

					}


				}
				else if (checkIfSymbolIsExtern(fourthWord)) {

					if (setOffsetToSymbol(fourthWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, "0", "R_X86_64_PC32", fourthWord);
						this->relocationRecords->addLast(record);
						string padNum = padNumber(displ2, 3);
						result.append("9");
						result.append(checkReg("%pc"));
						result.append(checkReg(secondWord));
						result.append(checkReg(thirdWord));
						result.append(padNum);
						return result;

					}
				}
			}
			return "";
		}
		case 25: {
			//bne
			if (isLiteral(fourthWord)) {

				if (!regex_match(fourthWord, val, hexRegex)) {
					fourthWord = decToHex(fourthWord);
					fourthWord = toUpperCase(fourthWord);
				}
				else {
					fourthWord = removeHexPrefix(fourthWord);
				}
				if (findAndCheckLiteral(fourthWord) != "") {
					result.append("3A");
					result.append(checkReg("%pc"));
					result.append(checkReg(secondWord));
					result.append(checkReg(thirdWord));
					int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
					int displ = addr - locationCounter;
					string displStr = to_string(displ);
					string padNum = padNumber(displStr, 3);
					result.append(padNum);
				}
				else {
					result.append("32");
					result.append(checkReg("%pc"));
					result.append(checkReg(secondWord));
					result.append(checkReg(thirdWord));
					string padNum = padNumber(fourthWord, 3);
					result.append(padNum);
				}
				return result;
			}
			else if (isSymbol(fourthWord)) {
				result.append("3");
				int sectionNumber = getSectionNumberForSymbolName(fourthWord);
				int symbolRbr = getRbrForSymbolName(fourthWord);
				int offsetSym = getOffsetForSymbolName(fourthWord);
				string offset = decToHex(to_string(offsetSym));
				offset = removeHexPrefix(offset);
				offset = toUpperCase(offset);
				if (returnSectionName(sectionNumber) != currentSectionName && sectionNumber != 0) {

					if (setOffsetToSymbol(fourthWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);
						string padNum = padNumber(displ2, 3);
						result.append("A");
						result.append(checkReg("%pc"));
						result.append(checkReg(secondWord));
						result.append(checkReg(thirdWord));
						result.append(padNum);
						return result;

					}

				}
				else if (returnSectionName(sectionNumber) == currentSectionName) {

					if (setOffsetToSymbol(fourthWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);
						string padNum = padNumber(displ2, 3);
						result.append("A");
						result.append(checkReg("%pc"));
						result.append(checkReg(secondWord));
						result.append(checkReg(thirdWord));
						result.append(padNum);
						return result;

					}


				}
				else if (checkIfSymbolIsExtern(fourthWord)) {

					if (setOffsetToSymbol(fourthWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, "0", "R_X86_64_PC32", fourthWord);
						this->relocationRecords->addLast(record);
						string padNum = padNumber(displ2, 3);
						result.append("A");
						result.append(checkReg("%pc"));
						result.append(checkReg(secondWord));
						result.append(checkReg(thirdWord));
						result.append(padNum);
						return result;

					}
				}
			}
			return "";
		}
		case 26: {
			//bgt
			if (isLiteral(fourthWord)) {

				if (!regex_match(fourthWord, val, hexRegex)) {
					fourthWord = decToHex(fourthWord);
					fourthWord = toUpperCase(fourthWord);
				}
				else {
					fourthWord = removeHexPrefix(fourthWord);
				}
				if (findAndCheckLiteral(fourthWord) != "") {
					result.append("3B");
					result.append(checkReg("%pc"));
					result.append(checkReg(secondWord));
					result.append(checkReg(thirdWord));
					int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
					int displ = addr - locationCounter;

					string displStr = to_string(displ);
					string padNum = padNumber(displStr, 3);
					result.append(padNum);
				}
				else {
					result.append("33");
					result.append(checkReg("%pc"));
					result.append(checkReg(secondWord));
					result.append(checkReg(thirdWord));
					string padNum = padNumber(fourthWord, 3);
					result.append(padNum);
				}
				return result;
			}
			else if (isSymbol(fourthWord)) {
				result.append("3");
				int sectionNumber = getSectionNumberForSymbolName(fourthWord);
				int symbolRbr = getRbrForSymbolName(fourthWord);
				int offsetSym = getOffsetForSymbolName(fourthWord);
				string offset = decToHex(to_string(offsetSym));
				offset = removeHexPrefix(offset);
				offset = toUpperCase(offset);
				if (returnSectionName(sectionNumber) != currentSectionName && sectionNumber != 0) {
					if (setOffsetToSymbol(fourthWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);
						string padNum = padNumber(displ2, 3);
						result.append("B");
						result.append(checkReg("%pc"));
						result.append(checkReg(secondWord));
						result.append(checkReg(thirdWord));
						result.append(padNum);
						return result;

					}
		
				}
				else if (returnSectionName(sectionNumber) == currentSectionName) {
					if (setOffsetToSymbol(fourthWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, offset, "R_X86_64_32", returnSectionName(sectionNumber));
						this->relocationRecords->addLast(record);
						string padNum = padNumber(displ2, 3);
						result.append("B");
						result.append(checkReg("%pc"));
						result.append(checkReg(secondWord));
						result.append(checkReg(thirdWord));
						result.append(padNum);
						return result;

					}


				}
				else if (checkIfSymbolIsExtern(fourthWord)) {
					if (setOffsetToSymbol(fourthWord)) {
						int addr = getOldSection()->getLiteralPool()->returnAddresForLiteral(fourthWord);
						string addr2 = decToHex(to_string(addr));
						addr2 = removeHexPrefix(addr2);
						addr2 = toUpperCase(addr2);
						int displ = addr - locationCounter;
						string displ2 = decToHex(to_string(displ));
						displ2 = removeHexPrefix(displ2);
						displ2 = toUpperCase(displ2);
						RelocationRecord* record = new RelocationRecord(currentSectionName, addr2, "0", "R_X86_64_PC32", fourthWord);
						this->relocationRecords->addLast(record);
						string padNum = padNumber(displ2, 3);
						result.append("B");
						result.append(checkReg("%pc"));
						result.append(checkReg(secondWord));
						result.append(checkReg(thirdWord));
						result.append(padNum);
						return result;

					}
				}
			}
			return "";
		}
	}


	return "";

}

bool Assembler::secondPass(vector<string> strings) {
	
	smatch val;
	
	
		for(string line : strings) {
			if (regex_match(line, val, label)) {
				continue;
			}
			
			string firstWord = getFirstWord(line);
			if (regex_match(firstWord, val, onlyDirectiveRegex)) {
				int dirNum = getNumberOfDirective(firstWord);
				if (dirNum == 0) return false;
				switch (dirNum) {
					//.global
				case 1: {
					vector<string> globalVars = extractGlobalVariables(line);
					for (auto& var : globalVars) {
						if (!regex_match(var, val, symbol)) return false;
						if (checkForSymbolInTable(var)) {
							setSymbolToBeGlobal(var);						
						}
						else {
							return false;
						}
					}
					break;
				}
					  //.extern
				case 2: {
					vector<string> externVars = extractExternVariables(line);
					for (auto& var : externVars) {
						if (!regex_match(var, val, symbol)) return false;

						if (checkForSymbolInTable(var)) {
							return false;
						}
						Symbol* newExternSymbol = new Symbol(var, 0, -1, true, false, true);
						symbolTable->addLast(newExternSymbol);
					}
					break;
				}
					  //.section
				case 3: {
					currentSection++;
					currentSectionName = getSectionName(line);
					locationCounter = 0;
					break;
				} //TODO azurirati currentSection na trenutnu sekciju 
					//.word
				case 4:
				{
					vector<string> wordVariables = extractWordVariables(line);
					for (auto& var : wordVariables) {
						if (!regex_match(var, val, symbol) && !regex_match(var, val, literal)) {
							return false;
						}

						if (regex_match(var, val, symbol)) {
							int sectionNumber = getSectionNumberForSymbolName(var);
							int symbolRbr = getRbrForSymbolName(var);
							int offsetSym = getOffsetForSymbolName(var);
							bool local = checkIfSymbolIsLocal(var);
							if (returnSectionName(sectionNumber) != currentSectionName && sectionNumber != 0) {
								string newLC = decToHex(to_string(locationCounter));
								newLC = removeHexPrefix(newLC);
								newLC = toUpperCase(newLC);
								string offsetSymNew = decToHex(to_string(offsetSym));
								offsetSymNew = removeHexPrefix(offsetSymNew);
								offsetSymNew = toUpperCase(offsetSymNew);
								RelocationRecord* record = new RelocationRecord(currentSectionName, newLC, offsetSymNew, "R_X86_64_32", returnSectionName(sectionNumber));
								this->relocationRecords->addLast(record);
								string bytes = "????????";
								insertIntoCurrentSectionBytes(bytes);
								
								locationCounter += 4;
							}
							else if (returnSectionName(sectionNumber) == currentSectionName) {
								string newLC = decToHex(to_string(locationCounter));
								newLC = removeHexPrefix(newLC);
								newLC = toUpperCase(newLC);
								string offsetSymNew = decToHex(to_string(offsetSym));
								offsetSymNew = removeHexPrefix(offsetSymNew);
								offsetSymNew = toUpperCase(offsetSymNew);
								RelocationRecord* record = new RelocationRecord(currentSectionName, newLC, offsetSymNew, "R_X86_64_32", returnSectionName(sectionNumber));
								this->relocationRecords->addLast(record);
								string bytes = "????????";
								insertIntoCurrentSectionBytes(bytes);

								locationCounter += 4;
							}
							else if (checkIfSymbolIsExtern(var) || checkIfSymbolIsGlobal(var)) {
								string newLC = decToHex(to_string(locationCounter));
								newLC = removeHexPrefix(newLC);
								newLC = toUpperCase(newLC);
								RelocationRecord* record = new RelocationRecord(currentSectionName, newLC, "0", "R_X86_64_PC32", var);
								this->relocationRecords->addLast(record);
								string bytes = "????????";
								insertIntoCurrentSectionBytes(bytes);
								
								locationCounter += 4;
								
							}
							
						}
						else {
							if (!regex_match(var, val, hexRegex)) {
								var = decToHex(var);
								var = toUpperCase(var);
							}
							else {
								var = removeHexPrefix(var);
							}
							string paddNumber = padNumber(var, 8);
							insertIntoCurrentSectionBytes(paddNumber);
							locationCounter += 4;
						}
					}
					
					break;
				}
				//.skip
				case 5: {
					string skipString = "";
					int numOfBytes = getSkipBytes(line);
					for (int i = 0; i < numOfBytes; i++) {
						skipString.append("00");
					}
					insertIntoCurrentSectionBytes(skipString);
					locationCounter += numOfBytes;
					break;
				}
					  //.end
					  //isto kao prvi
				case 6: break;
				}
			}
			else if (regex_match(firstWord, val, onlyInstructionRegex)) {
				string bytes = resolveInstruction(line);
				insertIntoCurrentSectionBytes(bytes);
				//srediti LC
				if (regex_match(line, val, loadLiteralFromMemory) || regex_match(line, val, loadSymbolFromMemory)) locationCounter += 8;
				else locationCounter += 4;

			}
		
		
	
	}
	return true;
}


void Assembler::clearTable() {
	int i = 0;
	while (i < symbolTable->getSize()) {
		symbolTable->removeFirst();
		i++;
	}
	while (i < sectionTable->getSize()) {
		sectionTable->removeFirst();
		i++;
	}
}

string Assembler::removeExtraSpaces(string line) {
	string res;
	bool isFirstSpace = true;
	bool hasExtraSpaces = false;

	for (char c : line) {
		if (c != ' ') {
			res += c;
			isFirstSpace = true;
		} else {
			if (isFirstSpace) {
				res += c;
				isFirstSpace = false;
			}
			else {
				hasExtraSpaces = true;
			}
		}
	}

	if (hasExtraSpaces) return res;
	else return "";
}

bool Assembler::checkIfIsEnd(string line) {
	smatch val; 
	if (regex_match(line, val, endd)) {
		return true;
	}
	else return false;
}

void Assembler::writeInFile(string filename) {
	ofstream out_file;
	out_file.open(filename);

	printSymbolTable(out_file);
	printSectionTable(out_file);
	printRelocationRecords(out_file);
	printBytesInCode(out_file);

	out_file.close();
}

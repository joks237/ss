#include "../inc/parser.h"

regex symbolTable("Tabela simbola:");
regex sectionTable("Tabela sekcija:");
regex relocationTable("Relokacioni zapisi:");
regex literalPool("Bazen literala:");
regex byteCode("Bajt kod:");
regex endOfSymbolTable("Kraj tabele simbola.");
regex endOfSectionTable("Kraj tabele sekcija.");
regex endOfRelocationRecord("Kraj relokacionih zapisa.");
regex endOfByteCodes("Kraj ispisa bajt koda.");
regex endOfLiteralPool("Kraj bazena literala.");
regex delimiter("\\s*\\|\\s*");

vector<string> Parser::tokens(string line) {
    std::istringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (ss >> token) {

        tokens.push_back(token);
    }

    return tokens;
}

Section* Parser::getSectionByName(string name) {
    int i = 0;
    for (i; i < listOfSections->getSize(); i++) {
        if (listOfSections->getWithoutRemovingFromList(i)->getName() == name) return listOfSections->getWithoutRemovingFromList(i);
    }
    return nullptr;
}

string Parser::getSectionNameForNumber(int number, string filename) {
    int size = listOfSections->getSize();
    for (int i = 0; i < size; i++) {
        if (listOfSections->getWithoutRemovingFromList(i)->getIdSection() == number && listOfSections->getWithoutRemovingFromList(i)->getFilename() == filename)
            return listOfSections->getWithoutRemovingFromList(i)->getName();
    }
    return "";
}

void Parser::resolveSymbolSections() {
    int size = listOfSymbols->getSize();
    for (int i = 0; i < size; i++) {
        int sectionNum = listOfSymbols->getWithoutRemovingFromList(i)->getSection();
        string filename = listOfSymbols->getWithoutRemovingFromList(i)->getFilename();
        string sectionName = getSectionNameForNumber(sectionNum, filename);
        listOfSymbols->getWithoutRemovingFromList(i)->setSectionName(sectionName);
    }
}




bool Parser::processFile(string filename) {

	std::ifstream inputFile(filename);
	if (!inputFile.is_open()) {
		std::cout << "Wrong File Name" << std::endl;
		return false;
	}
    bool isProcessingSymbolTable = false;
    bool isProcessingSectionTable = false;
    bool isProcessingRelocationTable = false;
    bool isProcessingByteCode = false;
    bool isProcessingLiteralPool = false;
    bool shouldSkipNextLine = false;
    bool getSectionName = false;
    bool getByteCodeName = false;
	std::string line;
    string sectionNameForPool;
    string sectionNameForByteCode;
    Section* section = nullptr;

	while (std::getline(inputFile, line)) {
        if (line.empty() || line.find_first_not_of(' ') == std::string::npos) {
            continue;  // Skip empty lines
        }
        if (shouldSkipNextLine) {
            shouldSkipNextLine = false; // Reset the flag
            continue; // Skip to the next iteration of the loop
        }

        if (getByteCodeName) {
            getByteCodeName = false;
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                sectionNameForByteCode = line.substr(0, colonPos);
            }
            continue;
        }
        if (getSectionName) {
            getSectionName = false;
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                sectionNameForPool = line.substr(0, colonPos);
            }
            shouldSkipNextLine = true;
            continue;
        }

        if (std::regex_search(line, sectionTable)) {
            
            isProcessingSectionTable = true;
            shouldSkipNextLine = true;
            continue;
        }

        if (std::regex_search(line, symbolTable)) {
            
            isProcessingSymbolTable = true;
            shouldSkipNextLine = true;
            continue;
        }

        if (std::regex_search(line, relocationTable)) {
            
            isProcessingRelocationTable = true;
            shouldSkipNextLine = true;
            continue;
        }

        if (std::regex_search(line, literalPool)) {
            
            isProcessingLiteralPool = true;
            getSectionName = true;
            continue;
        }

        if (std::regex_search(line, byteCode)) {
            
            isProcessingByteCode = true;
            getByteCodeName = true;
            continue;
        }

        if (isProcessingSymbolTable) {
           
            if (std::regex_search(line, endOfSymbolTable)) {
           
                isProcessingSymbolTable = false;
                continue;
            }

            vector<string> tokensFromLine = tokens(line);

            if (tokensFromLine.empty()) {
                cout << "PRAZNOOO " << line << endl;
            }
           
            Symbol* symbol = new Symbol(std::stoi(tokensFromLine.at(0)),tokensFromLine.at(1), std::stoi(tokensFromLine.at(2)), std::stoi(tokensFromLine.at(3)), std::stoi(tokensFromLine.at(4)), filename);
            
                  listOfSymbols->addLast(symbol);
        }
        else if (isProcessingSectionTable) {
           

            if (std::regex_search(line, endOfSectionTable)) {
           
                isProcessingSectionTable = false;
                continue;
            }

            vector<string> tokensFromLine = tokens(line);
       
                Section* section = new Section(std::stoi(tokensFromLine.at(0)), tokensFromLine.at(1), std::stoi(tokensFromLine.at(2)), std::stoi(tokensFromLine.at(3)), std::stoi(tokensFromLine.at(4)), std::stoi(tokensFromLine.at(5)), filename);
                listOfSections->addLast(section);
            
        }
        else if (isProcessingRelocationTable) {
           
            if (std::regex_search(line, endOfRelocationRecord)) {
           
                isProcessingRelocationTable = false;
                continue;
            }
            vector<string> tokensFromLine = tokens(line);
            RelocationRecord* record = new RelocationRecord(std::stoi(tokensFromLine.at(0)),tokensFromLine.at(1), tokensFromLine.at(2),tokensFromLine.at(3),  tokensFromLine.at(4),tokensFromLine.at(5), filename);
                listOfRelocationRecords->addLast(record);



        }
        else if (isProcessingLiteralPool) {
            
            vector<string> tokensFromLine = tokens(line);

            if (std::regex_search(line, endOfLiteralPool)) {
                isProcessingLiteralPool = false;
                continue;
            }

            section = getSectionByName(sectionNameForPool);
            Literal* lit = new Literal(std::stoi(tokensFromLine.at(1)), tokensFromLine.at(0), tokensFromLine.at(2));
            if (section != nullptr) {
                section->getLiteralPool()->addLiteralToList(lit);
            }
            
        }
        else if (isProcessingByteCode) {
            
            if (std::regex_search(line, endOfByteCodes)) {
                
                isProcessingByteCode = false;
                continue;
            }

            section = getSectionByName(sectionNameForByteCode);
                        
            section->insertIntoBytes(line);
        }

        resolveSymbolSections();

	}

	inputFile.close();
	return true;
}
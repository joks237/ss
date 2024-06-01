#include <iostream>
#include <fstream>
#include <string>
#include "../inc/assembler.h"
#include "../inc/exception.h"
using namespace std;


int main(int argc, char* argv[]) {

    char* out_file, *in_file;
    string option = argv[1];
    if (option == "-o") {
       out_file = argv[2];
       in_file = argv[3];
    }
    else {
       cout << "GRESKA PRI OTVARANJU FAJLA "<<endl;
       exit(-1);
    }
    Exception exception;
  
    std::ifstream inputFile(in_file); // Replace "input.txt" with your file path
    vector<string> outputFileStrings;
    if (!inputFile.is_open()) {
        std::cout << exception.wrongFileName() << std::endl;
        return 1;
    }
    
    Assembler a;

    std::string line;
    std::string comment = "";
    std::string label;
    std::string oldLine = "";
    bool isEnd = false;
    a.insertIntoSectionTableBeforeFirstPassUND();
    while (std::getline(inputFile, line)) {
        if (!isEnd) {
            if (line.empty() || line.find_first_not_of(' ') == std::string::npos) {
                continue;  // Skip empty lines
            }

            if (a.removeLeadingSpaces(line) != "") line = a.removeLeadingSpaces(line);

            if (a.removeExtraSpaces(line) != "") line = a.removeExtraSpaces(line);
            //za komentare
            size_t commentPos = line.find('#');

            if (commentPos != string::npos) {
                comment = line.substr(commentPos + 1);
                line = line.substr(0, commentPos);
            }

            //ako je komentar odmah na pocetku, preskoci obradu 
            if (commentPos == 0) continue;


            line = a.removeTrailingWhitespaces(line);
            if (a.prettifyString(line) != "") line = a.prettifyString(line);

            //        cout << "Line: " << line << endl;

            if (a.checkForMoreThanOneDirectiveInLine(line)) {
                cout << exception.moreThanOneDirectiveException() << line << endl;
                return -1;
            }

            if (a.checkIfThereIsAInstructionBeforeLabel(line)) {
                cout << exception.instructionBeforeLabel() << line << endl;
                return -1;
            }

            //u slucaju da posle labele ima neka instrukcija 
            if (a.extractAfterColon(line) != "") {
                label = a.extractLabel(line);
                if (!a.checkIfRegexMatch(label)) {
                    std::cout << exception.syntaxException() << endl;
                    return -1;
                }
                else {

                    if (!a.firstPass(label)) {
                        cout << exception.firstPassException() << endl;
                        return -1;
                    }
                    else {
                        outputFileStrings.push_back(label);
                    }
                }
                line = a.extractAfterColon(line);
                line = a.removeLeadingSpaces(line);
                if (a.removeExtraSpaces(line) != "") line = a.removeExtraSpaces(line);
            }

            if (a.checkIfIsEnd(line)) isEnd = true;

            // Process each line here
            if (!a.checkIfRegexMatch(line)) {
                std::cout << exception.syntaxException() << endl;
                return -1;
            }


            if (!a.firstPass(line)) {
                cout << exception.firstPassException() << endl;
                return -1;
            }

            outputFileStrings.push_back(line);
        }
        else {
            cout << exception.instructionsAfterEnd() << endl;
            break;
        }
    }
    //for (string s : outputFileStrings) {
    //    cout << s << endl;
    //}
    if (!a.secondPass(outputFileStrings)) {
        cout << exception.secondPassException() <<endl;
        return -1;
   }
    a.writeInFile(out_file);
    inputFile.close(); // Close the file after reading
 

	return 0;
}
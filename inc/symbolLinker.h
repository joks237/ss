#ifndef SYMBOL_LINKER_HPP
#define SYMBOL_LINKER_HPP
#include <string>
#include <stdio.h>
using namespace std;

class Symbol {
private:
    string name;
    int number = 0;
    int section;
    int offset;
    static int size;
    string filename;
    bool isGlobal;
    bool isLocal;
    bool isExtern;
    string sectionName;
    bool proccessed;

public:

    Symbol() : number(size++) {}

    Symbol(int id, string name, int section, int offset, bool isGlobal, string filename);

    // Getters
    int getNumber() const {
        return number;
    }

    int getSection() const {
        return section;
    }

    string getFilename() const {
        return filename;
    }

    int getOffset() const {
        return offset;
    }

    int getSize() const {
        return size;
    }

    string getName() const {
        return name;
    }

    string getSectionName() const {
        return sectionName;
    }

    bool getIsGlobal() const {
        return isGlobal;
    }

    bool getIsLocal() const {
        return isLocal;
    }

    bool getIsExtern() const {
        return isExtern;
    }

    // Setters
    void setNumber(int value) {
        number = value;
    }

    void setSection(int value) {
        section = value;
    }

    void setOffset(int value) {
        offset = value;
    }

    void setSize(int value) {
        size = value;
    }

    void setName(const string value) {
        name = value;
    }

    void setSectionName(const string value) {
        sectionName = value;
    }

    void setIsGlobal(bool value) {
        isGlobal = value;
    }

    void setIsLocal(bool value) {
        isLocal = value;
    }

    void setIsExtern(bool value) {
        isExtern = value;
    }

    bool getProccessed() {
        return proccessed;
    }

    void setProccessed(bool val) {
        proccessed = val;
    }
};
#endif


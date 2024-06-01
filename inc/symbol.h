#ifndef SYMBOL_H
#define SYMBOL_H

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

	bool isGlobal;
	bool isLocal;
	bool isExtern;

public:

    Symbol() : number(size++){}

	Symbol(string name, int section, int offset, bool isGlobal, bool isLocal, bool isExtern);

    // Getters
    int getNumber() const {
        return number;
    }

    int getSection() const {
        return section;
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

    void setIsGlobal(bool value) {
        isGlobal = value;
    }

    void setIsLocal(bool value) {
        isLocal = value;
    }

    void setIsExtern(bool value) {
        isExtern = value;
    }
};
#endif
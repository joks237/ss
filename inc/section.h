#ifndef SECTION_H
#define SECTION_H

#include <stdio.h>
#include <string>
#include <vector>
#include "literalPool.h"
using namespace std;

class Section {
private:
	string name;
	int size;
	static int ID;
	int idSection;
    vector<string> bytes;
    LiteralPool* literalPool;
public:
	Section() {};
	Section(string name, int size);
    // Getter for 'name'
    std::string getName() const {
        return name;
    }

    // Setter for 'name'
    void setName(const std::string& newName) {
        name = newName;
    }

    // Getter for 'size'
    int getSize() const {
        return size;
    }

    // Setter for 'size'
    void setSize(int newSize) {
        size = newSize;
    }

    // Getter for 'idSection'
    int getIdSection() const {
        return idSection;
    }

    // Setter for 'idSection'
    void setIdSection(int newIdSection) {
        idSection = newIdSection;
    }

    vector<string> getBytes() {
        return bytes;
    }

    void insertIntoBytes(string content) {
        bytes.push_back(content);
    }

    LiteralPool* getLiteralPool() {
        return this->literalPool;
    }
	
};
#endif
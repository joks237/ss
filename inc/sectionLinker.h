#ifndef SECTION_LINKER_HPP
#define SECTION_LINKER_HPP
#include <stdio.h>
#include <string>
#include <vector>
#include "literalPoolLinker.h"
using namespace std;

class Section {
private:
    string name;
    int size;
    static int ID;
    int idSection;
    int literalPoolBeginig;
    int literalPoolSize;
    int wholeSectionSize;
    vector<string> bytes;
    LiteralPool* literalPool;
    string filename;
public:
    Section() {};
    Section(int id, string name, int size, int literalPoolBeginig, int literalPoolSize, int wholeSectionSize, string filename);
    // Getter for 'name'
    std::string getName() const {
        return name;
    }

    std::string getFilename() const {
        return filename;
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

    void setBytes(string content) {
        bytes.clear();
        bytes.push_back(content);
    }

    LiteralPool* getLiteralPool() {
        return this->literalPool;
    }

    int getWholeSectionSize() {
        return this->wholeSectionSize;
   }

    void setWholeSectionSize(int size) {
         this->wholeSectionSize = size;
    }

    int getLiteralPoolSize() {
        return literalPoolSize;
    }
    void setLiteralPoolSize(int litPoolSize) {
        literalPoolSize = litPoolSize;
    }
};
#endif
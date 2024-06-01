#ifndef RELOCATIONRECORDS_LINKER_HPP
#define RELOCATIONRECORDS_LINKER_HPP
#include <stdio.h>
#include <string>
using namespace std;

class RelocationRecord {
private:
    static int ID;
    int rrId;
    string section;
    string offset;
    string type_of_relocation;
    string addend;
    string filename;
    string symbolName;
    string symbolRbr;
    bool razresen;

public:

    RelocationRecord(int id, string section, string offset, string type_of_relocation, string symbolRbr, string addend, string filename );
    // Getters
    int getID() const {
        return ID;
    }

    int getRRId() const {
        return rrId;
    }

    std::string getSection() const {
        return section;
    }

    string getOffset() const {
        return offset;
    }

    std::string getTypeOfRelocation() const {
        return type_of_relocation;
    }

    string getAddend() const {
        return addend;
    }

    string getFilename() const {
        return filename;
    }

    string getSymbolRbr() const {
        return symbolRbr;
    }

    // Setters
    void setID(int id) {
        ID = id;
    }

    void setRRId(int rrId) {
        this->rrId = rrId;
    }

    void setSection(const std::string& sec) {
        section = sec;
    }

    void setOffset(string off) {
        offset = off;
    }

    void setTypeOfRelocation(const std::string& type) {
        type_of_relocation = type;
    }

    void setAddend(string add) {
        addend = add;
    }

    void setSymbolName(string name) {
        symbolName = name;
    }

    string getSymbolName() {
        return symbolName;
    }

    void setRazresen(bool raz) {
        this->razresen = raz;
    }

    bool getRazresen() {
        return this->razresen;
    }
};
#endif

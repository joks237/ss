#include "../inc/relocationRecordLinker.h"

int RelocationRecord::ID = 0;

RelocationRecord::RelocationRecord(int id,string section, string offset, string type_of_relocation, string symbolRbr, string addend, string filename) : rrId(id) ,section(section), offset(offset),  type_of_relocation(type_of_relocation), symbolRbr(symbolRbr), addend(addend), filename(filename) {
	this->razresen = false;
}

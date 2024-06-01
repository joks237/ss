#include "../inc/relocationRecord.h"

int RelocationRecord::ID = 0;

RelocationRecord::RelocationRecord(string section, string offset, string addend, string type_of_relocation, string symbolRbr) : section(section), offset(offset), addend(addend), rrId(ID++), type_of_relocation(type_of_relocation), symbolRbr(symbolRbr) {}

#include "../inc/symbol.h"

int Symbol::size = 0;

Symbol::Symbol(string name, int section, int offset, bool isGlobal, bool isLocal, bool isExtern) 
	: name(name), section(section), offset(offset), isGlobal(isGlobal), isLocal(isLocal), isExtern(isExtern), number(size++)
{
		
}
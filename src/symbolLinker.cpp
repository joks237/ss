#include "../inc/symbolLinker.h"



Symbol::Symbol(int id, string name, int section, int offset, bool isGlobal, string filename)
	: number(id), name(name), section(section), offset(offset), isGlobal(isGlobal), filename(filename)
{
	proccessed = false;
}
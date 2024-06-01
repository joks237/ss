#include "../inc/sectionLinker.h"

int Section::ID = 0;

Section::Section(int id, string name, int size,int literalPoolBeginig, int literalPoolSize, int wholeSectionSize, string filename) : idSection(id), name(name), size(size), literalPoolBeginig(literalPoolBeginig), literalPoolSize(literalPoolSize), wholeSectionSize(wholeSectionSize), filename(filename) {
	literalPool = new LiteralPool(name);
	
}
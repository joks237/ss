#include "../inc/section.h"

int Section::ID = 0;

Section::Section(string name, int size) : name(name), size(size) {
	literalPool = new LiteralPool(name);
	idSection = ID++;
}
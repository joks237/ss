#ifndef EMULATOR_HPP
#define EMULATOR_HPP
#include <stdio.h>
#include <array>
#include <unordered_map>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

typedef unsigned long long ull;


class Emulator {

private:
	
	static int  numElements;
	std::map<unsigned int, string> memory;
	array<ull, 16> registers;
	array<ull, 16> csregisters;
	array<string, 16> printRegister;
	string inputFileName;
	bool haltHasCome;


public:
	Emulator() {}
	Emulator(string inputFile);
	void insertFromFile();
	unsigned int hexToDecimal(string hex_val);
	void proccessMemory();
	void resolveInstruction();
	string decToHex(int dec_val);
	int countBytes(string hexString);
	std::string convertToLittleEndian(const std::string& hexString);
	string padNumber(string number, int digits);
	int checkReg(char reg);
	void printRegisters();
	bool isNegative(ull val);
	bool isNegativeDispl(string val);
	long long negative(string val);
	int negativeDispl(string displ);

	void test(ull reg1, ull reg2, string val3); //radi test za negativne vrednosti !!! 
	void printMem();
};

#endif

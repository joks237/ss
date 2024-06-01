#include "../inc/emulator.h"
#include <algorithm>
#include <bitset>

int Emulator::numElements = 4294967295;


Emulator::Emulator(string inputFile) : inputFileName(inputFile) {

	for (int i = 0; i < 16; i++) {
		registers[i] = 0;
	}
	for (int i = 0; i < 3; i++) {
		csregisters[i] = 0;
	}
	insertFromFile();
	auto it = memory.begin();
	unsigned int key;
	if (it != memory.end()) {
		key = it->first;
	}
	registers[15] = key;
	registers[14] = 4294967040;
	haltHasCome = false;
}

unsigned int Emulator::hexToDecimal(string hex_val) {
	unsigned int dec_val;
	stringstream stream;
	stream << std::hex << hex_val;
	stream >> dec_val;
	return dec_val;
}

void Emulator::insertFromFile() {
	ifstream inputFile(inputFileName);
	if (!inputFile.is_open()) {
		std::cout << "Unable to open file" << endl;
		return;
	}
	string line;
	while (std::getline(inputFile, line)) {
		if (line.empty() || line.find_first_not_of(' ') == std::string::npos) {
			continue;  // Skip empty lines
		}

		std::size_t colonPos = line.find(':');


		std::string firstValue = line.substr(0, colonPos);


		std::string sequence = line.substr(colonPos + 1);

	
		sequence.erase(0, sequence.find_first_not_of(" "));
		sequence.erase(sequence.find_last_not_of(" ") + 1);

		sequence.erase(std::remove(sequence.begin(), sequence.end(), ' '), sequence.end());


		unsigned int decimal = hexToDecimal(firstValue);
		int inputLength = sequence.length();
		if (inputLength == 16) {
			for (std::size_t i = 0; i < inputLength; i += 2) {
				std::string twoChars = sequence.substr(i, 2);
				memory[decimal] = twoChars;
				decimal++;
			}
		}
		else {
			for (std::size_t i = 0; i < inputLength; i += 2) {
				std::string twoChars = sequence.substr(i, 2);
				memory[decimal] = twoChars;
				decimal++;
			}
			decimal += 4;
		}
	}


	inputFile.close();
}


void Emulator::proccessMemory() {
	
	while (!this->haltHasCome) {
		resolveInstruction();
	}
	printRegisters();
}

int Emulator::checkReg(char reg) {
	switch (reg) {
		case '0': {
			return 0;
		}
		case '1': {
			return 1;
		}
		case '2': {
			return 2;
		}
		case '3': {
			return 3;
		}
		case '4': {
			return 4;
		}
		case '5': {
			return 5;
		}
		case '6': {
			return 6;
		}
		case '7': {
			return 7;
		}
		case '8': {
			return 8;
		}
		case '9': {
			return 9;
		}
		case 'A': {
			return 10;
		}
		case 'B': {
			return 11;
		}
		case 'C': {
			return 12;
		}
		case 'D': {
			return 13;
		}
		case 'E': {
			return 14;
		}
		case 'F': {
			return 15;
		}
	}
}
string Emulator::decToHex(int dec_val) {
	stringstream stream;
	stream << uppercase << std::hex << dec_val;
	return stream.str();
}

std::string Emulator::convertToLittleEndian(const std::string& hexString) {
	std::string littleEndian;
	size_t length = hexString.length();

	// Process the string in pairs of two characters
	for (size_t i = 0; i < length; i += 2) {
		std::string pair = hexString.substr(i, 2);  // Get the current pair

		// Insert the pair at the beginning of the result string
		littleEndian.insert(0, pair);
	}

	return littleEndian;
}

string Emulator::padNumber(string number, int digits) { //ako se nesto zezne u asembleru za paddovanje, ovo je ispravna funkcija. Do sad je radilo jer nijedan pomeraj nije bio negativan! 
	if (number.length() >= digits) {
		return number.substr(number.length() - digits);
	}
	else {
		string newString = std::string(digits - number.length(), '0') + number;
		return newString;
	}
}

int Emulator::countBytes(string hexString) {
	int numChars = hexString.length();
	int numBytes = numChars / 2; // Each byte corresponds to 2 characters

	if (numChars % 2 != 0) {
		// If the number of characters is odd, we have an incomplete byte
		numBytes++;
	}

	return numBytes;

}


bool Emulator::isNegativeDispl(string val) {
	string padded = padNumber(val, 3);
	std::istringstream iss(padded);
	unsigned int number;
	iss >> std::hex >> number;
	return (number > 0x7FF);

}

bool Emulator::isNegative(ull val) {
	string hex = decToHex(val);
	string padded = padNumber(hex, 8);
    // Convert hexadecimal string to binary string
    std::string binaryString = std::bitset<32>(std::stoll(padded, nullptr, 32)).to_string();
		cout << binaryString << endl;
		cout << binaryString[0] << endl;
    // Check if the first bit is one
    return (binaryString[0] == '1');
}

long long Emulator::negative(std::string val) {
    try {
        long long result = std::stoll(val, nullptr, 16);
        if (result > 0x7FFFFFFF) {
            result -= 0x100000000;
        }
        return result;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;  // Rethrow the exception for handling at the caller's level
    }
}

int Emulator::negativeDispl(string displ) {

	try {
		int decimalValue = std::stoi(displ, nullptr, 16);

		if (decimalValue > 0x7FF) {

			decimalValue = static_cast<int>(static_cast<unsigned int>(decimalValue) | 0xFFFFF800);
		}

		return decimalValue;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		throw;  // Rethrow the exception for handling at the caller's level
	}
}


void Emulator::resolveInstruction() {

	string instructionCode = memory[registers[15]];
	char opCode = instructionCode[0];
	char mod = instructionCode[1];
	string regAB = memory[registers[15] + 1];
	char regA = regAB[0];
	char regB = regAB[1];
	string regCDispl = memory[registers[15] + 2];
	char regC = regCDispl[0];
	char displ = regCDispl[1];
	string displacement = memory[registers[15] + 3];
	displacement.insert(0, 1, displ);


	switch(opCode) {
	
		case '0': //halt
		{
			
			this->haltHasCome = true;
			registers[15] += 4;
			break;
		}
		
		case '1': //int
		{
			csregisters[2] = 4;
			//push status
			registers[14] -= 4;
			string hex = decToHex(csregisters[0]);
			memory[registers[14]] = decToHex(csregisters[0]);
			//push pc
			registers[14] -= 4;
			registers[15] += 4; // sledeca instrukcija 
			memory[registers[14]] = decToHex(registers[15]);
			ull invert = csregisters[0] & (~01);
			csregisters[0] = invert;
			registers[15] = csregisters[1];

			break;
		}
		case '2': //call
		{
			if (mod == '0') { //bez poola
				int registerA = checkReg(regA);
				registers[14] -= 4;
				registers[registerA] += 4; // sledeca instrukcija 
				memory[registers[14]] = decToHex(registers[registerA]);
				registers[registerA] = hexToDecimal(displacement);
			}
			else { //pool
				//push pc
				int registerA = checkReg(regA);
				registers[14] -= 4;

				///upisi novu vrednost u pc iz poola 
				int decimal = hexToDecimal(displacement);
				ull displToPool = registers[registerA] + decimal;

				registers[registerA] += 4; // sledeca instrukcija 
				memory[registers[14]] = decToHex(registers[registerA]);

				int pc = displToPool;
				string value1 = memory[pc];
				string value2 = memory[pc + 1];
				string value3 = memory[pc + 2];
				string value4 = memory[pc + 3];

				string newAddr = value4.append(value3).append(value2).append(value1);

				unsigned int dec = hexToDecimal(newAddr);
				registers[registerA] = dec;

		
			}
			break;
		}
		case '3':  //skokovi
		{
			if (mod == '0') { //jmp bez poola
				int registerA = checkReg(regA);
				int displ = hexToDecimal(displacement);
				registers[registerA] = displ;
			}
			else if (mod == '1') { //beq bez poola
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int registerC = checkReg(regC);
				if (registers[registerB] == registers[registerC]) {
					int decimal = hexToDecimal(displacement);

					registers[registerA] = decimal;
				}
				else {
					registers[registerA] += 4;
				}
			}
			else if (mod == '2') { //bne bez poola
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int registerC = checkReg(regC);
				if (registers[registerB] != registers[registerC]) {
					int decimal = hexToDecimal(displacement);

					registers[registerA] = decimal;
				}
				else {
					registers[registerA] += 4;
				}
			}
			else if (mod == '3') { //bgt bez poola
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int registerC = checkReg(regC);
				long trueRegisterB;
				long trueRegisterC;

				if (isNegative(registers[registerB])) {
					string hexValB = decToHex(registers[registerB]);
					string paddedHexValB = padNumber(hexValB, 8);
					trueRegisterB = negative(paddedHexValB);
					if (isNegative(registers[registerC])) {
						string hexValC = decToHex(registers[registerB]);
						string paddedHexValC = padNumber(hexValC, 8);
						trueRegisterC = negative(paddedHexValC);
					}
					else {
						trueRegisterC = registers[registerC];
					}
				}
				else if (isNegative(registers[registerC])) {
					string hexValC = decToHex(registers[registerB]);
					string paddedHexValC = padNumber(hexValC, 8);
					trueRegisterC = negative(paddedHexValC);
					trueRegisterB = registers[registerB];
				}
				else {
					trueRegisterB = registers[registerB];
					trueRegisterC = registers[registerC];
				}

				if (trueRegisterB > trueRegisterC) {
					int decimal = hexToDecimal(displacement);

					registers[registerA] = decimal;
				}
				else {
					registers[registerA] += 4;
				}
			}
			else if (mod == '8') { //jmp pool

				int registerA = checkReg(regA);

			
					int decimal = hexToDecimal(displacement);
					ull displToPool = registers[registerA] + decimal;

					int pc = displToPool;
					string value1 = memory[pc];
					string value2 = memory[pc + 1];
					string value3 = memory[pc + 2];
					string value4 = memory[pc + 3];

					string newAddr = value4.append(value3).append(value2).append(value1);

					unsigned int dec = hexToDecimal(newAddr);
					registers[registerA] = dec;

				
			}
			else if (mod == '9') { //beq pool
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int registerC = checkReg(regC);
				if (registers[registerB] == registers[registerC]) {
					int decimal = hexToDecimal(displacement);
					ull displToPool = registers[registerA] + decimal;

					int pc = displToPool;
					string value1 = memory[pc];
					string value2 = memory[pc + 1];
					string value3 = memory[pc + 2];
					string value4 = memory[pc + 3];

					string newAddr = value4.append(value3).append(value2).append(value1);

					unsigned int dec = hexToDecimal(newAddr);
					registers[registerA] = dec;
				}
				else {
					registers[registerA] += 4;
				}
			}
			else if (mod == 'A') { //bne pool
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int registerC = checkReg(regC);
				if (registers[registerB] != registers[registerC]) {
					int decimal = hexToDecimal(displacement);
					ull displToPool = registers[registerA] + decimal;

					int pc = displToPool;
					string value1 = memory[pc];
					string value2 = memory[pc + 1];
					string value3 = memory[pc + 2];
					string value4 = memory[pc + 3];

					string newAddr = value4.append(value3).append(value2).append(value1);

					unsigned int dec = hexToDecimal(newAddr);
					registers[registerA] = dec;
				}
				else {
					registers[registerA] += 4;
				}
			}
			else if (mod == 'B') { //bgt pool
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int registerC = checkReg(regC);
				long trueRegisterB;
				long trueRegisterC;

				if (isNegative(registers[registerB])) {
					string hexValB = decToHex(registers[registerB]);
					string paddedHexValB = padNumber(hexValB, 8);
					trueRegisterB = negative(paddedHexValB);
					if (isNegative(registers[registerC])) {
						string hexValC = decToHex(registers[registerB]);
						string paddedHexValC = padNumber(hexValC, 8);
						trueRegisterC = negative(paddedHexValC);
					}
					else {
						trueRegisterC = registers[registerC];
					}
				}
				else if (isNegative(registers[registerC])) {
					string hexValC = decToHex(registers[registerB]);
					string paddedHexValC = padNumber(hexValC, 8);
					trueRegisterC = negative(paddedHexValC);
					trueRegisterB = registers[registerB];
				}
				else {
					trueRegisterB = registers[registerB];
					trueRegisterC = registers[registerC];
				}


				if (trueRegisterB > trueRegisterC) {
					int decimal = hexToDecimal(displacement);
					ull displToPool = registers[registerA] + decimal;

					int pc = displToPool;
					string value1 = memory[pc];
					string value2 = memory[pc + 1];
					string value3 = memory[pc + 2];
					string value4 = memory[pc + 3];

					string newAddr = value4.append(value3).append(value2).append(value1);

					unsigned int dec = hexToDecimal(newAddr);
					registers[registerA] = dec;
				}
				else {
					registers[registerA] += 4;
				}
			}
			else {
				return;
			}
			break;
		}
		case '4': //xchg
		{
			
			int registarB = checkReg(regB);
			int registarC = checkReg(regC);
			ull temp = registers[registarB];
			registers[registarB] = registers[registarC];
			registers[registarC] = temp;
			registers[15] += 4;
			break;
		}
		case '5': //aritmeticke
		{
			int registarA = checkReg(regA);
			int registarB = checkReg(regB);
			int registarC = checkReg(regC);
			if (mod == '0') { //add
				registers[registarA] = registers[registarB] + registers[registarC];
		
			}
			else if (mod == '1') { //sub
				registers[registarA] = registers[registarB] - registers[registarC];
			
			}
			else if (mod == '2') { //mul
				registers[registarA] = registers[registarB] * registers[registarC];
	
			}
			else if (mod == '3') { //div
				registers[registarA] = registers[registarB] / registers[registarC];
				
			}
			registers[15] += 4;
			break;
		}
		case '6': //logicke
		{
			int registarA = checkReg(regA);
			int registarB = checkReg(regB);
			int registarC = checkReg(regC);
			if (mod == '0') { //not
				registers[registarA] = ~registers[registarB];
			}
			else if (mod == '1') { //and
				registers[registarA] = registers[registarB] & registers[registarC];
			}
			else if (mod == '2') { //or
				registers[registarA] = registers[registarB] | registers[registarC];
			}
			else if (mod == '3') { //xor
				registers[registarA] = registers[registarB] ^ registers[registarC];
			}
			registers[15] += 4;
			break;
		}
		case '7': //shiftovi
		{
			int registarA = checkReg(regA);
			int registarB = checkReg(regB);
			int registarC = checkReg(regC);

			if (mod == '0') { //shl
				registers[registarA] = registers[registarB] << registers[registarC];
			}
			else if (mod == '1') { //shr
				registers[registarA] = registers[registarB] >> registers[registarC];
			}
			registers[15] += 4;
			break;
		}
		case '8': //store
		{
			if (mod == '0') { //reg u mem
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int registerC = checkReg(regC);
				if (displacement == "000") {
					string hex = decToHex(registers[registerC]);
					string padNum = padNumber(hex, 8);
					string littleEndian = convertToLittleEndian(padNum);
					int decimal = 0;
					for (std::size_t i = 0; i < 8; i += 2) {
						std::string twoChars = littleEndian.substr(i, 2);
						memory[registers[registerA] + decimal] = twoChars;
						decimal++;
					}

				}
				else {
					int displ;

					if (isNegativeDispl(displacement)) {
						displ = negativeDispl(displacement);
					}	
					else 
					{

						displ = hexToDecimal(displacement);

					}
					string hex = decToHex(registers[registerC]);
					string padNum = padNumber(hex, 8);
					string littleEndian = convertToLittleEndian(padNum);
					int decimal = 0;
					for (std::size_t i = 0; i < 8; i += 2) {
						std::string twoChars = littleEndian.substr(i, 2);
						memory[registers[registerA] + decimal + displ] = twoChars;
						decimal++;
					}

				}
				registers[15] += 4;
			}
			else if (mod == '1') { //push
				int registerA = checkReg(regA); //sp
				int registerC = checkReg(regC);
				if (displacement == "FFC")
					registers[registerA] -= 4;
				memory[registers[registerA]] = decToHex(registers[registerC]);
				registers[15] += 4;
				//cout << registers[registerA] << endl;
				//cout << memory[registers[registerA]] << endl;

			}
			else if (mod == '2') { //pool
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int registerC = checkReg(regC);
			
				int displ = hexToDecimal(displacement);
				ull displToPool = registers[registerA] + displ;

				int pc = displToPool;
				string value1 = memory[pc];
				string value2 = memory[pc + 1];
				string value3 = memory[pc + 2];
				string value4 = memory[pc + 3];

				string newAddr = value4.append(value3).append(value2).append(value1);

				unsigned int dec = hexToDecimal(newAddr);

				string val = decToHex(registers[registerC]);
				string padNum = padNumber(val, 8);
				string littleEndian = convertToLittleEndian(padNum);
				int decimal = 0;
				for (std::size_t i = 0; i < 8; i += 2) {
					std::string twoChars = littleEndian.substr(i, 2);
					memory[dec + decimal] = twoChars;
					decimal++;
				}
				registers[15] += 4;

				//for (int i = 0; i < 4; i++) {
				//	cout << dec + i << endl;
				//	cout << memory[dec + i] << endl;
				//}


			}
			break;
		}
		case '9': //load
		{
			if (mod == '0') { //csrrd
				int registerA = checkReg(regA);
				int specReg = checkReg(regB);
				registers[registerA] = csregisters[specReg];
		
				registers[15] += 4;
			}
			else if (mod == '1') { //reg <= reg + pomeraj
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				if (displacement == "008" && registerA == 14 && registerB == 14) {
					registers[15] = hexToDecimal(memory[registers[registerB]]);
					registers[registerB] += 4;

					csregisters[0] = hexToDecimal(memory[registers[registerB]]);;
					registers[registerB] += 4;


				}
				else if (displacement != "000") { //ld  $lit , reg
					registers[registerA] = hexToDecimal(displacement);

					registers[15] += 4;
				}

				else { //ld reg, reg
					registers[registerA] = registers[regB];
					registers[15] += 4;
				}
				
			}
			else if (mod == '2' && checkReg(regB) == 15 && displacement != "000") { //iz memorije u registar, $lit i $symbol
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int decimal = hexToDecimal(displacement);
				ull displToPool = registers[registerB] + decimal;
			
				int pc = displToPool;
				string value1 = memory[pc];
				string value2 = memory[pc + 1];
				string value3 = memory[pc + 2];
				string value4 = memory[pc + 3];
				string newAddr = value4.append(value3).append(value2).append(value1);
			
				unsigned int dec = hexToDecimal(newAddr);
				registers[registerA] = dec;

			
				registers[registerB] += 4;
			}
			else if (mod == '2' && displacement != "000" && checkReg(regB) != 'F') {
				int registerB = checkReg(regB);
				int registerA = checkReg(regA);
				int displ;

				if (isNegativeDispl(displacement)) {
					displ = negativeDispl(displacement);
				}
				else {
					displ = hexToDecimal(displacement);
				}
				string vrednost = memory[registers[registerB] + displ];
				registers[registerA] = hexToDecimal(vrednost);
				registers[15] += 4;
		
			}
			else if (mod == '2' && displacement == "000") {
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int addr = registers[registerA];
				string val = memory[addr];
				string val2 = memory[addr + 1];
				string val3 = memory[addr + 2];
				string val4 = memory[addr + 3];
				string konacna = val.append(val2).append(val3).append(val4);
				string littleEndian = convertToLittleEndian(konacna);

				int rez = hexToDecimal(littleEndian);
				registers[registerB] = rez;
	
				registers[15] += 4;

			}
			else if (mod == '3') { //pop reg
		
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);
				int displ = hexToDecimal(displacement);
				
				registers[registerA] = hexToDecimal(memory[registers[registerB]]);
				registers[registerB] += displ;
				
				if (registerA != 15) registers[15] += 4;
				
			}
			if (mod == '4') { //csrwr
				int registerA = checkReg(regA);
				int registerB = checkReg(regB);

				csregisters[registerA] = registers[registerB];
				registers[15] += 4;

			}
			else if (mod == '5') { //csr <= csr | D

			}
			else if (mod == '6') { //iz memorije u registar csr
				int registerA = checkReg(regA); //status 
				int registerB = checkReg(regB); //pc
				registers[registerB] = hexToDecimal(memory[registers[14]]);
				registers[14] += 4;

				csregisters[registerA] = hexToDecimal(memory[registers[14]]);;
				registers[14] += 4;
			}
			else if (mod == '7') { //pop csr

			}

			break;
		}
	}

	

}

void Emulator::printRegisters() {
	for (int i = 0; i < 16; i++) {
		string regVal = decToHex(registers[i]);
		regVal = padNumber(regVal, 8);
		printRegister[i] = regVal;
	}

	cout << " r0=" << "0x" << printRegister[0] <<"   " << " r1=" << "0x" << printRegister[1] << "    " << " r2=" << "0x" << printRegister[2] << "     " << " r3=" << "0x" << printRegister[3] << endl;
	cout << " r4=" << "0x" << printRegister[4] << "   " << " r5=" << "0x" << printRegister[5] << "    " << " r6=" << "0x" << printRegister[6] << "     " << " r7=" << "0x" << printRegister[7] << endl;
	cout << " r8=" << "0x" << printRegister[8] << "   " << " r9=" << "0x" << printRegister[9] << "    " << "r10=" << "0x" << printRegister[10] << "     " << "r11=" << "0x" << printRegister[11] << endl;
	cout << "r12=" << "0x" << printRegister[12] << "   " << "r13=" << "0x" << printRegister[13] << "    " << "r14=" << "0x" << printRegister[14] << "     " << "r15=" << "0x" << printRegister[15] << endl;

}


void Emulator::test(ull val1, ull val2, string val3) {
	long trueRegisterB;
	long trueRegisterC;


	if (isNegative(val1)) {
		string hexValB = decToHex(val1);
		string paddedHexValB = padNumber(hexValB, 8);
		cout << paddedHexValB << endl;
		trueRegisterB = negative(paddedHexValB);
		if (isNegative(val2)) {
			cout << "drugi broj negativan" << endl;
			string hexValC = decToHex(val2);
			string paddedHexValC = padNumber(hexValC, 8);
			trueRegisterC = negative(paddedHexValC);
		}
		else {
			trueRegisterC = val2;
		}
	}
	else if (isNegative(val2)) {
		
		string hexValC = decToHex(val2);
		string paddedHexValC = padNumber(hexValC, 8);
		trueRegisterC = negative(paddedHexValC);
		trueRegisterB = val1;
	}
	else {
		trueRegisterB = val1;
		trueRegisterC = val2;
	}
	if (trueRegisterB > trueRegisterC) {
		cout << "B vece od C" << endl;
	}
	else {
		cout << "C vece od B" << endl;
	}


	cout << "Val1: " << trueRegisterB << endl;
	cout << "Val2: " << trueRegisterC << endl;


	int displ;

	if (isNegativeDispl(val3)) {
		displ = negativeDispl(val3);

	}
	else
	{
		displ = hexToDecimal(val3);
	}

	cout << displ << endl;
}


void Emulator::printMem(){
	    for (const auto& pair : memory) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
}
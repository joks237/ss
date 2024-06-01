all: assembler linker emulator

clean:
	rm -rf src/assembler.o src/linker.o src/emulator.o tests/*.o tests/*.hex assembler linker emulator

assembler: src/assembler.cpp
	g++ -std=c++17 -o assembler src/assembler.cpp src/exception.cpp src/literalPool.cpp src/main.cpp src/relocationRecord.cpp src/section.cpp src/symbol.cpp inc/assembler.h inc/exception.h inc/list.h inc/literalPool.h inc/relocationRecord.h inc/section.h inc/symbol.h 
linker: src/linker.cpp	
	g++ -std=c++17 -o linker src/linker.cpp src/parser.cpp src/literalPoolLinker.cpp src/mainLinker.cpp src/relocationRecordLinker.cpp src/sectionLinker.cpp src/symbolLinker.cpp inc/linker.h inc/listLinker.h inc/literalPoolLinker.h inc/parser.h inc/relocationRecordLinker.h inc/sectionLinker.h inc/symbolLinker.h
emulator: src/emulator.cpp
	g++ -std=c++17 -o emulator src/emulator.cpp src/mainEmulator.cpp inc/emulator.h

tests: assembler
	./assembler -o tests/main.o tests/main.s
	./assembler -o tests/math.o tests/math.s
	./assembler -o tests/handler.o tests/handler.s
	./assembler -o tests/isr_software.o tests/isr_software.s
	./assembler -o tests/isr_terminal.o tests/isr_terminal.s
	./assembler -o tests/isr_timer.o tests/isr_timer.s
	
	./linker -hex -place=my_code@0x40000000 -place=math@0xF0000000 -o tests/program.hex tests/main.o tests/math.o tests/handler.o tests/isr_timer.o tests/isr_software.o tests/isr_terminal.o

	./emulator tests/program.hex
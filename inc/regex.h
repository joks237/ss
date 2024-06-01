#ifndef REGEX_H
#define REGEX_H

#include <stdio.h>
#include <regex>
using namespace std;

//literali i simboli

regex literal("(0x[0-9a-fA-F]+|[0-9]+)");
regex symbol("(([a-zA-Z][a-zA-Z0-9_]*))");

// assembly directives
regex global(".global ([a-zA-Z][a-zA-Z0-9_]*)(,? ([a-zA-Z][a-zA-Z0-9_]*))*");
regex externn(".extern ([a-zA-Z][a-zA-Z0-9_]*)(,? ([a-zA-Z][a-zA-Z0-9_]*))*");
regex section(".section ([a-zA-Z][a-zA-Z0-9_]*)");
regex word(".word ([a-zA-Z0-9_]*|((0x[0-9a-fA-F]+|[0-9]+)))(,? ([a-zA-Z0-9_]*|((0x[0-9a-fA-F]+|[0-9]+))))*");
regex skip(".skip (0x[0-9a-fA-F]+|[0-9]+)");
regex endd(".end");
//instructions without parameters
regex withoutParam("halt|int|iret|ret");
//call instruction
regex callLiteral("(call) (0x[0-9a-fA-F]+|[0-9]+)");
regex callSymbol("(call) ([a-zA-Z][a-zA-Z0-9_]*)");
//arithmetic and logical instructions
regex arrAndLogInstr("(xchg|add|sub|mul|div|and|or|xor|shl|shr) %(r([0-9]|1[0-5])|sp|pc), %(r([0-9]|1[0-5])|sp|pc|sp|pc)");
//push, pop and not instruction (instructions with one parameter)
regex notPushPopInstr("(not|push|pop) %(r([0-9]|1[0-5])|sp|pc)");
//jump instruction
regex jumpLiteralValue("(jmp) (0x[0-9a-fA-F]+|[0-9]+)");
regex jumpSymbolValue("(jmp) ([a-zA-Z][a-zA-Z0-9_]*)");
//branch instructions
regex branchLiteral("(beq|bne|bgt) %(r([0-9]|1[0-5])|sp|pc), %(r([0-9]|1[0-5])|sp|pc), (0x[0-9a-fA-F]+|[0-9]+)");
regex branchSymbol("(beq|bne|bgt) %(r([0-9]|1[0-5])|sp|pc), %(r([0-9]|1[0-5])|sp|pc), ([a-zA-Z][a-zA-Z0-9_]*)");
//load instructions
regex loadLiteralValue("(ld) \\$(0x[0-9a-fA-F]+|[0-9]+), %(r([0-9]|1[0-5])|sp|pc)");
regex loadSymbolValue("(ld) \\$(([a-zA-Z0-9_]){1,}), %(r([0-9]|1[0-5])|sp|pc)");
regex loadLiteralFromMemory("(ld) (0x[0-9a-fA-F]+|[0-9]+), %(r([0-9]|1[0-5])|sp|pc)");
regex loadSymbolFromMemory("(ld) ([a-zA-Z][a-zA-Z0-9_]*), %(r([0-9]|1[0-5])|sp|pc)");
regex loadFromReg("(ld) %(r([0-9]|1[0-5])|sp|pc|status|handler|cause), %(r([0-9]|1[0-5])|sp|pc)");
regex loadFromMemoryReg("(ld) (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause)\\]), %(r([0-9]|1[0-5])|sp|pc)");
regex loadFromMemoryWithOffsetLiteral("(ld) (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (0x[0-9a-fA-F]+|[0-9]+)\\]), %(r([0-9]|1[0-5])|sp|pc)");
regex loadFromMemoryWithOffsetSymbol("(ld) (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (([a-zA-Z][a-zA-Z0-9_]*))\\]), %(r([0-9]|1[0-5])|sp|pc)");
//store instructions
regex storeLiteralValue("(st) %(r([0-9]|1[0-5])|sp|pc), \\$(0x[0-9a-fA-F]+|[0-9]+)");
regex storeSymbolValue("(st) %(r([0-9]|1[0-5])|sp|pc), \\$([a-zA-Z][a-zA-Z0-9_]*)");
regex storeLiteralFromMemory("(st) %(r([0-9]|1[0-5])|sp|pc), (0x[0-9a-fA-F]+|[0-9]+)");
regex storeSymbolFromMemory("(st) %(r([0-9]|1[0-5])|sp|pc), ([a-zA-Z][a-zA-Z0-9_]*)");
regex storeFromReg("(st) %(r([0-9]|1[0-5])|sp|pc), %(r([0-9]|1[0-5])|sp|pc|status|handler|cause)");
regex storeFromMemoryReg("(st) %(r([0-9]|1[0-5])|sp|pc), (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause)\\])");
regex storeFromMemoryWithOffsetLiteral("(st) %(r([0-9]|1[0-5])|sp|pc), (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (0x[0-9a-fA-F]+|[0-9]+)\\])");
regex storeFromMemoryWithOffsetSymbol("(st) %(r([0-9]|1[0-5])|sp|pc), (\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (([a-zA-Z][a-zA-Z0-9_]*))\\])");
//read and write control status register
regex readControlStatusReg("(csrrd) %(status|handler|cause), %(r([0-9]|1[0-5])|sp|pc)");
regex writeControlStatusReg("(csrwr) %(r([0-9]|1[0-5])|sp|pc), %(status|handler|cause)");
//pomocni regexi za load i store
regex litVal("\\$(0x[0-9a-fA-F]+|[0-9]+|[0-1]+b)");
regex symVal("\\$(([a-zA-Z][a-zA-Z0-9_]*))");
regex reg("%(r([0-9]|1[0-5])|sp|pc|status|handler|cause)");
regex fromMemoryReg("\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause)\\]");
regex fromMemoryWithOffsetLiteral("\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (0x[0-9a-fA-F]+|[0-9]+)\\]");
regex fromMemoryWithOffsetSymbol("\\[%(r([0-9]|1[0-5])|sp|pc|status|handler|cause) \\+ (([a-zA-Z][a-zA-Z0-9_]*))\\]");
regex label("^[A-Za-z][A-Za-z0-9]*:$");
regex directives(".global|.extern|.skip|.word|.end|.section|halt|int|ret|iret|call|ld|st|csrrd|csrwr|add|sub|mul|div|and|or|xor|not|xchg|shl|shr|push|pop|jmp|beq|bne|bgt");
regex hexRegex("^0x[0-9A-Fa-f]+$");
regex binaryRegex("^[01]+b$");
regex onlyDirectiveRegex(".global|.extern|.section|.word|.skip|.end");
regex onlyInstructionRegex("halt|int|ret|iret|call|ld|st|csrrd|csrwr|add|sub|mul|div|and|or|xor|not|xchg|shl|shr|push|pop|jmp|beq|bne|bgt");
regex specialReg("%status|%handler|%cause");

#endif
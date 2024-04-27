#pragma once

//to run the program, call to compileAndLoad and then call interpret_next(virus), until the virus.active becomes false; when the virus terminates, its 'passed' will be true only if it passed the test

#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
// #include "terminal.h"

struct SeagullVirus {
	constexpr static uint32_t stackSize = 64;
	uint32_t registerFile[8];
	uint32_t stack[stackSize];
	uint32_t stackPointer = 0;
	std::vector<uint32_t> instructionStream;
	uint32_t instructionPointer;
	bool active;
	bool passed;
	std::vector<std::string>* logFile;
};

std::vector<uint32_t> compileProgram(const char* code);

SeagullVirus virus {};

uint32_t inputN;
int atN;
int primes[] { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997 };

void compileAndLoad(std::vector<std::string>* code, std::vector<std::string>* logFile) {
	virus = {};
	srand(time(NULL));
	inputN = rand() % 168;
	atN = 0;
	std::string preprocessed = "";
	for (int i = 0; i < code->size(); i++) {
		preprocessed.append((*code)[i]);
		preprocessed.push_back('\n');
	}
	preprocessed.pop_back();
	virus.logFile = logFile;
	virus.instructionStream = compileProgram(preprocessed.c_str());
	virus.active = true;
}

//_______________________________________________________________________________________

enum Instruction {
	ADD,SUB,REM,JMP,JZE,CEQ,CNE,CGT,CGE,CLT,CLE,MOV,AND,ORR,XOR,INN,POUT,PSH,POP,LOG
};

void interpret_next(SeagullVirus& virus) {
	if (!virus.active) {
		return;
	}
	if (virus.instructionPointer >= virus.instructionStream.size()) {
		virus.active = false;
		return;
	}
	uint32_t data = virus.instructionStream[virus.instructionPointer++];
	Instruction op = static_cast<Instruction>(data & 0xFF);
	data >>= 8;
	if (op == MOV && virus.instructionPointer >= virus.instructionStream.size()) {
		virus.active = false;
		return;
	}
	switch (op) {
	case ADD: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] + virus.registerFile[(data >> 16) & 7]; break;
	case SUB: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] - virus.registerFile[(data >> 16) & 7]; break;
	case REM: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] % virus.registerFile[(data >> 16) & 7]; break;
	case JMP: virus.instructionPointer = data; break;
	case JZE: virus.instructionPointer = virus.registerFile[data & 7] ? virus.instructionPointer : data >> 8; break;
	case CEQ: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] == virus.registerFile[(data >> 16) & 7]; break;
	case CNE: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] != virus.registerFile[(data >> 16) & 7]; break;
	case CGT: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] > virus.registerFile[(data >> 16) & 7]; break;
	case CGE: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] >= virus.registerFile[(data >> 16) & 7]; break;
	case CLT: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] < virus.registerFile[(data >> 16) & 7]; break;
	case CLE: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] <= virus.registerFile[(data >> 16) & 7]; break;
	case MOV: virus.registerFile[data & 7] = virus.instructionStream[virus.instructionPointer++]; break;
	case AND: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] & virus.registerFile[(data >> 16) & 7]; break;
	case ORR: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] | virus.registerFile[(data >> 16) & 7]; break;
	case XOR: virus.registerFile[data & 7] = virus.registerFile[(data >> 8) & 7] ^ virus.registerFile[(data >> 16) & 7]; break;
	case PSH:
		if (virus.stackPointer >= SeagullVirus::stackSize) {
			virus.active = false;
			return;
		}
		virus.stack[virus.stackPointer++] = virus.registerFile[data & 7];
		break;
	case POP:
		if (virus.stackPointer == 0) {
			virus.active = false;
			return;
		}
		virus.registerFile[data & 7] = virus.stack[--virus.stackPointer];
		break;
	case INN:
		virus.registerFile[data & 7] = inputN;
		break;
	case POUT:
		if (virus.registerFile[data & 7] == primes[atN]) {
			atN++;
			if (atN == inputN) {
				virus.passed = true;
				virus.active = false;
			}
		} else {
			virus.active = false;
		}
		break;
	case LOG:
		if (virus.logFile != nullptr) {
			virus.logFile->push_back(std::to_string(virus.registerFile[data & 7]));
		}
		break;
	default: virus.active = false; break;
	}
}

//skip to next non space character (or :)
void skipToNext(const char*& cur) {
	while (*cur == '\n' || *cur == ' ' || *cur == ':' || *cur == ',') {
		cur++;
	}
}

//convert range to a string
std::string rangeToString(const char* start, const char* end) {
	std::string str = "";
	while (start != end) {
		str += *start;
		start++;
	}
	return str;
}

//compare if range in char array is equal to a string
bool compare(const char* start, const char* end, std::string str) {
	if (end - start != str.size()) return false;
	for (int i = 0; i < str.size(); i++)
		if (str[i] != toupper(*(start + i))) return false;
	return true;
}

//return a number of the next register given a range (-1 if invalid)
int getRegister(const char* start, const char* end) {
	if (end - start != 2) return -1;
	if (*start != 'R' && *start != 'r') return -1;
	int rid = *(start + 1) - '0';
	if (rid > 7 || rid < 0) return -1;
	return rid;
}

//get next token and put into 'buffer'
const char* nextToken(const char* cur) {
	while (!(*cur == ' ' || *cur == ':' || *cur == '\0' || *cur == '\n')) {
		cur++;
	}
	return cur;
}

//get next number (or register) and put into 'buffer'
const char* nextArg(const char* cur) {
	while (!(*cur == '\n' || *cur == ',' || *cur == '\0')) {
		cur++;
	}
	return cur;
}

//get number of instruction represented by a range (or -1 if not found)
int findInstruction(const char* start, const char* end) {
	if (compare(start, end, "ADD")) return ADD;
	if (compare(start, end, "SUB")) return SUB;
	if (compare(start, end, "REM")) return REM;
	if (compare(start, end, "JMP")) return JMP;
	if (compare(start, end, "JZE")) return JZE;
	if (compare(start, end, "CEQ")) return CEQ;
	if (compare(start, end, "CNE")) return CNE;
	if (compare(start, end, "CGT")) return CGT;
	if (compare(start, end, "CGE")) return CGE;
	if (compare(start, end, "CLT")) return CLT;
	if (compare(start, end, "CLE")) return CLE;
	if (compare(start, end, "AND")) return AND;
	if (compare(start, end, "ORR")) return ORR;
	if (compare(start, end, "XOR")) return XOR;
	if (compare(start, end, "MOV")) return MOV;
	if (compare(start, end, "INN")) return INN;
	if (compare(start, end, "OUT")) return POUT;
	if (compare(start, end, "PSH")) return PSH;
	if (compare(start, end, "POP")) return POP;
	if (compare(start, end, "LOG")) return LOG;
	return -1;
}

void createAndPush(int index, std::string labelName, std::unordered_map<std::string, std::vector<uint32_t>>& uninit_labels) {
	if (uninit_labels.find(labelName) == uninit_labels.end()) { //this label doesn't exist in uninit_labels yet
		uninit_labels[labelName] = std::vector<uint32_t>();
	}
	uninit_labels[labelName].push_back(index);
}

//given a just read instruction, read all of its inputs; return flase if there's an error occured
bool readInstruction(int instruction, const char*& cur, 
	std::vector<uint32_t>& compiled, 
	std::unordered_map<std::string, uint32_t>& labels,
	std::unordered_map<std::string, std::vector<uint32_t>>& uninit_labels) {
	uint32_t fullInstruction = instruction; //a "full" instruction with instruction, registers, etc
	if (instruction == INN || instruction == POUT || instruction == PSH || instruction == POP || instruction == LOG) {
		//read 1 reg
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 8);
	} else if (instruction == JMP) {
		//label
		skipToNext(cur);
		const char* next = nextArg(cur);
		std::string label = rangeToString(cur, next);
		cur = next;
		if (labels.find(label) == labels.end()) { //it doesn't exist yet
			createAndPush(compiled.size(), label, uninit_labels);
			//temporarily, this is still 0
		} else {
			fullInstruction |= (labels[label] << 8);
		}
	} else if (instruction == JZE) {
		//reg
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 8);
		//label
		skipToNext(cur);
		next = nextArg(cur);
		std::string label = rangeToString(cur, next);
		cur = next;
		if (labels.find(label) == labels.end()) { //it doesn't exist yet
			createAndPush(compiled.size(), label, uninit_labels);
			//temporarily, this is still 0
		}
		else {
			fullInstruction |= (labels[label] << 16);
		}
	} else if (instruction == MOV) {
		//1 reg
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 8);
		//push back a number
		skipToNext(cur);
		next = nextArg(cur);
		std::string numStr = rangeToString(cur, next);
		cur = next;
		skipToNext(cur);
		//the only instruction in 2 32 memory cells (instruction, immidiate)
		uint32_t im = std::stoi(numStr);
		compiled.push_back(fullInstruction);
		compiled.push_back(im);
		return true;
	} else {
		//push a register, but 3 times
		skipToNext(cur);
		const char* next = nextArg(cur);
		int reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 8);
		skipToNext(cur);
		next = nextArg(cur);
		reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 16);
		skipToNext(cur);
		next = nextArg(cur);
		reg = getRegister(cur, next);
		if (reg == -1) return false;
		cur = next;
		fullInstruction |= (reg << 24);
	}
	skipToNext(cur);
	compiled.push_back(fullInstruction);
	return true;
}

//first 8 bytes (int & 0xFF) is a command (unless immediate for MOV), next bytes (int & 0xFF00; 0xFF0000; 0xFF000000) are params

//compile to a vector
std::vector<uint32_t> compileProgram(const char* code) {
	std::vector<uint32_t> compiled;
	const char* cur = code;
	std::unordered_map<std::string, uint32_t> labels;
	std::unordered_map<std::string, std::vector<uint32_t>> uninit_labels;
	while (*cur != '\0') {
		skipToNext(cur);
		const char* next = nextToken(cur);
		int nextInstruction = findInstruction(cur, next);
		if (nextInstruction == -1) { //it's a label
			std::string label = rangeToString(cur, next);
			if (labels.find(label) == labels.end()) {
				labels[label] = compiled.size();
				cur = next;
			} else {
				return std::vector<uint32_t>(); //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
			}
		} else {
			cur = next;
			bool noError = readInstruction(nextInstruction, cur, compiled, labels, uninit_labels);
			if (!noError) {
				return std::vector<uint32_t>(); //AAAAA
			}
		}
	}
	for (std::pair<std::string, std::vector<uint32_t>> vec : uninit_labels) {
		if (labels.find(vec.first) == labels.end()) {
			return std::vector<uint32_t>(); //AAAAAAAAAAAAAAAAAAAAA
		}
		int jumpTo = labels[vec.first];
		for (uint32_t i : vec.second) {
			if((compiled[i] & 0xFF) == 3) //it it's a regular jump, not conditional
				compiled[i] |= (jumpTo << 8);
			else
				compiled[i] |= (jumpTo << 16);
		}
	}
	return compiled;
}

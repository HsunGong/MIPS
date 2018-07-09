#pragma once
#include <iostream>
#include <cstdio>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "exception.hpp"

#define M 10000000
#define ins_size 16
#define EMPTY -1
using namespace std;

map<string, int> label;


vector<string> forOperator = {
	"add", "addu", "addiu", "sub", "subu", "xor", "xoru", "rem", "remu", "seq", "sge", "sgt", "sle", "slt", "sne",
	"neg", "negu", "li", "move",
	"mfhi", "mflo",
	"jr", "jalr",
	"nop", "syscall",
	"mul", "mulu", "div", "divu",
	"b", "j", "jal",
	"beq", "bne", "bge", "ble", "bgt", "blt",
	"beqz", "bnez", "blez", "bgez", "bgtz", "bltz",
	"la", "lb", "lh", "lw",
	"sb", "sh", "sw"
};

enum MIPS_OPERATOR {
	//First_scanf:
	ADD, ADDU, ADDIU, SUB, SUBU, XOR, XORU, REM, REMU, SEQ, SGE, SGT, SLE, SLT, SNE, // Rd R1 R2/Imm
	NEG, NEGU, LI, MOVE, //Rd R1/Imm
	MFHI, MFLO, //Rd
	JR, JALR, //R1
	NOP, SYSCALL,
	MUL, MULU, DIV, DIVU, //R1 R2/Imm
	//Second_scanf:
	B, J, JAL, //Label 
	BEQ, BNE, BGE, BLE, BGT, BLT, //R1 R2/Imm Label 
	BEQZ, BNEZ, BLEZ, BGEZ, BGTZ, BLTZ, //R1 Label 
	LA, LB, LH, LW, //Rd Address (Del) R1 -> Rd
	SB, SH, SW //R1 Address (Del) R1 -> Rd
};
map<string, int> OperatorIndex; 

vector<string> forRegister = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2" ,"s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1",
	"gp", "sp", "fp", "ra", "lo", "hi", "EMPTY"
};
enum MIPS_REGISTER {
	ZERO, AT, V0, V1, A0, A1, A2, A3,
	T0, T1, T2, T3, T4, T5, T6, T7,
	S0, S1, S2, S3, S4, S5, S6, S7,
	T8, T9, K0, K1,
	GP, SP, FP, RA, LO, HI, PC
};
const int S8 = 30;
map<string, int> RegisterIndex;


//stoi (find a number)
int mstoi(string &s, int &loc) {
	//type: int16_t;int32_t...
	int result = 0, f = 1;
	while (loc < s.length() && s[loc] != '-' && (s[loc] > '9' || s[loc] < '0') ) ++loc;
		if (s[loc] == '-') f = -1, ++loc;
		while (loc < s.length() && s[loc] >= '0' && s[loc] <= '9') {
			result = result * 10 + s[loc] - '0';
			++loc;
		}
		return result * f;
}

bool skip(const char &c) {
	if (c == ' ' || c == '\t' || c == '\n') return 1;
	return 0;
}




const string REG_STR[] = { "$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23", "$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31", "$lo", "$hi", "$pc" };
const string REG_NUM[] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra", "$lo", "$hi", "$pc" };

class ram {
public:
	char memory[M];
	int reg[34] = { 0 };
	int mem_low = 0;

	ram() {
		_ram.mem_low = 0;
		_ram.reg[SP] = M - 1;
	}

	static int storeg(string &s, int &cur) {
		while (s[cur] != '$') ++cur;
		++cur;
		if (s[cur] >= '0' && s[cur] <= '9') {
			int ret = 0;
			for (; s[cur] >= '0' && s[cur] <= '9'; ret = ret * 10 + s[cur] - '0', ++cur);
			// cout << "Debug " << ret << '\n';
			return ret;
		}
		else {
			int next = cur;
			while(!skip(s[next]) && s[next] != ',' && s[next] != ')') ++next;
			string RegisterName(&s[cur], &s[next]);
			cur = next;
			return RegisterIndex[RegisterName];
		}
	}


	//placement new
	void saveInt(int x, int & cur, int ord);
	void saveString(string & str, int & cur);
	void saveStruct(string & line, int & cur, int len, int l, int r, int index, bool isLabel);

}_ram;





class instruction {
public:
	int8_t status;//the order

	//存放寄存器，操作数1，操作数2
	int8_t regist[3];//
	//int8_t rdest, rsrc1, rsrc2;;
	int imm, label, offset;

	instruction(int8_t index = 255) : status(index) {
		for (int i = 0; i < 3; ++i) regist[i] = EMPTY;
		offset = 0;
	}
	void load(int loc);
};











//#define DEBUG










#ifdef DEBUG
void Debug(instruction & _order, int & cnt) {
	cout << "Debug: \n{\n" << ++cnt << "\nOperator: " << forOperator[_order.status] << '\n';
	for (int i = 0; i < 3; ++i)
		cout << "Register[" << i << "]: " << int(_order.regist[i]) << " or " << forRegister[_order.regist[i]] << '\n';
	cout << "Delta: " << _order.offset << '\n'
		<< "Imm :" << _order.imm << '\n'
		<< "Label :" << _order.label << "\n";
	for (int i = 0; i < 34; ++i) cout << i << '\t'; cout << '\n';
	for (int i = 0; i < 34; ++i) cout << _ram.reg[i] << '\t'; cout << "\n}\n\n";
}
#endif

#ifndef DEBUG
void Debug(instruction & _order, int & cnt) {}
#endif






/*
string:

string(char *a, char *b) :  new string is (from a to b-1)
s.substr(start pos, size)
s.stoi : can't oversize, cant transfer "ssss123" type

*/
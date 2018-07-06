#pragma once



#include <utility>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>


#include "exception.hpp"


using namespace std;

//std::to_string(xx);
//std::stoi(), stol(), stoll();



#pragma region wrong code
//
////´æ´¢´úÂë
//vector<string> Code;
//
////¶ÁÐ´
//ifstream fin;
////ifstream fin("code.s");
////ofstream cout("code.out");
////ifstream cin("data.in");
//
////²Ù×÷ÊýÓ³Éä
//vector<string> forOperator = {
//	"add", "addu", "addiu", "sub", "subu", "xor", "xoru", "rem", "remu", "seq", "sge", "sgt", "sle", "slt", "sne",
//	"neg", "negu", "li", "move",
//	"mfhi", "mflo",
//	"jr", "jalr",
//	"nop", "syscall",
//	"mul", "mulu", "div", "divu",
//	"b", "j", "jal",
//	"beq", "bne", "bge", "ble", "bgt", "blt",
//	"beqz", "bnez", "blez", "bgez", "bgtz", "bltz",
//	"la", "lb", "lh", "lw",
//	"sb", "sh", "sw"
//};
//
//enum MIPS_OPERATOR {
//	//First_scanf:
//	ADD, ADDU, ADDIU, SUB, SUBU, XOR, XORU, REM, REMU, SEQ, SGE, SGT, SLE, SLT, SNE, // Rd R1 R2/Imm
//	NEG, NEGU, LI, MOVE, //Rd R1/Imm
//	MFHI, MFLO, //Rd
//	JR, JALR, //R1
//	NOP, SYSCALL,
//	MUL, MULU, DIV, DIVU, //R1 R2/Imm
//						  //Second_scanf:
//						  B, J, JAL, //Label 
//						  BEQ, BNE, BGE, BLE, BGT, BLT, //R1 R2/Imm Label 
//						  BEQZ, BNEZ, BLEZ, BGEZ, BGTZ, BLTZ, //R1 Label 
//						  LA, LB, LH, LW, //Rd Address (Del) R1 -> Rd
//						  SB, SH, SW //R1 Address (Del) R1 -> Rd
//};
//map<string, int> OperatorIndex; //²Ù×÷µÄÓ³Éä
//
//								//¼Ä´æÆ÷Ó³Éä
//vector<string> forRegister = {
//	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
//	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
//	"s0", "s1", "s2" ,"s3", "s4", "s5", "s6", "s7",
//	"t8", "t9", "k0", "k1",
//	"gp", "sp", "fp", "ra", "lo", "hi", "EMPTY"
//};
//enum MIPS_REGISTER {
//	ZERO, AT, V0, V1, A0, A1, A2, A3,
//	T0, T1, T2, T3, T4, T5, T6, T7,
//	S0, S1, S2, S3, S4, S5, S6, S7,
//	T8, T9, K0, K1,
//	GP, SP, FP, RA, LO, HI, EMPTY
//};
//const int S8 = 30;
//map<string, int> RegisterIndex; //¼Ä´æÆ÷µÄÓ³Éä
#pragma endregion



map<string, int> text_label, data_label;


int pow_2(int n) {
	int res = 1;
	while (n--) res <<= 1;
	return res;
}

// ".axx s"
//  012345
// pos = 1, end = 4
string get_phrase(const string &str, int &tail) {
	int head = tail;
	tail = str.find(' ', tail);
	if (tail == string::npos) throw not_exist();
	return str.substr(head, tail - head);
}


string to_str(string &s, int &i) {
	string res;
	++i;//s[i]== '\"'

	while (i < s.length()) {//s[s.length()] == '\"'
		res += s[i++];
		if (res.back() == '\\') {
			char ch = s[i++];
			char transfer;
			switch (ch) {
			case 'n': transfer = '\n'; break;
			case 'r': transfer = '\r'; break;
			case 't': transfer = '\t'; break;
			case '\\': transfer = '\\'; break;
			case '\'': transfer = '\''; break;
			case '\"': transfer = '\"'; break;
			case '0': transfer = '\0'; break;
			}
			res.back() = transfer;// change back from '\\' into '\n' or ....
		}
	}

	return res;
}


#define maxc 1000








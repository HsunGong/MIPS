#include "head.hpp"

map<string, int> label;

vector<string> forIns = {
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


map<string, int> Ins_map;

vector<string> forReg = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2" ,"s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1",
	"gp", "sp", "fp", "ra", "lo", "hi", "EMPTY"
};



const int S8 = 30;
map<string, int> Reg_map;




//stoi (find a number)
int mstoi(string &s, int &cur) {
	//type: int16_t;int32_t...
	int result = 0, f = 1;
	while (cur < s.length() && s[cur] != '-' && (s[cur] > '9' || s[cur] < '0')) ++cur;
	if (s[cur] == '-') f = -1, ++cur;
	while (cur < s.length() && s[cur] >= '0' && s[cur] <= '9') {
		result = result * 10 + s[cur] - '0';
		++cur;
	}
	return result * f;
}

bool skip(const char &c) {
	if (c == ' ' || c == '\t' || c == '\n') return 1;
	return 0;
}







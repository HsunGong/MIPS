#include "head.hpp"
#include "instruction.hpp"

ram _ram;

vector<string> Code;

extern map<string, int> label;
extern vector<string> forIns;
extern map<string, int> Ins_map;
extern vector<string> forReg;
extern map<string, int> Reg_map;
extern ram _ram;


const string REG_STR[] = { "$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23", "$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31", "$lo", "$hi", "$pc" };
const string REG_NUM[] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra", "$lo", "$hi", "$pc" };



ram::ram() {
		reg[SP] = M - 1;
}

int ram::storeg(string &s, int &cur) {
		while (s[cur] != '$') ++cur;
		++cur;
		if (s[cur] >= '0' && s[cur] <= '9') {
			int ret = 0;
			while (s[cur] >= '0' && s[cur] <= '9') {
				ret = ret * 10 + s[cur] - '0';
				++cur;
			}
			return ret;
		}
		else {
			int next = cur;
			while(!skip(s[next]) && s[next] != ',' && s[next] != ')') ++next;
			string RegisterName(&s[cur], &s[next]);
			cur = next;
			return Reg_map[RegisterName];
		}
}


//placement new
void ram::saveInt(int x, int & cur, int size) {
	if (size == 1) new(memory + cur) int8_t(x);
	else if (size == 2) new(memory + cur) int16_t(x);
	else new(memory + cur) int32_t(x);
	cur += size;
}

void ram::saveString(string & str, int & cur) {
	str.push_back('\0');
	for (int len = str.length(), i = 0; i < len - 1; ++i) {
		char chr = str[i];
		if (str[i] == '\\') {
			++i;
			switch (str[i]) {
			case('a'): chr = 7; break;
			case('b'): chr = 8; break;
			case('f'): chr = 12; break;
			case('n'): chr = 10; break;
			case('r'): chr = 13; break;
			case('t'): chr = 9; break;
			case('v'): chr = 11; break;
			case('\\'): chr = 92; break;
			case('\''): chr = 39; break;
			case('\"'): chr = 34; break;
			case('\?'): chr = 63; break;
			case('0'): chr = 0; break;
			default: --i; break;//does it happen?
			}
		}
		memory[cur++] = chr;
	}
	//_ram.memory[cur++] = '\0';
}

void ram::saveStruct(string & line, int & cur, int len, int l, int r, int index, bool isLabel) {
	instruction tmp(index);
	int i = l;//if possible, start from 0--1--2
	while (i <= r) {
		//for (; cur < len && !((line[cur] >= '0' && line[cur] <= '9') || line[cur] == '-' || line[cur] == '$'); ++cur);
		while (cur < len && (skip(line[cur]) || line[cur] == ',')) ++cur;
		if (cur == len) break;

		if (line[cur] == '$') tmp.regist[i] = storeg(line, cur);
		else tmp.imm = mstoi(line, cur);

		++i;
	}
	tmp.offset = i - l;
	if (isLabel) {
		while (skip(line[cur]) || line[cur] == ',') ++cur;
		if (line[cur] == '-' || (line[cur] >= '0' && line[cur] <= '9')) {
			tmp.offset = mstoi(line, cur);
			++cur;

			if (tmp.regist[0] == EMPTY) tmp.regist[0] = storeg(line, cur);
			else tmp.regist[1] = storeg(line, cur);
		}
		else {
			int next = cur;
			while (!skip(line[next])) ++next;

			string labelName(&line[cur], &line[next]);// how about ':'????
			tmp.label = label[labelName];
			// cout << "Debug " << labelName << ' ' << tmp.Label << '\n';
		}
	}
	new(memory + heap_top) instruction(tmp);
	heap_top += ins_size;
	// cout << "Debug ML = " << MEM_LOW << '\n';
}







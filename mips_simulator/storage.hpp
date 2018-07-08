#pragma once

#include "Header.h"
#include "instruction.hpp"



	//placement new
void ram::saveInt(int x, int & loc, int ord) {
		if (ord == 1) new(memory + loc) char(x);
		else if (ord == 2) new(memory + loc) short(x);
		else new(memory + loc) int(x);
		loc += ord;
}

void ram::saveString(string & str, int & loc) {
	str.push_back('\0');
	for (int len = str.length(), i = 0; i < len - 1; ++i) {
		char chr = str[i];
		if (str[i] == '\\') {
			++i;
			switch (str[i]){
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
		memory[loc++] = chr;
	}
	//MEMORY[loc++] = '\0';
}

void ram::saveStruct(string & line, int & loc, int len, int l, int r, int index, bool isLabel) {
	operatorcode tmp(index);
	int i = l;
	while(i <= r) {
		//for (; loc < len && !((line[loc] >= '0' && line[loc] <= '9') || line[loc] == '-' || line[loc] == '$'); ++loc);
		while(loc < len && (skip(line[loc]) || line[loc] == ',')) ++loc;
		if (loc == len) break;

		if (line[loc] == '$') tmp.regist[i] = storeg(line, loc);
		else tmp.imm = mstoi(line, loc);

		++i;
	}
	tmp.delta = i - l;
	if (isLabel) {
		while (skip(line[loc]) || line[loc] == ',') ++loc;
		if (line[loc] == '-' || (line[loc] >= '0' && line[loc] <= '9')) {
			tmp.delta = mstoi(line, loc);
			++loc;

			if (tmp.regist[0] == EMPTY) tmp.regist[0] = storeg(line, loc);
			else tmp.regist[1] = storeg(line, loc);
		}
		else {
			int nxt = loc;
			while(!skip(line[nxt])) ++nxt;

			string labelName(&line[loc], &line[nxt]);// how about ':'????
			tmp.label = label[labelName];
			// cout << "Debug " << labelName << ' ' << tmp.Label << '\n';
		}
	}
	new(memory + mem_low) operatorcode(tmp);
	mem_low += NXT;
	// cout << "Debug ML = " << MEM_LOW << '\n';
}




vector<string> Code;



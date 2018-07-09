#pragma once

#include "Header.h"
#include "instruction.hpp"



	//placement new
void ram::saveInt(int x, int & cur, int ord) {
		if (ord == 1) new(memory + cur) char(x);
		else if (ord == 2) new(memory + cur) short(x);
		else new(memory + cur) int(x);
		cur += ord;
}

void ram::saveString(string & str, int & cur) {
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
		memory[cur++] = chr;
	}
}

//cur is the order's next (means space)
//l and r is the possible parameter size
void ram::saveStruct(string & line, int & cur, int len, int l, int r, int status, bool isLabel) {
	instruction tmp(status);
	int i = l;
	while(i <= r) {//scan rsrc1, rsrc2/src2(imm or regist)
		while(cur < len && (skip(line[cur]) || line[cur] == ',')) ++cur;
		if (cur == len) break;

		if (line[cur] == '$') tmp.regist[i] = storeg(line, cur);
		else tmp.imm = mstoi(line, cur);

		++i;
	}
	tmp.offset = i - l;
	if (isLabel) {//the last one may be label or address
		while (skip(line[cur]) || line[cur] == ',') ++cur;
		if (line[cur] == '-' || (line[cur] >= '0' && line[cur] <= '9')) {
			tmp.offset = mstoi(line, cur);
			++cur;

			if (tmp.regist[0] == EMPTY) tmp.regist[0] = storeg(line, cur);
			else tmp.regist[1] = storeg(line, cur);
		}
		else {
			int next = cur;
			while(!skip(line[next])) ++next;

			string labelName(&line[cur], &line[next]);// how about ':'????
			tmp.label = label[labelName];
			// cout << "Debug " << labelName << ' ' << tmp.label << '\n';
		}
	}

	new(memory + mem_low) instruction(tmp);
	mem_low += ins_size;
	// cout << "Debug ML = " << _ram.mem_low << '\n';
}




vector<string> Code;



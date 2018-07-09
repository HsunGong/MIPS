#pragma once

#include "Header.h"
#include "instruction.hpp"



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




vector<string> Code;



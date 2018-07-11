#pragma once

#include "head.hpp"
#include "instruction.hpp"



class ram {
public:
	int8_t memory[M];
	int reg[34] = { 0 };
	int heap_top = 0;

	ram();
	static int storeg(string &s, int &cur);

	//placement new
	void saveInt(int x, int & cur, int ord);
	void saveString(string & str, int & cur);
	void saveStruct(string & line, int & cur, int len, int l, int r, int index, bool isLabel);

};
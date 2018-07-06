#pragma once
#include "Header.h"




using int32_t = __int32;

const int reg_size = 34;
const int memsize = 4 * 1024 * 1024 - 400000;

//storage of memory and register

//memory

class memory {
public:	
	char mem[memsize];

	int heap_top = 0;//cur, not maxmimum
	int text_top = 0;//cur, not maximum
public:
	memory(){}
	~memory(){}
	int store_text(int size) {
		int tmp = text;
		text += size;
		return tmp;
	}
	int store_data(int type, int size) {
		int tmp;
		switch (type) {
		case 1:
			tmp = data;
			data += size;
			heap = data;
			return tmp;
		case 2:
			tmp = heap;
			heap += size;
			if (heap > stack) throw _exception();
			return tmp;
		case 3:
			stack -= size;
			if (heap > stack) throw _exception();
			return stack;
		}
	}
}_memory;
int MEM_LOW;



//register
const string REG_STR[] = { "$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23", "$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31", "$lo", "$hi" };
const string REG_NUM[] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra", "$lo", "$hi" };
class regist {
public:
	map<string, size_t> reg_name;

public:
	int32_t reg[34] = { 0 };//4 bytes
	bool vis[34] = { 0 };//deal the hazard
public:
	regist() {
		reg_name["zero"] = 0,
		reg_name["at"] = 1,
		reg_name["v0"] = 2, reg_name["v1"] = 3,
		reg_name["a0"] = 4, reg_name["a1"] = 5, reg_name["a2"] = 6, reg_name["a3"] = 7,
		reg_name["t0"] = 8, reg_name["t1"] = 9, reg_name["t2"] = 10, reg_name["t3"] = 11, reg_name["t4"] = 12, reg_name["t5"] = 13, reg_name["t6"] = 14, reg_name["t7"] = 15,
		reg_name["s0"] = 16, reg_name["s1"] = 17, reg_name["s2"] = 18, reg_name["s3"] = 19, reg_name["s4"] = 20, reg_name["s5"] = 21, reg_name["s6"] = 22, reg_name["s7"] = 23,
		reg_name["t8"] = 24, reg_name["t9"] = 25,
		//reg_name[""] = 26 , reg_name[""] = 27 ,
		reg_name["gp"] = 28,
		reg_name["sp"] = 29,
		reg_name["fp"] = 30,
		reg_name["ra"] = 31,
		reg_name["hi"] = 32,
		reg_name["lo"] = 33,
		reg_name["pc"] = 34;

		reg[29] = memsize - 1;



	}
	~regist(){}
	static int32_t storeg(string s) noexcept{
		if (s.empty()) return -1;
		if (s.back() == ',') s.pop_back();
		for (int32_t i = 0; i < 34; ++i) if (s == REG_STR[i] || s == REG_NUM[i]) return i;
		return -1;
	}

}_regist;


bool SIMULATOR = 1;
int mem_low = 0, pc, SIMULATOR_RET = 0;

map<string, int> label;//only one label accessable

vector<string> code;



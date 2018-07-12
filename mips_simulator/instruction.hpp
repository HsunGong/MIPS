#pragma once

#include "head.hpp"
#include "store.hpp"


class instruction {
public:
	int8_t Ins_type;//the order

	int8_t regist[3];//存放寄存器，操作数1，操作数2
	int imm, label, offset;

	instruction(int8_t type = 255);
	void load(int cur);
	instruction &operator=(instruction &rhs);
	void clear();
};

#ifdef PIPELINE
class IF : public instruction {
public:
	bool block = 0;
	bool jump_block = 0;
	void push(int cur);
};

class ID : public instruction {
public:
	bool block;
	bool tovis[3];
	int reg[3];

	void push(IF &rhs);
};

class EX : public instruction {
public:
	int reg[3];
	void push(ID &rhs);
};

class MA : public instruction {
public:
	void push(EX &rhs);
};

class WB : public instruction {
public:
	void push(MA &rhs);
};

#endif
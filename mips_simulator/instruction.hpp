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
	instruction &operator=(instruction &rhs) {
		if (this == &rhs) return *this;
		Ins_type = rhs.Ins_type;
		imm = rhs.imm;
		label = rhs.label;
		offset = rhs.offset;
		for (int i = 0; i < 3; ++i) regist[i] = rhs.regist[i];

		return *this;
	}
	void clear() {
		Ins_type = -1;
		for (int i = 0; i < 3; ++i) regist[i] = EMPTY;
		imm = label = offset = 0;
	}
};

class IF : public instruction {
public:
	bool block = 0;
	bool jump_block = 0;

};

class ID : public instruction {
public:
	bool block;
	bool tovis[3];
	int reg[3];

	void push(IF &rhs) {
		instruction *p = this;
		*p = rhs;
		p = &rhs;
		p->clear();
	}
};

class EX : public instruction {
public:
	int reg[3];
	void push(ID &rhs) {
		instruction *p = this;
		*p = rhs;
		p = &rhs;
		p->clear();
		for (int i = 0; i < 3; ++i) reg[i] = rhs.reg[i];
	}
};

class MA : public instruction {
public:
	void push(EX &rhs) {
		instruction *p = this;
		*p = rhs;
		p = &rhs;
		p->clear();
	}
};

class WB : public instruction {
public:
	void push(MA &rhs) {
		instruction *p = this;
		*p = rhs;
		p = &rhs;
		p->clear();
	}
};
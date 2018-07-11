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
};

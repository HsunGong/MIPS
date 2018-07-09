#pragma once

#include "Header.h"
#include "storage.hpp"

void instruction::load(int cur) {
	Ins_type = _ram.memory[cur++];
	for (int i = 0; i < 3; ++i) {
		regist[i] = _ram.memory[cur++];
	}
	imm = *((int*)(_ram.memory + cur));
	label = *((int*)(_ram.memory + cur + sizeof(int)));
	offset = *((int*)(_ram.memory + cur + 2 * sizeof(int)));

}

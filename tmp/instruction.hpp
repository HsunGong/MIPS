#pragma once

#include "Header.h"
#include "storage.hpp"

void operatorcode::load(int loc) {
		operatorindex = _ram.memory[loc++];
		for (int i = 0; i < 3; ++i) {
			regist[i] = _ram.memory[loc++];
		}
		imm = *((int*)(_ram.memory + loc));
		label = *( (int*)(_ram.memory + loc + sizeof(int)) );
		delta = *( (int*)(_ram.memory + loc + 2*sizeof(int)) );
		
}

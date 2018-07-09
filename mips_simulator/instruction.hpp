#pragma once

#include "Header.h"
#include "storage.hpp"

void instruction::load(int loc) {
		status = _ram.memory[loc++];
		for (int i = 0; i < 3; ++i) {
			regist[i] = _ram.memory[loc++];
		}
		rdest = _ram.memory[loc++];
		rsrc1 = _ram.memory[loc++];
		rsrc2 = _ram.memory[loc++];


		imm = *((int*)(_ram.memory + loc));
		label = *( (int*)(_ram.memory + loc + sizeof(int)) );
		offset = *( (int*)(_ram.memory + loc + 2*sizeof(int)) );
		
}

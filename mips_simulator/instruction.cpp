#include "instruction.hpp"

extern map<string, int> label;
extern vector<string> forIns;
extern map<string, int> Ins_map;
extern vector<string> forReg;
extern map<string, int> Reg_map;
extern ram _ram;
extern vector<string> Code;

void instruction::load(int cur) {
	Ins_type = _ram.memory[cur++];
	for (int i = 0; i < 3; ++i) {
		regist[i] = _ram.memory[cur++];
	}
	imm = *((int*)(_ram.memory + cur));
	label = *((int*)(_ram.memory + cur + sizeof(int)));
	offset = *((int*)(_ram.memory + cur + 2 * sizeof(int)));

}


instruction::instruction(int8_t type) : Ins_type(type) {
	for (int i = 0; i < 3; ++i) regist[i] = EMPTY;
	offset = 0;
}
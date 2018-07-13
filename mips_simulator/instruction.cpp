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

#ifdef PIPELINE

instruction& instruction::operator=(instruction &rhs) {
	if (this == &rhs) return *this;
	Ins_type = rhs.Ins_type;
	imm = rhs.imm;
	label = rhs.label;
	offset = rhs.offset;
	for (int i = 0; i < 3; ++i) regist[i] = rhs.regist[i];

	return *this;
}
void instruction::clear() {
	Ins_type = -1;
	for (int i = 0; i < 3; ++i) regist[i] = EMPTY;
	imm = label = offset = 0;
}

void IF::push(int cur) {
	instruction *p = this;
	p->load(cur);
}

void ID::push(IF &rhs) {
	instruction *p = this;
	*p = rhs;
	p = &rhs;
	p->clear();
}

void EX::push(ID &rhs) {
	instruction *p = this;
	*p = rhs;
	p = &rhs;
	p->clear();
	for (int i = 0; i < 3; ++i) reg[i] = rhs.reg[i];
}

void MA::push(EX &rhs) {
	instruction *p = this;
	*p = rhs;
	p = &rhs;
	p->clear();
}

void WB::push(MA &rhs) {
	instruction *p = this;
	*p = rhs;
	p = &rhs;
	p->clear();
}

#else

instruction &instruction::operator=(instruction &rhs) { return *this; }
void instruction::clear(){}

#endif


#ifdef PREDICT

class predictor {
	//one bit
	bool flag = 0;
	void tranverse() {
		flag = !flag;
	}
};

#endif
#define DEBUG
//#define RELEASE


#include "Header.h"
#include "instruction.hpp"

vector<instruction*> _instruction;

//interprete
void fetch(ifstream &fin) {
	string str;// instruction sentences
	int ins_cnt = 0;
	vector<string> _name, _r1, _r2, _r3;
	bool text_block = 0;
	//if r2, r3 not exist --> it is ""

	instruction *p;
	string name, r1, r2, r3;


	while (getline(fin, str)) {
		string tmp;
		int i = 0;
		while (str[i] == ' ' || str[i] == '\t') ++i;

		if (str[i] == '.') {// "." order
			++i;
			tmp = get_phrase(str, i);
			if (tmp == "align") {
				++i;
				tmp = get_phrase(str, i);
				int n = stoi(tmp);
				n = pow(n, 2);
				_memory.heap_top += (n - _memory.heap_top % n) % n;
			}
			else if (tmp == "ascii" || tmp == "asciiz") {
				i += 2;
				tmp = get_str(str, i);
				for (int i = 0; i < tmp.length(); ++i)
					_memory.mem[_memory.heap_top++] = tmp[i];
				if (tmp == "asciiz") _memory.mem[_memory.heap_top++] = '\0';
			}
			else if (tmp == "byte" || tmp == "half" || tmp == "word") {
				int m = tmp == "byte" ? 0 : (tmp == "half" ? 1 : 2);
				while (1) {
					if (i == l) break;
					++i;
					tmp = get_phrase(str, i);
					int n = stoi(tmp);
					memcpy(_memory.mem + _memory.heap_top, &n, m);
				}
			}
			else if (tmp == "space") {
				++i;
				tmp = get_phrase(str, i);
				int n = stoi(tmp);
				_memory.heap_top += n;
			}
			else if (tmp == "data" || tmp == "text") {
				text_block = (tmp == "text");
			}
		}
		else if (str.back() == ':') {
			string tmp = get_phrase(str, i);
			if (text_block) text_label[tmp] = ins_cnt;
			else data_label[tmp] = _memory.heap_top;
		}
		else {//text's instruction
			try {
				_name.push_back(get_phrase(str, i));	++i;
				++ins_cnt;
				_r1.push_back(get_phrase(str, i));		++i;// attention has $
				_r2.push_back(get_phrase(str, i));		++i;
				_r3.push_back(get_phrase(str, i));		++i;
			}
			catch (...) {
				continue;
			}
		}
	}


	for (int i = 0; i < ins_cnt; ++i) {
			name = _name[i];
			r1 = _r1[i];
			r2 = _r2[i];
			r3 = _r3[i];
			

			if (name == "la")	   p = new la(r1, r2);
			else if (name == "lb") p = new lb(r1, r2);
			else if (name == "lh") p = new lb(r1, r2);
			else if (name == "lw") p = new lb(r1, r2);

			else if (name == "sb") p = new sb(r1, r2);
			else if (name == "sh") p = new sh(r1, r2);
			else if (name == "sw") p = new sw(r1, r2);

			else if (name == "li")   p = new Move(r1, r2);
			else if (name == "move") p = new Move(r1, r2);
			else if (name == "mfhi") p = new mfhi(r1);
			else if (name == "mflo") p = new mflo(r1);

			else if (name == "add") p = new add(r1, r2, r3, 1);
			else if (name == "addu" || name == "addiu") p = new add(r1, r2, r3, 0);
			else if (name == "sub")  p = new sub(r1, r2, r3, 1);
			else if (name == "subu") p = new sub(r1, r2, r3, 0);
			else if (name == "mul")  p = new mul(r1, r2, r3, 1);
			else if (name == "mulu") p = new mul(r1, r2, r3, 0);
			else if (name == "div")  p = new Div(r1, r2, r3, 1);
			else if (name == "divu") p = new Div(r1, r2, r3, 0);
			else if (name == "xor")  p = new Xor(r1, r2, r3, 1);
			else if (name == "xoru") p = new Xor(r1, r2, r3, 0);
			else if (name == "neg")  p = new neg(r1, r2, r3, 1);
			else if (name == "negu") p = new neg(r1, r2, r3, 0);
			else if (name == "rem")  p = new rem(r1, r2, r3, 1);
			else if (name == "remu") p = new rem(r1, r2, r3, 0);

			else if (name == "nop")	p = new instruction();
			else if (name == "lb")	p = new syscall(cin, cout);

			else if (name == "seq")	p = new comparation(r1, r2, r3, comparation::seq);
			else if (name == "sge") p = new comparation(r1, r2, r3, comparation::sge);
			else if (name == "sgt") p = new comparation(r1, r2, r3, comparation::sgt);
			else if (name == "sle") p = new comparation(r1, r2, r3, comparation::sle);
			else if (name == "slt") p = new comparation(r1, r2, r3, comparation::slt);
			else if (name == "sne") p = new comparation(r1, r2, r3, comparation::sne);

			else if (name == "b" || name == "jr" || name == "j")p = new b_jump(r1, 0);
			else if (name == "jalr" || name == "jal")			p = new b_jump(r1, 1);
			else if (name == "beq")		p = new beq(r1, r2, r3);
			else if (name == "beqz")	p = new beq(r1, 0, r3);
			else if (name == "bne")		p = new beq(r1, r2, r3);
			else if (name == "bnez")	p = new beq(r1, 0, r3);
			else if (name == "bge")		p = new bge(r1, r2, r3);
			else if (name == "bgez")	p = new bge(r1, 0, r3);
			else if (name == "ble")		p = new ble(r1, r2, r3);
			else if (name == "blez")	p = new ble(r1, 0, r3);
			else if (name == "bgt")		p = new bgt(r1, r2, r3);
			else if (name == "bgtz")	p = new bgt(r1, 0, r3);
			else if (name == "blt")		p = new blt(r1, r2, r3);
			else if (name == "bltz")	p = new blt(r1, 0, r3);
			_instruction.push_back(p);
		}
	
}

void execute_pipeline() {
	_memory.text_top = text_label["main"];
	int ins_size = _instruction.size();
	int cnt = 0;
	queue<instruction*> IF, ID, Ex, MA, WB;
	instruction* cur = nullptr;
	while (_memory.text_top < ins_size || !IF.empty() || !ID.empty() || !Ex.empty() || !MA.empty() || !WB.empty()) {
		//MA-->WB-->Ex-->ID-->IF:simualtor 5 level
		//write first, then read

		//MA
		if (!MA.empty()) {
			cur = MA.front();
			MA.pop();
			cur->memory_access();
			delete cur;
			cur = nullptr;
		}
		//WB
		if (!WB.empty()) {
			cur = WB.front();
			WB.pop();
			cur->write_back();
			MA.push(cur);
		}
		//Ex
		if (!Ex.empty()) {
			cur = Ex.front();
			Ex.pop();
			cur->execute();
			MA.push(cur);
		}
		//ID
		if (!ID.empty()) {
			cur = ID.front();
			ID.pop();
			cur->data_preparation();
			Ex.push(cur);
		}

		//IF
		if (_memory.text_top < ins_size) IF.push(_instruction[_memory.text_top++]->copy());
		if (!IF.empty()) {
			cur = IF.front();
			IF.pop();
			//already IF
			ID.push(cur);
		}
	}
}

void execute_simple() {
	_memory.text_top = text_label["main"];
	int ins_vec_sz = _instruction.size();
	int cnt = 0;
	while (_memory.text_top < ins_vec_sz) {
		//cout << "\nins: " << _memory.text_top << endl;
		instruction *ptr = _instruction[_memory.text_top++]->copy();
		ptr->data_preparation();
		ptr->execute();
		ptr->memory_access();
		ptr->write_back();
		delete ptr;
		/*
		for (int i = 0; i < 16; ++i)
		file << setw(5) << reg[i] << " ";
		for (int i = 32; i < 34; ++i)
		file << reg[i] << " ";
		file << endl;
		*/
	}
}

void clear() {
	auto it = _instruction.begin();
	while (it != _instruction.end()) 	_instruction.erase(it);
}








#define SIMPLE
//#define FIVE_PIPELINE


int main(int argc, char *argv[]) {
	ifstream main_fin;
	main_fin.open(argv[1]);
	if (!main_fin) throw file_error();


	fetch(main_fin);

#ifdef SIMPLE 
	execute_simple();

#endif
#ifdef FIVE_PIPELINE
	execute_pipeline();
#endif

	clear();
	main_fin.close();

	return 0;
}
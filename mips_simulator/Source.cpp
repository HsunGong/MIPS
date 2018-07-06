#define DEBUG
//#define RELEASE


#include "Header.h"
#include "instruction.hpp"

vector<instruction*> _instruction;

void translate(string &name, string &r1, string &r2, string &r3, istream &sysin, ostream &sysout) {
	instruction *p;

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
	else if (name == "syscall")	p = new syscall(sysin, sysout);

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


//interprete
void fetch(ifstream &fin, istream &sysin, ostream &sysout) {
	string str;// instruction sentences
	int ins_cnt = 0;
	//vector<string> _name, _r1, _r2, _r3;
	bool text_block = 0;

	//if r2, r3 not exist --> it is ""

	while (getline(fin, str)) {
		string tmp;
		int i = 0;
		while (str[i] == ' ' || str[i] == '\t') ++i;

		if (str[i] == '.') {// "." order
			++i;
			tmp = get_phrase(str, i);
			if (tmp == "align") {
				++i;
				int n = stoi(get_phrase(str, i));
				n = pow_2(n);
				_memory.heap_top += (n - _memory.heap_top % n);// again % n;
			}
			else if (tmp == "ascii" || tmp == "asciiz") {
				++i;
				tmp = to_str(str, i);
				memcpy(_memory.mem + _memory.heap_top, tmp.data(), tmp.length());
				_memory.heap_top += tmp.length();
				if (tmp == "asciiz") _memory.mem[_memory.heap_top++] = '\0';
			}
			else if (tmp == "byte" || tmp == "half" || tmp == "word") {
				int m = ( tmp == "byte" ? 1 : (tmp == "half" ? 2 : 4) );
				if (i == str.length()) throw order_error();
				while (1) {
					//if (i == str.length()) break;				
					try {
						++i;
						int n = stoi(get_phrase(str, i));
						memcpy(_memory.mem + _memory.heap_top, &n, m);
						_memory.heap_top += m;
					}
					catch (...) {
						break;
					}
					
					
				}
			}
			else if (tmp == "space") {
				++i;
				int n = stoi(get_phrase(str, i));
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
			int ccc = ins_cnt;
			string name, r1, r2, r3;
			try {
				name = get_phrase(str, i);	++i;
				++ins_cnt;
				r1 = get_phrase(str, i);	++i;// attention has $
				r2 = get_phrase(str, i);	++i;
				r3 = get_phrase(str, i);	++i;
			}
			catch (...) {
				if(ccc == ins_cnt) continue;

				translate(name, r1, r2, r3, sysin, sysout);
			}
		}
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

#ifdef RELEASE
	ifstream main_fin;
	main_fin.open(argv[1]);
	if (!main_fin) throw file_error();
	fetch(main_fin, cin, cout);

#ifdef SIMPLE 
	execute_simple();

#endif

#ifdef FIVE_PIPELINE
	execute_pipeline();
#endif

	clear();
	main_fin.close();

#endif

#ifdef DEBUG	
for (int i = 1; i <= 60; ++i) {
	char num = '0' + i;
	string name = "../test/data/"; // or string name =  "..\\test\\data\\";
	name += num;

	ifstream main_fin(name + ".s");
	ifstream testin(name + ".in");

	if (!main_fin || !testin) throw file_error();

	fetch(main_fin, testin, cout);

#ifdef SIMPLE 
		execute_simple();
#endif
#ifdef FIVE_PIPELINE
		execute_pipeline();
#endif

	clear();
	main_fin.close();
	testin.close();

	cin >> name;
}
#endif


	return 0;
}
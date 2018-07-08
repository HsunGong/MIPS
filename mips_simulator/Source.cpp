#include "Header.h"
#include "instruction.hpp"
#include "storage.hpp"



void manageString(string &line, int &loc, int len) {
	if (line[loc] == '.') {
		int next = loc, k, cmp, multime = 1;
		for (; line[next] != ' ' && line[next] != '\t' && line[next] != '\n'; ++next);
		string key(&line[loc + 1], &line[next]), str;

		switch (key[3]) {// '.' order
		case('g')://align
			k = mstoi(line, loc);
			cmp = 2 << (k - 1);
			//while (_ram.mem_low > cmp) {
			//	cmp += 2 << (k - 1);
			//	++multime;
			//}
			for (; _ram.mem_low > cmp; cmp += (2 << (k - 1)), ++multime);
			_ram.mem_low = cmp;
			break;
		case('i')://ascii/asciiz
			//while (skip(line[next])) ++next;
			//loc = next;// has sth
			//++next;//next is (left)'\"'
			//while (line[next] != '\"') ++next;//right "
			for (; line[next] == ' ' || line[next] == '\t'; ++next); loc = next;
			for (++next; line[next] != '\"'; ++next);

			str = string(&line[loc + 1], &line[next]);
			_ram.saveString(str, _ram.mem_low);
			if (key.length() == 6) _ram.memory[_ram.mem_low++] = '\0';
			break;
		case('e')://byte
			do {_ram.saveInt(mstoi(line, loc), _ram.mem_low, 1);}//whether it is oko?
			while (line[loc++] == ',');///???????? is it work>>
			break;
		case('f')://half
			//do {_ram.saveInt(mstoi(line, loc), _ram.mem_low, 2);} 
			//while (line[loc++] == ',');
			for (_ram.saveInt(mstoi(line, loc), _ram.mem_low, 2); line[loc] == ','; _ram.saveInt(mstoi(line, ++loc), _ram.mem_low, 1));

			break;
		case('d')://word
			//do {_ram.saveInt(mstoi(line, loc), _ram.mem_low, 4);}
			//while (line[loc++] == ',');
			for (_ram.saveInt(mstoi(line, loc), _ram.mem_low, 4); line[loc] == ','; _ram.saveInt(mstoi(line, ++loc), _ram.mem_low, 1));

			break;
		case('c')://space
			_ram.mem_low += mstoi(line, loc);
			break;
		default://data---text
			return;
		}
	}
	else {//instructions
		int nxt = loc;
		for (; line[nxt] != ' ' && line[nxt] != '\t' && line[nxt] != '\n'; ++nxt);
		string operatorname(&line[loc], &line[nxt]);
		loc = nxt;
		////First_scanf:
		//ADD, ADDU, ADDIU, SUB, SUBU, XOR, XORU, REM, REMU, SEQ, SGE, SGT, SLE, SLT, SNE, // Rd R1 R2/Imm
		//	NEG, NEGU, LI, MOVE, //Rd R1/Imm
		//	MFHI, MFLO, //Rd
		//	JR, JALR, //R1
		//	NOP, SYSCALL,
		//	MUL, MULU, DIV, DIVU, //R1 R2/Imm
		//						  //Second_scanf:
		//	B, J, JAL, //Label 
		//	BEQ, BNE, BGE, BLE, BGT, BLT, //R1 R2/Imm Label 
		//	BEQZ, BNEZ, BLEZ, BGEZ, BGTZ, BLTZ, //R1 Label 
		//	LA, LB, LH, LW, //Rd Address (Del) R1 -> Rd
		//	SB, SH, SW //R1 Address (Del) R1 -> Rd
		int _index = OperatorIndex[operatorname];
		if (_index <= SNE) _ram.saveStruct(line, loc, len, 0, 2, _index, 0);
		else if (_index <= MOVE) _ram.saveStruct(line, loc, len, 0, 1, _index, 0);
		else if (_index <= MFLO) _ram.saveStruct(line, loc, len, 0, 0, _index, 0);
		else if (_index <= JALR) _ram.saveStruct(line, loc, len, 1, 1, _index, 0);
		else if (_index <= SYSCALL) _ram.saveStruct(line, loc, len, 0, -1, _index, 0);
		else if (_index <= DIVU) _ram.saveStruct(line, loc, len, 0, 2, _index, 0);
		else if (_index <= JAL) _ram.saveStruct(line, loc, len, 0, -1, _index, 1);
		else if (_index <= BLT) _ram.saveStruct(line, loc, len, 1, 2, _index, 1);
		else if (_index <= BLTZ) _ram.saveStruct(line, loc, len, 1, 1, _index, 1);
		else if (_index <= LW) _ram.saveStruct(line, loc, len, 0, 0, _index, 1);
		else _ram.saveStruct(line, loc, len, 1, 1, _index, 1);
	}

}


void code_scanf() {
	string line;
	for (int i = 0; i < Code.size(); ++i) {
		line = Code[i];
		int len = line.length(), loc = 0;

		int annotation = line.find('#');//deal annotation
		if (annotation != -1) {
			line.erase(annotation, len - annotation);
			line.push_back('\n');
			len = annotation + 1;
		}

		for (; loc < len && (line[loc] == ' ' || line[loc] == '\t' || line[loc] == '\n'); ++loc); // "  .data" -> loc: "  ^data"
		if (loc == len) continue;//wrong code
		
		int pos = line.find(':');
		int ept = line.find('\"');
		while (pos >= 0 && (ept == -1 || pos < ept)) {//deal label
			string label_name(&line[loc], &line[pos]);
			label[label_name] = _ram.mem_low;

			for (++pos; pos < len && (line[pos] == ' ' || line[pos] == '\t' || line[pos] == '\n'); ++pos);
			line.erase(0, pos);
			loc = 0;
			len -= pos;

			pos = line.find(':');
			ept = line.find('\"');
		}
		if (!len) continue;

		manageString(line, loc, len);//other orders
	}
}

void init(const char * argv, ifstream &fin, ostream &fout) {
	fin.open(argv);

	//prepare()
	for (int i = 0; i < forOperator.size(); ++i) OperatorIndex[forOperator[i]] = i;
	for (int i = 0; i < forRegister.size(); ++i) RegisterIndex[forRegister[i]] = i;
	RegisterIndex["s8"] = 30;

	//code_in();
	string tmp;
	while (getline(fin, tmp)) {
		tmp.push_back('\n');
		Code.push_back(tmp);
	}

	code_scanf();
	_ram.mem_low = 0;
	code_scanf();
	_ram.reg[SP] = M - 1;
}

int simulate(ifstream &fin, ostream &fout) {
	int cur_loc = label["main"];
	operatorcode cur_code;
	int cnt = 0;
	while (1) {
		cur_code.load(cur_loc);

		if(cnt >= 7) Debug(cur_code, cnt);
		else cnt++;

		int tmp;// for compare order
		int loc;//for store order
		switch (cur_code.operatorindex) {
		case(ADD):
		case(ADDU):
		case(ADDIU):
			if (cur_code.regist[2] == EMPTY) _ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] + cur_code.imm;
			else _ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] + _ram.reg[cur_code.regist[2]];
			break;

		case(SUB):
		case(SUBU):
			if (cur_code.regist[2] == EMPTY) _ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] - cur_code.imm;
			else _ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] - _ram.reg[cur_code.regist[2]];
			break;

		case(XOR):
		case(XORU):
			if (cur_code.regist[2] == EMPTY) _ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] ^ cur_code.imm;
			else _ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] ^ _ram.reg[cur_code.regist[2]];
			break;
		
		case(REM):
			if (cur_code.regist[2] == EMPTY) _ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] % cur_code.imm;
			else _ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] % _ram.reg[cur_code.regist[2]];
			break;
		case(REMU):
			if (cur_code.regist[2] == EMPTY) _ram.reg[cur_code.regist[0]] = (uint32_t) _ram.reg[cur_code.regist[1]] % (uint32_t)cur_code.imm;
			else _ram.reg[cur_code.regist[0]] = (uint32_t)_ram.reg[cur_code.regist[1]] % (uint32_t)_ram.reg[cur_code.regist[2]];
			break;//deal unsigned


		case(SEQ):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] == tmp) _ram.reg[cur_code.regist[0]] = 1;
			else _ram.reg[cur_code.regist[0]] = 0;
			break;
		case(SGE):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] >= tmp) _ram.reg[cur_code.regist[0]] = 1;
			else _ram.reg[cur_code.regist[0]] = 0;
			break;
		case(SGT):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] > tmp) _ram.reg[cur_code.regist[0]] = 1;
			else _ram.reg[cur_code.regist[0]] = 0;
			break;
		case(SLE):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] <= tmp) _ram.reg[cur_code.regist[0]] = 1;
			else _ram.reg[cur_code.regist[0]] = 0;
			break;
		case(SLT):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] < tmp) _ram.reg[cur_code.regist[0]] = 1;
			else _ram.reg[cur_code.regist[0]] = 0;
			break;
		case(SNE):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] != tmp) _ram.reg[cur_code.regist[0]] = 1;
			else _ram.reg[cur_code.regist[0]] = 0;
			break;

		case(NEG):
		case(NEGU):
			_ram.reg[cur_code.regist[0]] = -_ram.reg[cur_code.regist[1]];
			break;

		case(LI):
			_ram.reg[cur_code.regist[0]] = cur_code.imm;
			break;
		case(MOVE):
			_ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]];
			break;
		case(MFHI):
			_ram.reg[cur_code.regist[0]] = _ram.reg[HI];
			break;
		case(MFLO):
			_ram.reg[cur_code.regist[0]] = _ram.reg[LO];
			break;

		case(JR):
			cur_loc = _ram.reg[cur_code.regist[1]] - NXT;
			break;
		case(JALR):
			_ram.reg[31] = cur_loc + NXT;
			cur_loc = _ram.reg[cur_code.regist[1]] - NXT;
			break;

		case(MUL): 
			if (cur_code.delta == 2) {
				int64_t tmp = _ram.reg[cur_code.regist[0]];
				if (cur_code.regist[1] == EMPTY) tmp *= cur_code.imm;
				else tmp *= _ram.reg[cur_code.regist[1]];
				_ram.reg[LO] = (int32_t)(tmp);
				_ram.reg[HI] = tmp >> 32;
			}
			else {
				int64_t tmp = _ram.reg[cur_code.regist[1]];
				if (cur_code.regist[2] == EMPTY) tmp *= cur_code.imm;
				else tmp *= _ram.reg[cur_code.regist[2]];
				_ram.reg[cur_code.regist[0]] = (int32_t)(tmp);
			}
			break;
		case(MULU):
			if (cur_code.delta == 2) {
				uint64_t tmp = _ram.reg[cur_code.regist[0]];
				if (cur_code.regist[1] == EMPTY) tmp *= (uint32_t) cur_code.imm;
				else tmp *= (uint32_t)_ram.reg[cur_code.regist[1]];
				_ram.reg[LO] = (uint32_t)(tmp);
				_ram.reg[HI] = tmp >> 32;
			}
			else {
				uint32_t tmp = _ram.reg[cur_code.regist[1]];
				if (cur_code.regist[2] == EMPTY) tmp *= (uint32_t)cur_code.imm;
				else tmp *= (uint32_t)_ram.reg[cur_code.regist[2]];
				_ram.reg[cur_code.regist[0]] = (uint32_t)(tmp);
			}
			break;
		
		case(DIV):
			if (cur_code.delta == 2) {
				int tmp = (cur_code.regist[1] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[1]];
				_ram.reg[LO] = _ram.reg[cur_code.regist[0]] / tmp;
				_ram.reg[HI] = _ram.reg[cur_code.regist[0]] % tmp;
			}
			else {
				int tmp = (cur_code.regist[2] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[2]];
				_ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] / tmp;
			}
			break;
		case(DIVU):
			if (cur_code.delta == 2) {
				uint32_t tmp = (cur_code.regist[1] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[1]];
				_ram.reg[LO] = ((uint32_t) _ram.reg[cur_code.regist[0]]) / tmp;
				_ram.reg[HI] = ((uint32_t)_ram.reg[cur_code.regist[0]]) % tmp;
			}
			else {
				uint32_t tmp = (cur_code.regist[2] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[2]];
				_ram.reg[cur_code.regist[0]] = ((uint32_t)_ram.reg[cur_code.regist[1]]) / tmp;
			}
			break;

		case(B):
		case(J):
			cur_loc = cur_code.label - NXT;
			break;
		case(JAL):
			_ram.reg[31] = cur_loc + NXT;
			cur_loc = cur_code.label - NXT;
			break;

		case(BEQ):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] == tmp) cur_loc = cur_code.label - NXT;
			break;
		case(BNE):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] != tmp) cur_loc = cur_code.label - NXT;
			break;
		case(BGE):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] >= tmp) cur_loc = cur_code.label - NXT;
			break;
		case(BLE):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] <= tmp) cur_loc = cur_code.label - NXT;
			break;
		case(BGT):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] > tmp) cur_loc = cur_code.label - NXT;
			break;
		case(BLT):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] < tmp) cur_loc = cur_code.label - NXT;
			break;

		case(BEQZ):
			if (_ram.reg[cur_code.regist[1]] == 0) cur_loc = cur_code.label - NXT;
			break;
		case(BNEZ):
			if (_ram.reg[cur_code.regist[1]] != 0) cur_loc = cur_code.label - NXT;
			break;
		case(BLEZ):
			if (_ram.reg[cur_code.regist[1]] <= 0) cur_loc = cur_code.label - NXT;
			break;
		case(BGEZ):
			if (_ram.reg[cur_code.regist[1]] >= 0) cur_loc = cur_code.label - NXT;
			break;
		case(BGTZ):
			if (_ram.reg[cur_code.regist[1]] > 0) cur_loc = cur_code.label - NXT;
			break;
		case(BLTZ):
			if (_ram.reg[cur_code.regist[1]] < 0) cur_loc = cur_code.label - NXT;
			break;

		case(LA):
			if (cur_code.regist[1] == EMPTY) _ram.reg[cur_code.regist[0]] = cur_code.label;
			else  _ram.reg[cur_code.regist[0]] = cur_code.regist[1] + cur_code.delta;
			break;
		case(LB):
			if (cur_code.regist[1] == EMPTY) _ram.reg[cur_code.regist[0]] = *((int8_t*)(_ram.memory + cur_code.label));
			else  _ram.reg[cur_code.regist[0]] = *((int8_t*)(_ram.memory + _ram.reg[cur_code.regist[1]] + cur_code.delta));
			break;
		case(LH):
			if (cur_code.regist[1] == EMPTY) _ram.reg[cur_code.regist[0]] = *((int16_t*)(_ram.memory + cur_code.label));
			else  _ram.reg[cur_code.regist[0]] = *((int16_t*)(_ram.memory + _ram.reg[cur_code.regist[1]] + cur_code.delta));
			break;
		case(LW):
			if (cur_code.regist[1] == EMPTY) _ram.reg[cur_code.regist[0]] = *((int32_t*)(_ram.memory + cur_code.label));
			else  _ram.reg[cur_code.regist[0]] = *((int32_t*)(_ram.memory + _ram.reg[cur_code.regist[1]] + cur_code.delta));
			break;

		case(SB):
			if (cur_code.regist[0] == EMPTY) loc = cur_code.label;
			else loc = _ram.reg[cur_code.regist[0]] + cur_code.delta;
			_ram.saveInt(_ram.reg[cur_code.regist[1]], loc, 1);
			break;
		case(SH):
			if (cur_code.regist[0] == EMPTY) loc = cur_code.label;
			else loc = _ram.reg[cur_code.regist[0]] + cur_code.delta;
			_ram.saveInt(_ram.reg[cur_code.regist[1]], loc, 2);
			break;
		case(SW):
			if (cur_code.regist[0] == EMPTY) loc = cur_code.label;
			else loc = _ram.reg[cur_code.regist[0]] + cur_code.delta;
			_ram.saveInt(_ram.reg[cur_code.regist[1]], loc, 4);
			break;

		case(NOP):
			break;
		case(SYSCALL):
			switch (_ram.reg[V0]) {
			case(1):
				fout << _ram.reg[A0];
				break;
			case(4): {
				int loc = _ram.reg[A0];
				while (_ram.memory[loc] != '\0') fout << _ram.memory[loc++];
				break;
			}

			case(5):
				cin >> _ram.reg[V0];
				break;
			case(8): {// if {} then can define int, string
				int loc = _ram.reg[A0];
				string str;
				cin >> str;
				if (str.length() > _ram.reg[A1] - 1)
					//str = str.substr(0, _ram.reg[A1] - 1);
					str = string(&str[0], &str[_ram.reg[A1] - 1]);
				_ram.saveString(str, loc);
				break;
			}
			case(9):
				_ram.reg[V0] = _ram.mem_low;
				_ram.mem_low += _ram.reg[A0];
				break;
			case(10):
				return 0;
			case(17):
				return _ram.reg[A0];
			default:
				break;//wrong code
			}
			break;

		default://wrong
			break;
		}


		cur_loc += NXT;//step

	}
}

#define DEBUG

int main(int argc, char *argv[]) {
	ifstream fin;

	//ofstream fout;
	ostream &fout = cout;

	//init(argv[1], fin, fout);
	init("..\\test\\data\\2.s", fin, fout);



	int ex = simulate(fin, fout);

	fin.close();
	exit(ex);// or return ex

	return 0;
}
#include "Header.h"
#include "instruction.hpp"
#include "storage.hpp"



void manageString(string &line, int &cur, int len) {
	if (line[cur] == '.') {
		int next = cur, k, cmp, multime = 1;
		//for (; line[next] != ' ' && line[next] != '\t' && line[next] != '\n'; ++next);
		while (!skip(line[next])) ++next;
		string key(&line[cur + 1], &line[next]), str;

		switch (key[3]) {// '.' order
		case('g')://align
			k = mstoi(line, cur);
			cmp = 2 << (k - 1);
			//while (_ram.mem_low > cmp) {
			//	cmp += 2 << (k - 1);
			//	++multime;
			//}
			for (; _ram.mem_low > cmp; cmp += (2 << (k - 1)), ++multime);
			_ram.mem_low = cmp;
			break;
		case('i')://ascii/asciiz
			//for (; line[next] == ' ' || line[next] == '\t'; ++next); 
			while (skip(line[next])) ++next;// '\n'???
			cur = next++;//left "
			while(line[next] != '\"') ++next;// right "

			str = string(&line[cur + 1], &line[next]);
			_ram.saveString(str, _ram.mem_low);
			if (key.length() == 6) _ram.memory[_ram.mem_low++] = '\0';
			break;
		case('e')://byte
			do {_ram.saveInt(mstoi(line, cur), _ram.mem_low, 1);}
			while (line[cur++] == ',');
			break;
		case('f')://half
			do {_ram.saveInt(mstoi(line, cur), _ram.mem_low, 2);} 
			while (line[cur++] == ',');
			break;
		case('d')://word
			do {_ram.saveInt(mstoi(line, cur), _ram.mem_low, 4);}
			while (line[cur++] == ',');
			//for (_ram.saveInt(mstoi(line, cur), _ram.mem_low, 4); line[cur] == ','; _ram.saveInt(mstoi(line, ++cur), _ram.mem_low, 1));
			break;
		case('c')://space
			_ram.mem_low += mstoi(line, cur);
			break;
		default://data---text
			return;
		}
	}
	else {//instructions
		int next = cur;
		while(!skip(line[next])) ++next;
		string ins_name(&line[cur], &line[next]);

		cur = next;//the last name of instruction + 1
		int _index = Ins_type[ins_name];
		switch (_index) {
		//First_scanf:
		case ADD: case ADDU: case ADDIU: case SUB: case SUBU:
		case XOR: case XORU: case REM: case REMU:
		case SEQ: case SGE: case SGT: case SLE: case SLT: case SNE:// Rd, R1, R2/Imm
			_ram.saveStruct(line, cur, len, 0, 2, _index, 0);break;
		case NEG: case NEGU: case LI: case MOVE://Rd, R1/Imm
			_ram.saveStruct(line, cur, len, 0, 1, _index, 0);break;
		case MFHI: case MFLO://Rd
			_ram.saveStruct(line, cur, len, 0, 0, _index, 0);break;
		case JR: case JALR://R1
			_ram.saveStruct(line, cur, len, 1, 1, _index, 0);break;
		case NOP: case SYSCALL://NULL
			_ram.saveStruct(line, cur, len, 0, -1, _index, 0);break;
		case MUL: case MULU: case DIV: case DIVU: //R1, R2/Imm
			_ram.saveStruct(line, cur, len, 0, 2, _index, 0);break;
			//Second_scanf:
		case B: case J: case JAL: //Label
			_ram.saveStruct(line, cur, len, 0, -1, _index, 1);break;
		case BEQ: case BNE: case BGE: case BLE: case BGT: case BLT: //R1, R2/Imm, Label 
			_ram.saveStruct(line, cur, len, 1, 2, _index, 1);break;
		case BEQZ: case BNEZ: case BLEZ: case BGEZ: case BGTZ: case BLTZ: //R1,,mm ,  Label 
			_ram.saveStruct(line, cur, len, 1, 1, _index, 1);break;
		case LA: case LB: case LH: case LW: //Rd, Address, (Del) R1 -> Rd
			_ram.saveStruct(line, cur, len, 0, 0, _index, 1);break;
		case SB: case SH: case SW: //R1, Address, (Del) R1 -> Rd
			_ram.saveStruct(line, cur, len, 1, 1, _index, 1);break;
		}
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
	for (int i = 0; i < forInstruction.size(); ++i) Ins_type[forInstruction[i]] = i;
	for (int i = 0; i < forRegister.size(); ++i) Reg_type[forRegister[i]] = i;
	Reg_type["s8"] = 30;

	//code_in();
	string tmp;
	while (getline(fin, tmp)) {
		tmp.push_back('\n');
		Code.push_back(tmp);
	}

	code_scanf();
	_ram.mem_low = 0;// will change!!!!
	code_scanf();
}

int simulate(ifstream &fin, ostream &fout) {
	int cur_loc = label["main"];
	instruction cur_code;
	int cnt = 0;
	while (1) {
		cur_code.load(cur_loc);

		if(cnt >= 7) Debug(cur_code, cnt);
		else cnt++;

		int tmp;// for compare order
		int loc;//for store order
		switch (cur_code.status) {
		case(ADD):
		case(ADDU):
		case(ADDIU):
			_ram.reg[cur_code.rdest] = (cur_code.rsrc2 == EMPTY) ? 
				_ram.reg[cur_code.rsrc1] + cur_code.imm :
				_ram.reg[cur_code.rsrc1] + _ram.reg[cur_code.rsrc2];
			break;

		case(SUB):
		case(SUBU):
			_ram.reg[cur_code.rdest] = _ram.reg[cur_code.rsrc1] -
				((cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2]);
			break;

		case(XOR):
		case(XORU):
			_ram.reg[cur_code.rdest] = _ram.reg[cur_code.rsrc1] ^
				((cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2]);
			break;
		
		case(REM):
			_ram.reg[cur_code.rdest] = _ram.reg[cur_code.rsrc1] %
				((cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2]);
			break;
		case(REMU):
			_ram.reg[cur_code.rdest] = (uint32_t)_ram.reg[cur_code.rsrc1] %
				(uint32_t)((cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2]);
			break;//deal unsigned


		case(SEQ):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			_ram.reg[cur_code.rdest] = (_ram.reg[cur_code.rsrc1] == tmp) ? 1 : 0;
			break;
		case(SGE):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			_ram.reg[cur_code.rdest] = (_ram.reg[cur_code.rsrc1] >= tmp) ? 1 : 0;
			break;
		case(SGT):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			_ram.reg[cur_code.rdest] = (_ram.reg[cur_code.rsrc1] >= tmp) ? 1 : 0;
			break;
		case(SLE):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			_ram.reg[cur_code.rdest] = (_ram.reg[cur_code.rsrc1] <= tmp) ? 1 : 0;
			break;
		case(SLT):
			if (cur_code.rsrc2 == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.rsrc2];
			if (_ram.reg[cur_code.rsrc1] < tmp) _ram.reg[cur_code.rdest] = 1;
			else _ram.reg[cur_code.rdest] = 0;
			break;
		case(SNE):
			if (cur_code.rsrc2 == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.rsrc2];
			if (_ram.reg[cur_code.rsrc1] != tmp) _ram.reg[cur_code.rdest] = 1;
			else _ram.reg[cur_code.rdest] = 0;
			break;

		case(NEG):
		case(NEGU):
			_ram.reg[cur_code.rdest] = -_ram.reg[cur_code.rsrc1];
			break;

		case(LI):
			_ram.reg[cur_code.rdest] = cur_code.imm;
			break;
		case(MOVE):
			_ram.reg[cur_code.rdest] = _ram.reg[cur_code.rsrc1];
			break;
		case(MFHI):
			_ram.reg[cur_code.rdest] = _ram.reg[HI];
			break;
		case(MFLO):
			_ram.reg[cur_code.rdest] = _ram.reg[LO];
			break;

		case(JR):
			cur_loc = _ram.reg[cur_code.rsrc1] - ins_size;
			break;
		case(JALR):
			_ram.reg[31] = cur_loc + ins_size;
			cur_loc = _ram.reg[cur_code.rsrc1] - ins_size;
			break;

		case(MUL): 
			if (cur_code.offset == 2) {
				int64_t tmp = _ram.reg[cur_code.rdest];
				tmp *= (cur_code.rsrc1 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc1];
				_ram.reg[LO] = (int32_t)(tmp);
				_ram.reg[HI] = tmp >> 32;
			}
			else {
				int64_t tmp = _ram.reg[cur_code.rsrc1];
				tmp *= (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
				_ram.reg[cur_code.rdest] = (int32_t)(tmp);
			}
			break;
		case(MULU)://offset means size 
			if (cur_code.offset == 2) {
				uint64_t tmp = _ram.reg[cur_code.rdest];
				tmp *= (cur_code.rsrc1 == EMPTY) ? (uint32_t) cur_code.imm : (uint32_t)_ram.reg[cur_code.rsrc1];
				_ram.reg[LO] = (uint32_t)(tmp);
				_ram.reg[HI] = tmp >> 32;
			}
			else {
				uint32_t tmp = _ram.reg[cur_code.rsrc1];
				tmp *= (cur_code.rsrc2 == EMPTY) ? (uint32_t)cur_code.imm : (uint32_t)_ram.reg[cur_code.rsrc2];
				_ram.reg[cur_code.rdest] = (uint32_t)(tmp);
			}
			break;
		
		case(DIV):
			if (cur_code.offset == 2) {
				int tmp = (cur_code.rsrc1 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc1];
				_ram.reg[LO] = _ram.reg[cur_code.rdest] / tmp;
				_ram.reg[HI] = _ram.reg[cur_code.rdest] % tmp;
			}
			else {
				int tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
				_ram.reg[cur_code.rdest] = _ram.reg[cur_code.rsrc1] / tmp;
			}
			break;
		case(DIVU):
			if (cur_code.offset == 2) {
				uint32_t tmp = (cur_code.rsrc1 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc1];
				_ram.reg[LO] = ((uint32_t) _ram.reg[cur_code.rdest]) / tmp;
				_ram.reg[HI] = ((uint32_t)_ram.reg[cur_code.rsrc1]) % tmp;
			}
			else {
				uint32_t tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
				_ram.reg[cur_code.rdest] = ((uint32_t)_ram.reg[cur_code.rsrc1]) / tmp;
			}
			break;

		case(B):
		case(J):
			cur_loc = cur_code.label - ins_size;
			break;
		case(JAL):
			_ram.reg[31] = cur_loc + ins_size;
			cur_loc = cur_code.label - ins_size;
			break;

		case(BEQ):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			if (_ram.reg[cur_code.rsrc1] == tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BNE):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			if (_ram.reg[cur_code.rsrc1] != tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BGE):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			if (_ram.reg[cur_code.rsrc1] >= tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BLE):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			if (_ram.reg[cur_code.rsrc1] <= tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BGT):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			if (_ram.reg[cur_code.rsrc1] > tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BLT):
			tmp = (cur_code.rsrc2 == EMPTY) ? cur_code.imm : _ram.reg[cur_code.rsrc2];
			if (_ram.reg[cur_code.rsrc1] < tmp) cur_loc = cur_code.label - ins_size;
			break;

		case(BEQZ):
			if (_ram.reg[cur_code.rsrc1] == 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BNEZ):
			if (_ram.reg[cur_code.rsrc1] != 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BLEZ):
			if (_ram.reg[cur_code.rsrc1] <= 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BGEZ):
			if (_ram.reg[cur_code.rsrc1] >= 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BGTZ):
			if (_ram.reg[cur_code.rsrc1] > 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BLTZ):
			if (_ram.reg[cur_code.rsrc1] < 0) cur_loc = cur_code.label - ins_size;
			break;

		case(LA):
			_ram.reg[cur_code.rdest] = (cur_code.rsrc1 == EMPTY) ?
				cur_code.label :
				cur_code.rsrc1 + cur_code.offset;
			break;
		case(LB):
			_ram.reg[cur_code.rdest] = (cur_code.rsrc1 == EMPTY) ?
				*((int8_t*)(_ram.memory + cur_code.label)) :
				*((int8_t*)(_ram.memory + _ram.reg[cur_code.rsrc1] + cur_code.offset));
			break;
		case(LH):
			_ram.reg[cur_code.rdest] = (cur_code.rsrc1 == EMPTY) ?
				*((int16_t*)(_ram.memory + cur_code.label)) :
				*((int16_t*)(_ram.memory + _ram.reg[cur_code.rsrc1] + cur_code.offset));
			break;
		case(LW):			
			_ram.reg[cur_code.rdest] = (cur_code.rsrc1 == EMPTY) ?
				*((int32_t*)(_ram.memory + cur_code.label)) : 
				*((int32_t*)(_ram.memory + _ram.reg[cur_code.rsrc1] + cur_code.offset));
			break;

		case(SB):
			loc = (cur_code.rdest == EMPTY) ? cur_code.label :  _ram.reg[cur_code.rdest] + cur_code.offset;
			_ram.saveInt(_ram.reg[cur_code.rsrc1], loc, 1);
			break;
		case(SH):
			loc = (cur_code.rdest == EMPTY) ? cur_code.label : _ram.reg[cur_code.rdest] + cur_code.offset;
			_ram.saveInt(_ram.reg[cur_code.rsrc1], loc, 2);
			break;
		case(SW):
			loc = (cur_code.rdest == EMPTY) ? cur_code.label : _ram.reg[cur_code.rdest] + cur_code.offset;
			_ram.saveInt(_ram.reg[cur_code.rsrc1], loc, 4);
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


		cur_loc += ins_size;//step

	}
}



int main(int argc, char *argv[]) {
	ifstream fin;

	//ofstream fout;
	ostream &fout = cout;

	//init(argv[1], fin, fout);
	init(argv[1], fin, fout);



	int ex = simulate(fin, fout);

	fin.close();
	exit(ex);// or return ex

	return 0;
}
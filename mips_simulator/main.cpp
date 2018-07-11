#include "head.hpp"
#include "instruction.hpp"
#include "store.hpp"

extern map<string, int> label;
extern vector<string> forIns;
extern map<string, int> Ins_map;
extern vector<string> forReg;
extern map<string, int> Reg_map;
extern ram _ram;
extern vector<string> Code;

#ifdef DEBUG
void Debug(instruction & _OperatorCode, int & cnt) {
	cout << "Debug: \n{\n" << ++cnt << "\nOperator: " << forIns[_OperatorCode.Ins_type] << '\n';
	for (int i = 0; i < 3; ++i)
		cout << "Register[" << i << "]: " << int(_OperatorCode.regist[i]) << " or " << forReg[_OperatorCode.regist[i]] << '\n';
	cout << "Delta: " << _OperatorCode.offset << '\n'
		<< "Imm :" << _OperatorCode.imm << '\n'
		<< "Label :" << _OperatorCode.label << "\n";
	for (int i = 0; i < 34; ++i) cout << i << '\t'; cout << '\n';
	for (int i = 0; i < 34; ++i) cout << _ram.reg[i] << '\t'; cout << "\n}\n\n";
}
#else
void Debug(instruction & _OperatorCode, int & cnt) {}
#endif


void manageString(string &line, int &cur, int len) {
	
	if (line[cur] == '.') {
		int next = cur, k, cmp, multime = 1;
		while(!skip(line[next])) ++next;
		string key(&line[cur + 1], &line[next]), str;

		//get memory sattled
		switch (key[3]) {// '.' order
		case('g')://align
			k = mstoi(line, cur);
			cmp = 2 << (k - 1);
			while (_ram.heap_top > cmp) {
				cmp += 2 << (k - 1);
				++multime;
			}
			_ram.heap_top = cmp;
			break;
		case('i')://ascii/asciiz
			while(skip(line[next])) ++next;
			cur = next++;
			while( line[next] != '\"') ++next;

			str = string(&line[cur + 1], &line[next]);
			_ram.saveString(str, _ram.heap_top);
			if (key.length() == 6) _ram.memory[_ram.heap_top++] = '\0';
			break;
		case('e')://byte
			do { _ram.saveInt(mstoi(line, cur), _ram.heap_top, 1); }
			while (line[cur++] == ',');
			break;
		case('f')://half
			do {_ram.saveInt(mstoi(line, cur), _ram.heap_top, 2);} 
			while (line[cur++] == ',');
			//for (_ram.saveInt(mstoi(line, cur), _ram.heap_top, 2); line[cur] == ','; _ram.saveInt(mstoi(line, ++cur), _ram.heap_top, 1));
			break;
		case('d')://word
			do {_ram.saveInt(mstoi(line, cur), _ram.heap_top, 4);}
			while (line[cur++] == ',');
			break;
		case('c')://space
			_ram.heap_top += mstoi(line, cur);
			break;
		default://data---text
			return;
		}
	}
	else {//instructions
		int next = cur;
		while(!skip(line[next])) ++next;
		string ins(&line[cur], &line[next]);
		cur = next;
		int _index = Ins_map[ins];

		switch (_index) {
		//First_scanf:
		case ADD: case ADDU: case ADDIU: case SUB: case SUBU: 
		case XOR: case XORU: case REM: case REMU:
		case SEQ: case SGE: case SGT: case SLE: case SLT: case SNE: // Rd R1 R2/Imm
			_ram.saveStruct(line, cur, len, 0, 2, _index, 0); break;
		case NEG: case NEGU: case LI: case MOVE: //Rd R1/Imm
			_ram.saveStruct(line, cur, len, 0, 1, _index, 0); break;
		case MFHI: case MFLO: //Rd
			_ram.saveStruct(line, cur, len, 0, 0, _index, 0); break;
		case JR: case JALR: //R1
			_ram.saveStruct(line, cur, len, 1, 1, _index, 0); break;
		case NOP: case SYSCALL:
			_ram.saveStruct(line, cur, len, 0, -1, _index, 0); break;
		case MUL: case MULU: case DIV: case DIVU: //R1 R2/Imm
			_ram.saveStruct(line, cur, len, 0, 2, _index, 0); break;
		//Second_scanf:
		case B: case J: case JAL: //Label 
			_ram.saveStruct(line, cur, len, 0, -1, _index, 1); break;
		case BEQ: case BNE: case BGE: case BLE: case BGT: case BLT: //R1 R2/Imm Label 
			_ram.saveStruct(line, cur, len, 1, 2, _index, 1); break;
		case 	BEQZ: case BNEZ: case BLEZ: case BGEZ: case BGTZ: case BLTZ: //R1 Label 
			_ram.saveStruct(line, cur, len, 1, 1, _index, 1); break;
		case LA: case LB: case LH: case LW: //Rd Address (Del) R1 -> Rd
			_ram.saveStruct(line, cur, len, 0, 0, _index, 1); break;
		case SB: case SH: case SW: //R1 Address (Del) R1 -> Rd
			_ram.saveStruct(line, cur, len, 1, 1, _index, 1); break;
		}
		}
}


void code_scanf(int second_scanf = 0) {
	string line;
	for (int i = 0; i < Code.size(); ++i) {
		line = Code[i];
		int len = line.length(), cur = 0;

		int annotation = line.find('#');//deal annotation
		if (annotation != -1) {
			line.erase(annotation, len - annotation);
			line.push_back('\n');
			len = annotation + 1;
		}

		while(cur < len &&  skip(line[cur])) ++cur; // "  .data" -> cur: "  ^data"
		if (cur == len) continue;//wrong code

		int pos = line.find(':');
		int ept = line.find('\"');
		while (pos >= 0 && (ept == -1 || pos < ept)) {//deal label
			string label_name(&line[cur], &line[pos]);
			label[label_name] = _ram.heap_top;

			++pos;
			while(pos < len && skip(line[pos])) ++pos;
			line.erase(0, pos);
			cur = 0;
			len -= pos;

			pos = line.find(':');
			ept = line.find('\"');
		}
		if (!len) continue;

		manageString(line, cur, len);//other orders
	}
}

void init(const char * argv, ifstream &fin, ostream &fout) {
	fin.open(argv);

	//prepare()
	for (int i = 0; i < forIns.size(); ++i) Ins_map[forIns[i]] = i;
	for (int i = 0; i < forReg.size(); ++i) Reg_map[forReg[i]] = i;
	Reg_map["s8"] = 30;

	//code_in();
	string tmp;
	while (getline(fin, tmp)) {
		tmp.push_back('\n');
		Code.push_back(tmp);
	}

	code_scanf(0);
	_ram.heap_top = 0;//have to do it twice
	code_scanf(1);
	//_ram.reg[SP] = M - 1;
}

int simulate(ifstream &fin, ostream &fout) {
	int cur_loc = label["main"];
	instruction cur_code;
	int cnt = 0;
	while (1) {
		cur_code.load(cur_loc);

		if (cnt >= 7) Debug(cur_code, cnt);
		else cnt++;

		int tmp;// for compare order
		int cur;//for store order
		switch (cur_code.Ins_type) {
		case(ADD):
		case(ADDU):
		case(ADDIU):
			_ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] + 
				((cur_code.regist[2] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[2]]) ;
			break;

		case(SUB):
		case(SUBU):
			_ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] -
				((cur_code.regist[2] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[2]]);
			break;

		case(XOR):
		case(XORU):
			_ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] -
				((cur_code.regist[2] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[2]]);
			break;

		case(REM):
			_ram.reg[cur_code.regist[0]] = _ram.reg[cur_code.regist[1]] %
				((cur_code.regist[2] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[2]]);
			break;
		case(REMU):
			_ram.reg[cur_code.regist[0]] = (uint32_t) _ram.reg[cur_code.regist[1]] +
				((cur_code.regist[2] == EMPTY) ? (uint32_t)cur_code.imm : (uint32_t)_ram.reg[cur_code.regist[2]]);
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
			cur_loc = _ram.reg[cur_code.regist[1]] - ins_size;
			break;
		case(JALR):
			_ram.reg[31] = cur_loc + ins_size;
			cur_loc = _ram.reg[cur_code.regist[1]] - ins_size;
			break;

		case(MUL):
			if (cur_code.offset == 2) {
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
			if (cur_code.offset == 2) {
				uint64_t tmp = _ram.reg[cur_code.regist[0]];
				if (cur_code.regist[1] == EMPTY) tmp *= (uint32_t)cur_code.imm;
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
			if (cur_code.offset == 2) {
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
			if (cur_code.offset == 2) {
				uint32_t tmp = (cur_code.regist[1] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[1]];
				_ram.reg[LO] = ((uint32_t)_ram.reg[cur_code.regist[0]]) / tmp;
				_ram.reg[HI] = ((uint32_t)_ram.reg[cur_code.regist[0]]) % tmp;
			}
			else {
				uint32_t tmp = (cur_code.regist[2] == EMPTY) ? cur_code.imm : _ram.reg[cur_code.regist[2]];
				_ram.reg[cur_code.regist[0]] = ((uint32_t)_ram.reg[cur_code.regist[1]]) / tmp;
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
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] == tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BNE):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] != tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BGE):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] >= tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BLE):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] <= tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BGT):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] > tmp) cur_loc = cur_code.label - ins_size;
			break;
		case(BLT):
			if (cur_code.regist[2] == EMPTY) tmp = cur_code.imm;
			else tmp = _ram.reg[cur_code.regist[2]];
			if (_ram.reg[cur_code.regist[1]] < tmp) cur_loc = cur_code.label - ins_size;
			break;

		case(BEQZ):
			if (_ram.reg[cur_code.regist[1]] == 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BNEZ):
			if (_ram.reg[cur_code.regist[1]] != 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BLEZ):
			if (_ram.reg[cur_code.regist[1]] <= 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BGEZ):
			if (_ram.reg[cur_code.regist[1]] >= 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BGTZ):
			if (_ram.reg[cur_code.regist[1]] > 0) cur_loc = cur_code.label - ins_size;
			break;
		case(BLTZ):
			if (_ram.reg[cur_code.regist[1]] < 0) cur_loc = cur_code.label - ins_size;
			break;

		case(LA):
			if (cur_code.regist[1] == EMPTY) _ram.reg[cur_code.regist[0]] = cur_code.label;
			else  _ram.reg[cur_code.regist[0]] = cur_code.regist[1] + cur_code.offset;
			break;
		case(LB):
			if (cur_code.regist[1] == EMPTY) _ram.reg[cur_code.regist[0]] = *((int8_t*)(_ram.memory + cur_code.label));
			else  _ram.reg[cur_code.regist[0]] = *((int8_t*)(_ram.memory + _ram.reg[cur_code.regist[1]] + cur_code.offset));
			break;
		case(LH):
			if (cur_code.regist[1] == EMPTY) _ram.reg[cur_code.regist[0]] = *((int16_t*)(_ram.memory + cur_code.label));
			else  _ram.reg[cur_code.regist[0]] = *((int16_t*)(_ram.memory + _ram.reg[cur_code.regist[1]] + cur_code.offset));
			break;
		case(LW):
			if (cur_code.regist[1] == EMPTY) _ram.reg[cur_code.regist[0]] = *((int32_t*)(_ram.memory + cur_code.label));
			else  _ram.reg[cur_code.regist[0]] = *((int32_t*)(_ram.memory + _ram.reg[cur_code.regist[1]] + cur_code.offset));
			break;

		case(SB):
			if (cur_code.regist[0] == EMPTY) cur = cur_code.label;
			else cur = _ram.reg[cur_code.regist[0]] + cur_code.offset;
			_ram.saveInt(_ram.reg[cur_code.regist[1]], cur, 1);
			break;
		case(SH):
			if (cur_code.regist[0] == EMPTY) cur = cur_code.label;
			else cur = _ram.reg[cur_code.regist[0]] + cur_code.offset;
			_ram.saveInt(_ram.reg[cur_code.regist[1]], cur, 2);
			break;
		case(SW):
			if (cur_code.regist[0] == EMPTY) cur = cur_code.label;
			else cur = _ram.reg[cur_code.regist[0]] + cur_code.offset;
			_ram.saveInt(_ram.reg[cur_code.regist[1]], cur, 4);
			break;

		case(NOP):
			break;
		case(SYSCALL):
			switch (_ram.reg[V0]) {
			case(1):
				fout << _ram.reg[A0];
				break;
			case(4): {
				int cur = _ram.reg[A0];
				while (_ram.memory[cur] != '\0') fout << _ram.memory[cur++];
				break;
			}

			case(5):
				cin >> _ram.reg[V0];
				break;
			case(8): {// if {} then can define int, string
				int cur = _ram.reg[A0];
				string str;
				cin >> str;
				if (str.length() > _ram.reg[A1] - 1)
					//str = str.substr(0, _ram.reg[A1] - 1);
					str = string(&str[0], &str[_ram.reg[A1] - 1]);
				_ram.saveString(str, cur);
				break;
			}
			case(9):
				_ram.reg[V0] = _ram.heap_top;
				_ram.heap_top += _ram.reg[A0];
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

//#define DEBUG

int main(int argc, char *argv[]) {
	ifstream fin;

	//ofstream fout;
	ostream &fout = cout;
    //cout << "haha\n";
	init(argv[1], fin, fout);
	//init("..\\test\\data\\1.s", fin, fout);



	int ex = simulate(fin, fout);
    
	fin.close();
	// or exit(ex);

	return ex;
}
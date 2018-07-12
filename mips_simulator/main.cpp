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
		while (!skip(line[next])) ++next;
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
			while (skip(line[next])) ++next;
			cur = next++;
			while (line[next] != '\"') ++next;

			str = string(&line[cur + 1], &line[next]);
			_ram.saveString(str, _ram.heap_top);
			if (key.length() == 6) _ram.memory[_ram.heap_top++] = '\0';
			break;
		case('e')://byte
			do { _ram.saveInt(mstoi(line, cur), _ram.heap_top, 1); } while (line[cur++] == ',');
			break;
		case('f')://half
			do { _ram.saveInt(mstoi(line, cur), _ram.heap_top, 2); } while (line[cur++] == ',');
			//for (_ram.saveInt(mstoi(line, cur), _ram.heap_top, 2); line[cur] == ','; _ram.saveInt(mstoi(line, ++cur), _ram.heap_top, 1));
			break;
		case('d')://word
			do { _ram.saveInt(mstoi(line, cur), _ram.heap_top, 4); } while (line[cur++] == ',');
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
		while (!skip(line[next])) ++next;
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

		while (cur < len &&  skip(line[cur])) ++cur; // "  .data" -> cur: "  ^data"
		if (cur == len) continue;//wrong code

		int pos = line.find(':');
		int ept = line.find('\"');
		while (pos >= 0 && (ept == -1 || pos < ept)) {//deal label
			string label_name(&line[cur], &line[pos]);
			label[label_name] = _ram.heap_top;

			++pos;
			while (pos < len && skip(line[pos])) ++pos;
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


#ifdef PIPELINE

IF I_F;
ID I_D;
EX EX_;
MA M_A;
WB W_B;

bool SIMULATOR = true;
int PC, return_value = 0;


void Instruction_Fetch(ostream &fout){
	if (!I_F.block && !I_F.jump_block) {
		I_F.push(PC);
		PC += ins_size;

		switch (I_F.Ins_type) {
		case JR: case JALR:
		case B: case J: case JAL:
		case BEQ: case BNE: case BGE: case BLE: case BGT: case BLT:
		case BEQZ: case BNEZ: case BLEZ: case BGEZ: case BGTZ: case BLTZ:
			I_F.jump_block = 1;
			break;
		default :
			break;
		}

		I_D.push(I_F);
	}
}

void Stop_Instruction_Decode(){
	I_D.block = 1;
	I_F.block = 1;
}
void Instruction_Decode(ostream &fout){
	if (I_D.Ins_type == -1) return;

	I_D.block = I_F.jump_block = 0;
	for (int i = 0; i < 3; ++i) I_D.tovis[i] = 0;

	switch (I_D.Ins_type) {
	case ADD: case ADDU: case ADDIU: case SUB: case SUBU: case XOR: case XORU: case REM: case REMU:
	case SEQ: case SGE: case SGT: case SLE: case SLT: case SNE:  // Rd R1 R2/Imm
	case BEQ: case BNE: case BGE: case BLE: case BGT: case BLT:  //R1 R2/Imm Label 
		I_D.tovis[1] = I_D.tovis[2] = 1;
		break;
	case NEG: case NEGU: case MOVE: //case LI:  //Rd R1/Imm
	case JR: case JALR:  //R1
	case BEQZ: case BNEZ: case BLEZ: case BGEZ: case BGTZ: case BLTZ:  //R1 Label 
	case LA: case LB: case LH: case LW:  //Rd Address (Del) R1 -> Rd
		I_D.tovis[1] = 1;
		break;
	case MFHI: 
		if (_ram.vis[HI]) {
			Stop_Instruction_Decode();
			return;
		}
		else {
			I_D.imm = _ram.reg[HI];
		}
		break;
	case MFLO:  //Rd
		if (_ram.vis[LO]) {
			Stop_Instruction_Decode();
			return;
		}
		else {
			I_D.imm = _ram.reg[LO];
		}
		break;
	case MUL: case MULU: case DIV: case DIVU:  //R1 R2/Imm
		if (I_D.offset == 2) I_D.tovis[0] = I_D.tovis[1] = 1;
		else I_D.tovis[1] = I_D.tovis[2] = 1;
		break;
	case SB: case SH: case SW: //R1 Address (Del) R1 -> Rd
		I_D.tovis[0] = I_D.tovis[1] = 1;
		break;
	//case B: case J: case JAL:  //Label 
	//case NOP: 
	case SYSCALL:
		if (_ram.vis[V0]) {
			Stop_Instruction_Decode();
			return;
		}
		else {
			I_D.offset = _ram.reg[V0];
			switch (I_D.offset) {
			case(1):
				if (_ram.vis[A0]) {
					Stop_Instruction_Decode();
					return;
				}
				else {
					I_D.imm = _ram.reg[A0];
				}
				break;
			case(4):
				if (_ram.vis[A0]) {
					Stop_Instruction_Decode();
					return;
				}
				else {
					I_D.label = _ram.reg[A0];
				}
				break;
			case(8):
				if (_ram.vis[A0] || _ram.vis[A1]) {
					Stop_Instruction_Decode();
					return;
				}
				else {
					I_D.imm = _ram.reg[A1];
					I_D.label = _ram.reg[A0];
				}
				break;
			case(9):
				if (_ram.vis[A0]) {
					Stop_Instruction_Decode();
					return;
				}
				else {
					I_D.imm = _ram.reg[A0];
				}
				break;
			case(17):
				if (_ram.vis[A0]) {
					Stop_Instruction_Decode();
					return;
				}
				else {
					I_D.imm = _ram.reg[A0];
				}
				break;
			default://case 10, 5
				break;
			}
		}
	default:
		break;
	}

	for (int i = 0; i < 3; ++i) {
		if (I_D.tovis[i] && I_D.regist[i] != EMPTY && _ram.vis[I_D.regist[i]]) {
			Stop_Instruction_Decode();
			return;
		}
	}

	if (!I_D.block) {
		for (int i = 0; i < 3; ++i) {
			if (I_D.tovis[i]) {
				if (I_D.regist[i] == EMPTY) I_D.reg[i] = I_D.imm;
				else I_D.reg[i] = _ram.reg[I_D.regist[i]];
			}
		}

		switch (I_D.Ins_type) {
		case ADD: case ADDU: case ADDIU: case SUB: case SUBU: case XOR: case XORU: case REM: case REMU:
		case SEQ: case SGE: case SGT: case SLE: case SLT: case SNE:  // Rd R1 R2/Imm
		case NEG: case NEGU: case LI: case MOVE:  //Rd R1/Imm
		case MFHI: case MFLO:  //Rd
		case LA: case LB: case LH: case LW:  //Rd Address (Del) R1 -> Rd
			_ram.vis[I_D.regist[0]] = 1;
			break;
		case JR: case JALR:  //R1
			_ram.vis[31] = 1;
			break;	
		case MUL: case MULU: case DIV: case DIVU:  //R1 R2/Imm
			if (I_D.offset == 2) _ram.vis[LO] = _ram.vis[HI] = 1;
			else _ram.vis[I_D.regist[0]] = 1;
			break;
		case SYSCALL:
			switch (I_D.offset) {
			case(5):
			case(9):
				_ram.vis[V0] = 1;
				break;
			default: break;
			}
		//case NOP: 
		//case B: case J: case JAL:  //Label 
		//case BEQ: case BNE: case BGE: case BLE: case BGT: case BLT:  //R1 R2/Imm Label 
		case BEQZ: case BNEZ: case BLEZ: case BGEZ: case BGTZ: case BLTZ:  //R1 Label 
			I_D.reg[2] = 0;
			break;
		//case SB: case SH: case SW: //R1 Address (Del) R1 -> Rd
		default: break;
		}
	}
	EX_.push(I_D);
}

void Execution(ostream &fout){
	if (EX_.Ins_type == -1) return;
	switch (EX_.Ins_type) {
	case ADD: case ADDU: case ADDIU: EX_.imm = EX_.reg[1] + EX_.reg[2]; break;
	case SUB: case SUBU: EX_.imm = EX_.reg[1] - EX_.reg[2]; break;
	case XOR: case XORU: EX_.imm = EX_.reg[1] ^ EX_.reg[2]; break;
	case REM: EX_.imm = EX_.reg[1] % EX_.reg[2]; break;
	case REMU: EX_.imm = (uint32_t) EX_.reg[1] % (uint32_t) EX_.reg[2]; break;
	case SEQ: EX_.imm = EX_.reg[1] == EX_.reg[2]; break;
	case SGE: EX_.imm = EX_.reg[1] >= EX_.reg[2]; break;
	case SGT: EX_.imm = EX_.reg[1] > EX_.reg[2]; break;
	case SLE: EX_.imm = EX_.reg[1] <= EX_.reg[2]; break;
	case SLT: EX_.imm = EX_.reg[1] < EX_.reg[2]; break;
	case SNE: EX_.imm = EX_.reg[1] != EX_.reg[2]; break;
	case NEG: case NEGU: EX_.imm = -EX_.reg[1]; break;
	case LI: case MOVE:  EX_.imm = EX_.reg[1]; break;
	case MFHI: EX_.imm = _ram.reg[HI]; break;
	case MFLO: EX_.imm = EX_.reg[LO]; break;
	case JR: case JALR:  EX_.imm = EX_.reg[1]; break;
	case MUL: 
		if (EX_.offset == 2) {
			int64_t tmp = (int64_t)EX_.reg[0] * (int64_t)EX_.reg[1];
			EX_.imm = tmp;
			tmp >>= 32;
			EX_.label = tmp;
		}
		else {
			int64_t tmp = (int64_t)EX_.reg[1] * (int64_t)EX_.reg[2];
			EX_.imm = tmp;
		}
		break;
	case MULU:
		if (EX_.offset == 2) {
			uint64_t tmp = (uint64_t)EX_.reg[0] * (uint64_t)EX_.reg[1];
			EX_.imm = tmp;
			tmp >>= 32;
			EX_.label = tmp;
		}
		else {
			uint64_t tmp = (uint64_t)EX_.reg[1] * (uint64_t)EX_.reg[2];
			EX_.imm = tmp;
		}
		break;
	case DIV:
		if (EX_.offset == 2) {
			EX_.imm = EX_.reg[0] / EX_.reg[1];
			EX_.label = EX_.reg[0] % EX_.reg[1];
		}
		else {
			EX_.imm = EX_.reg[0] / EX_.reg[1];
		}
		break;
	case DIVU:
		if (EX_.offset == 2) {
			EX_.imm = (uint32_t)EX_.reg[0] / (uint32_t)EX_.reg[1];
			EX_.label = (uint32_t)EX_.reg[0] % (uint32_t)EX_.reg[1];
		}
		else {
			EX_.imm = (uint32_t)EX_.reg[0] / (uint32_t)EX_.reg[1];
		}
		break;
	case BEQ: case BEQZ: EX_.imm = (EX_.reg[1] == EX_.reg[2]); break;
	case BNE: case BNEZ: EX_.imm = (EX_.reg[1] != EX_.reg[2]); break;
	case BGE: case BGEZ: EX_.imm = (EX_.reg[1] >= EX_.reg[2]); break;
	case BLE: case BLEZ: EX_.imm = (EX_.reg[1] <= EX_.reg[2]); break;
	case BGT: case BGTZ: EX_.imm = (EX_.reg[1] > EX_.reg[2]); break;
	case BLT: case BLTZ: EX_.imm = (EX_.reg[1] < EX_.reg[2]); break;
	case LA: case LB: case LH: case LW: 
		if (EX_.regist[1] != EMPTY) EX_.label = EX_.reg[1] + EX_.offset;
		break;
	case SB: case SH: case SW:
		if (EX_.regist[0] != EMPTY) EX_.label = EX_.reg[0] + EX_.offset;
		EX_.imm = EX_.reg[1];
		break;
	//case B: case J: case JAL:  //Label 
	//case NOP: case SYSCALL: 
	default :
		break;
	}
	M_A.push(EX_);
}

void Memory_Access(ostream &fout){
	if (M_A.Ins_type == -1) return;
	int loc;
	string str;
	switch (M_A.Ins_type) {
	case LA: M_A.imm = M_A.label; break;
	case LB: M_A.imm = *((int8_t*)(_ram.memory + M_A.label)); break;
	case LH: M_A.imm = *((int16_t*)(_ram.memory + M_A.label)); break;
	case LW: M_A.imm = *((int32_t*)(_ram.memory + M_A.label)); break;
	case SB: loc = M_A.label; _ram.saveInt(M_A.imm, loc, 1); break;
	case SH: loc = M_A.label; _ram.saveInt(M_A.imm, loc, 2); break;
	case SW: loc = M_A.label; _ram.saveInt(M_A.imm, loc, 4); break;
	case SYSCALL:
		switch (M_A.offset) {
		case(1):
			fout << M_A.imm;
		case(4):
			loc = M_A.label;
			while (_ram.memory[loc] != '\0') {
				fout << _ram.memory[loc];
				++loc;
			}
			break;
		case(5):
			cin >> M_A.imm;
		case(8):
			getline(cin, str);
			if (str.length() > M_A.imm - 1) str = string(&str[0], &str[M_A.imm - 1]);
			loc = M_A.label;
			_ram.saveString(str, loc);
			break;
		case(9):
			M_A.label = _ram.heap_top;
			_ram.heap_top += M_A.imm;
			break;
		default: break;
		}
	default: break;
	//case NOP:
	//case ADD: case ADDU: case ADDIU: case SUB: case SUBU: case XOR: case XORU: case REM: case REMU:
	//case SEQ: case SGE: case SGT: case SLE: case SLT: case SNE:  // Rd R1 R2/Imm
	//case NEG: case NEGU: case LI: case MOVE:  //Rd R1/Imm
	//case MFHI: case MFLO:  //Rd
	//case JR: case JALR:  //R1

	//case MUL: case MULU: case DIV: case DIVU:  //R1 R2/Imm
	//										   //Second_scanf:
	//case B: case J: case JAL:  //Label 
	//case BEQ: case BNE: case BGE: case BLE: case BGT: case BLT:  //R1 R2/Imm Label 
	//case BEQZ: case BNEZ: case BLEZ: case BGEZ: case BGTZ: case BLTZ:  //R1 Label 
	}
	W_B.push(M_A);
}

void Write_Back(ostream &fout){
	if (W_B.Ins_type == -1) return;
	switch (W_B.Ins_type) {
		//First_scanf:
	case ADD: case ADDU: case ADDIU: case SUB: case SUBU: case XOR: case XORU: case REM: case REMU:
	case SEQ: case SGE: case SGT: case SLE: case SLT: case SNE:  // Rd R1 R2/Imm
	case NEG: case NEGU: case LI: case MOVE:  //Rd R1/Imm
	case MFHI: case MFLO:  //Rd
	case LA: case LB: case LH: case LW:  //Rd Address (Del) R1 -> Rd
		_ram.reg[W_B.regist[0]] = W_B.imm;
		_ram.vis[W_B.regist[0]] = 0;
		break;
	case JALR:  //R1
		_ram.reg[31] = PC;
		_ram.vis[31] = 0;
	case JR: 
		PC = W_B.label;
		I_F.jump_block = 0;
		break;
	case MUL: case MULU: case DIV: case DIVU:  //R1 R2/Imm
		if (W_B.offset == 2) {
			_ram.reg[LO] = W_B.imm;
			_ram.reg[HI] = W_B.label;
			_ram.vis[LO] = _ram.vis[HI] = 0;
		}
		else {
			_ram.reg[W_B.regist[0]] = W_B.imm;
			_ram.vis[W_B.regist[0]] = 0;
		}
		break;
	case JAL:
		_ram.reg[31] = PC;
		_ram.vis[31] = 0;
	case B: case J:  //Label 
		PC = W_B.label;
		I_F.jump_block = 0;
		break;
	case BEQ: case BNE: case BGE: case BLE: case BGT: case BLT:  //R1 R2/Imm Label 
	case BEQZ: case BNEZ: case BLEZ: case BGEZ: case BGTZ: case BLTZ:  //R1 Label 
		if (W_B.imm) PC = W_B.label;
		I_F.jump_block = 0;
		break;
	case SYSCALL:
		switch (W_B.offset) {
		case(5):
			_ram.reg[V0] = W_B.imm;
			_ram.vis[V0] = 0;
			break;
		case(9):
			_ram.reg[V0] = W_B.label;
			_ram.vis[V0] = 0;
		case(10):
			SIMULATOR = 0;
			return;
		case(17):
			SIMULATOR = 0;
			return_value = W_B.imm;
			return;
		default: break;
		}
		break;
	default: break;
	//	case SB: case SH: case SW: //R1 Address (Del) R1 -> Rd
	//	case NOP: 
	}
	W_B.clear();
}

int simulate(ostream &fout) {
	PC = label["main"];
	int cnt = 0;
	while (SIMULATOR) {
		Instruction_Fetch(fout);
		Debug(I_F, cnt);
		Instruction_Decode(fout);
		Debug(I_D, cnt);
		Execution(fout);
		Debug(EX_, cnt);
		Memory_Access(fout); 
		Debug(M_A, cnt);
		Write_Back(fout);
		Debug(W_B, cnt);
	}

	return return_value;
}

#else

int simulate(ostream &fout) {
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
#endif
//#define DEBUG

int main(int argc, char *argv[]) {
	ifstream fin;

	//ofstream fout;
	ostream &fout = cout;
    //cout << "haha\n";
	init(argv[1], fin, fout);
	//init("..\\test\\data\\1.s", fin, fout);



	int ex = simulate(fout);
    
	fin.close();
	// or exit(ex);

	return ex;
}
#ifndef MIPS_PREPARE_H
#define MIPS_PREPARE_H

#include <iostream>
#include <cstdio>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#define M 10000000
#define NXT 16
using namespace std;

map<string, int> Label;

//�ܴ洢�����ݡ����
char MEMORY[M];
int REGISTER[34] = { 0 };
bool vis[34] = { 0 }, SIMULATOR = true;
int MEM_LOW = 0, PC, SIMULATOR_RET = 0;

//�洢����
vector<string> Code;

//��д
ifstream fin;
//ifstream fin("code.s");
//ofstream cout("code.out");
//ifstream cin("data.in");

//������ӳ��
vector<string> forOperator = {
	"add", "addu", "addiu", "sub", "subu", "xor", "xoru", "rem", "remu", "seq", "sge", "sgt", "sle", "slt", "sne",
	"neg", "negu", "li", "move",
	"mfhi", "mflo",
	"jr", "jalr",
	"nop", "syscall",
	"mul", "mulu", "div", "divu",
	"b", "j", "jal",
	"beq", "bne", "bge", "ble", "bgt", "blt",
	"beqz", "bnez", "blez", "bgez", "bgtz", "bltz",
	"la", "lb", "lh", "lw",
	"sb", "sh", "sw"
};

enum MIPS_OPERATOR {
	//First_scanf:
	ADD, ADDU, ADDIU, SUB, SUBU, XOR, XORU, REM, REMU, SEQ, SGE, SGT, SLE, SLT, SNE, // Rd R1 R2/Imm
	NEG, NEGU, LI, MOVE, //Rd R1/Imm
	MFHI, MFLO, //Rd
	JR, JALR, //R1
	NOP, SYSCALL,
	MUL, MULU, DIV, DIVU, //R1 R2/Imm
	//Second_scanf:
	B, J, JAL, //Label 
	BEQ, BNE, BGE, BLE, BGT, BLT, //R1 R2/Imm Label 
	BEQZ, BNEZ, BLEZ, BGEZ, BGTZ, BLTZ, //R1 Label 
	LA, LB, LH, LW, //Rd Address (Del) R1 -> Rd
	SB, SH, SW //R1 Address (Del) R1 -> Rd				  
};
map<string, int> OperatorIndex; 

//�Ĵ���ӳ��
vector<string> forRegister = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2" ,"s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1",
	"gp", "sp", "fp", "ra", "lo", "hi", "EMPTY"
};
enum MIPS_REGISTER {
	ZERO, AT, V0, V1, A0, A1, A2, A3,
	T0, T1, T2, T3, T4, T5, T6, T7,
	S0, S1, S2, S3, S4, S5, S6, S7,
	T8, T9, K0, K1,
	GP, SP, FP, RA, LO, HI, EMPTY
};
const int S8 = 30;
map<string, int> RegisterIndex; 

//������
class OperatorCode { //����洢�� 
public:
	char operatorIndex; //�������
						//��żĴ�����������1��������2
	char Register[3];
	int Imm, Label, Delta;
	OperatorCode(char index = 255) :operatorIndex(index) {
		for (int i = 0; i < 3; ++i) Register[i] = EMPTY;
		Delta = 0;
	}
	void load(int loc) {
		operatorIndex = MEMORY[loc++];
		for (int i = 0; i < 3; ++i) {
			Register[i] = MEMORY[loc++];
		}
		Imm = *((int*)(MEMORY + loc));
		Label = *((int*)(MEMORY + loc + 4));
		Delta = *((int*)(MEMORY + loc + 8));
	}
	OperatorCode & operator=(OperatorCode & obj) {
		if (this == &obj) return *this;
		this->operatorIndex = obj.operatorIndex;
		for (int i = 0; i < 3; ++i) this->Register[i] = obj.Register[i];
		this->Imm = obj.Imm, this->Delta = obj.Delta, this->Label = obj.Label;
		return *this;
	}
	void clear() {
		operatorIndex = -1;
		for (int i = 0; i < 3; ++i) Register[i] = EMPTY;
		Imm = Label = Delta = 0;
		return;
	}
};

class WriteBack {
public:
	OperatorCode _OperatorCode;
	void load(OperatorCode & obj) {
		_OperatorCode = obj;
		obj.clear();
	}
	void go();
}_WriteBack;

class MemoryAccess {
public:
	OperatorCode _OperatorCode;
	void load(OperatorCode & obj) {
		_OperatorCode = obj;
		obj.clear();
	}
	void go();
}_MemoryAccess;

class Execution {
public:
	OperatorCode _OperatorCode;
	int Register[3];
	void load(OperatorCode & obj, int r[]) {
		_OperatorCode = obj;
		for (int i = 0; i < 3; ++i) Register[i] = r[i];
		obj.clear();
	}
	void go();
}_Execution;

class DataPreparation {
public:
	OperatorCode _OperatorCode;
	int Register[3];
	bool tovis[3], block;
	void load(OperatorCode & obj) {
		_OperatorCode = obj;
		obj.clear();
	}
	void go();
	void Stop_DataPreparation(); 
}_DataPreparation;

class InstructionFetch {
public:
	OperatorCode _OperatorCode;
	bool block = false, jump_block = false;
	void go() {
		if (!block && !jump_block) {
			_OperatorCode.load(PC);
			PC += 16;
			if ((_OperatorCode.operatorIndex >= JR && _OperatorCode.operatorIndex <= JALR) || (_OperatorCode.operatorIndex >= B && _OperatorCode.operatorIndex <= BLTZ))
				jump_block = true;
			_DataPreparation.load(_OperatorCode);
		}
	}
}_InstructionFetch;

//########################################  ת��  ################################################

int toInt(string & line, int & loc, int len); //string�д�loc��ȡ����

int toRInt(string & line, int & loc, int len); //string�д�loc��ȡ�Ĵ������

void saveInt(int x, int & loc, int ord); //��int����char����

void saveString(string & str, int & loc); //��string����char����

void saveStruct(string & line, int & loc, int len, int l, int r, int index, bool label); //��locλ�ù�����������char����
																						 
//#######################################  Ԥ����  ###############################################

void MIPS_PRETREATMENT(); //Ԥ����

void code_in();

void prepare(); //�����Ĵ����Ͳ�����mapӳ��

void code_scanf();

void manageString(string & line, int & loc, int & len); //������У�����ǰ�ÿո�

//######################################  Pipeline  ################################################

int MIPS_SIMULATOR();

void Debug(OperatorCode & _OperatorCode, int & cnt, int REGISTER[]) {
	if(_OperatorCode.operatorIndex)cout << "Debug: \n{\n" << ++cnt << "\nOperator: " << forOperator[_OperatorCode.operatorIndex] << '\n';
	else cout << "Debug: \n{\n" << ++cnt << "\nOperator: EMPTY" << '\n';
	for (int i = 0; i < 3; ++i)
		cout << "Register[" << i << "]: " << int(_OperatorCode.Register[i]) << " or " << forRegister[_OperatorCode.Register[i]] << '\n';
	cout << "Delta: " << _OperatorCode.Delta << '\n'
		<< "Imm :" << _OperatorCode.Imm << '\n'
		<< "Label :" << _OperatorCode.Label << "\n";
	for (int i = 0; i < 34; ++i) cout << i << '\t'; cout << '\n';
	for (int i = 0; i < 34; ++i) cout << REGISTER[i] << '\t'; cout << "\n}\n\n";
}

#endif // !MIPS_PREPARE

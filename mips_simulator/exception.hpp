#pragma once

#include <exception>
//#include <stdexcept>

using namespace std;
class file_error : public exception {};
class not_exist : public exception {};
class order_error : public exception {};

/*

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
int n = mstoi(tmp);
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
int n = mstoi(tmp);
memcpy(_memory.mem + _memory.heap_top, &n, m);
}
}
else if (tmp == "space") {
++i;
tmp = get_phrase(str, i);
int n = mstoi(tmp);
_memory.heap_top += n;
}
else if (tmp == "data" || tmp == "text") {
text_block = (tmp == "text");
}
}
else if (str.back() == ':') {
string tmp = get_phrase(str, i);
if (text_block) text_label[tmp] = ins_cnt;
else [tmp] = _memory.heap_top;
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


*/
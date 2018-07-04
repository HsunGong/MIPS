#pragma once
#include "Header.h"
#include "storage.hpp"


enum order_type {
	align,
	ascii,
	asciiz,
	byte,
	half,
	word,
	space,
	data,
	text
};

class order {
protected:
	order_type type;
	int32_t pos;
	order(order_type &t, int32_t &p) :type(t), pos(p){ }
	virtual void execute(){}
	virtual ~order(){}
};

class align :public order {
	uint32_t x;
	align(order_type &t, int32_t &p, uint32_t &n): order(t, p), x(x){}
	virtual void execute() {

	}
	virtual ~align(){}
};
class ascii :public order {
	string x;
	ascii(order_type &t, int32_t &p, string &x) : order(t, p), x(x + "\n") {}
	virtual void execute() {

	}
	virtual ~ascii() {}
};
class asciiz :public order {
	string x;
	asciiz(order_type &t, int32_t &p, string &x) : order(t, p), x(x) {}
	virtual void execute() {

	}
	virtual ~asciiz() {}
};

class byte :public order {
	vector<int8_t> x;
	byte(order_type &t, int32_t &p, vector<int8_t> &x) : order(t, p), x(x) {}
	virtual void execute() {

	}
	virtual ~byte() {}
};

class half :public order {
	vector<int16_t> x;
	half(order_type &t, int32_t &p, vector<int16_t> &x) : order(t, p), x(x) {}
	virtual void execute() {

	}
	virtual ~half() {}
};
class half :public order {
	int16_t x;
	half(order_type &t, int32_t &p, int16_t &x) : order(t, p), x(x) {}
	virtual void execute() {

	}
	virtual ~half() {}
};
class word :public order {
	vector<int32_t> x;
	word(order_type &t, int32_t &p, vector<int32_t> &x) : order(t, p), x(x) {}
	virtual void execute() {

	}
	virtual ~word() {}
};
class space :public order {
	space(order_type &t, int32_t &p) : order(t, p) {}
	virtual void execute() {

	}
	virtual ~space() {}
};

class data :public order {
	data(order_type &t, int32_t &p) : order(t, p) {}
	virtual void execute() {

	}
	virtual ~data() {}
};
class text :public order {
	text(order_type &t, int32_t &p) : order(t, p) {}
	virtual void execute() {

	}
	virtual ~text() {}
};






vector<order*> _order;



//pretreatment
void pre_orders(ifstream &in) {
	prepare(in);

}


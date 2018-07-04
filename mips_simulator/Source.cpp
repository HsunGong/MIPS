#include "Header.h"




int main(int argc, char *argv[]) {
	ifstream in(argv[1]);
	if (!in) throw file_error();

	pre_sentence(in);

	in.close();
	return 0;
}
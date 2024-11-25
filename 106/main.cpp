#include "include/praktable.hpp"

using table = prak::table<double>;

void ex1(void) {
	table t;
	t.print();
}

int main() {
	ex1();	
	return 0;
}

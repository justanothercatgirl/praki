#include <iostream>

#include "include/praktable.hpp"

using table = prak::table<f64>;
using f64p = prak::pvalue<f64>;
using f64v = std::vector<f64>;
using vecarg = const std::vector<f64> &;
using argvec = const std::vector<f64> &;

void ex1(std::string file) {
	table t(file);
}

int main() {
	ex1("data");	
	return 0;
}
